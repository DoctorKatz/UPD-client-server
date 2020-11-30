use std::net::UdpSocket;
use std::{str, fs};
use rand::Rng;
use serde::{Deserialize, Serialize};
use crc32c;
use bincode;

const MAX_DATA: usize = 23;//22
const PACKAGE_SIZE: usize = MAX_DATA + 17;
const ACK: u8 = 0;
const PUT: u8 = 1; //TODO: check it in protocol


fn deserialize_arr_to_u32(arr: [u8;4]) -> u32{

     let number = u32::from(arr[0]) << 24 | u32::from(arr[1]) << 16
     | u32::from(arr[2]) << 8
     | u32::from(arr[3]);
    number
}
//padding off
#[derive(Serialize, Deserialize)]
struct PackageData{
    seq_number: u32,
    seq_total: u32,
    i_type: u8,
    id: [u8; 8],
    data:Vec<u8>
}
#[derive(Serialize, Deserialize, PartialEq, Debug)]
#[repr(C)]
struct RequestData{
    seq_number: u32,
    seq_total: u32,
    i_type: u8,
    id: [u8; 8],
    data:[u8;4]
}

impl PackageData{
    pub fn new(seq_number: u32, seq_total: u32, i_type: u8, id: [u8; 8], data:Vec<u8>) -> Option<PackageData>{
        Some(PackageData {
            seq_number,
            seq_total,
            i_type: 0,
            id,
            data
        })
    }

    pub fn get_file(filename: String, id_arr: &[u8;8]) -> Vec<PackageData>{
        let mut contents = fs::read_to_string(filename)
            .expect("Something went wrong reading the file");
        let mut len = contents.len();
        let mut seq_number: u32 = 0;
        let mut seq_total: u32;
        let length = len % MAX_DATA;
        if len % MAX_DATA == 0 {
            seq_total = ((len / MAX_DATA)) as u32;
        }
        else { seq_total = ((len / MAX_DATA) + 1) as u32; }
        let mut file:Vec<PackageData> = vec![];

        while len > MAX_DATA {
            let sub_context = contents.split_off(MAX_DATA);
            len = sub_context.len();
            let package = PackageData::new(seq_number, seq_total, PUT,
                                           *id_arr, contents.into_bytes()).expect("Can't make object");
            file.push(package);
            seq_number+=1;
            contents = sub_context;
        }
        if contents.len() > 0 {
            let package = PackageData::new(seq_number, seq_total, PUT, *id_arr, contents.into_bytes()).expect("Can't make object");
            file.push(package);
        }
        file
    }

    pub fn get_seq_total(&self) -> u32{
        self.seq_total
    }
    pub fn get_number(&self) -> u32{
        self.seq_number
    }
    pub fn get_id(&self) -> [u8; 8] {
        self.id
    }

}

fn main() {

    let id:[u8; 8] = [0,0,0,0,0,0,0,1];
    let id_2:[u8; 8] = [0,0,0,0,0,0,1,1];

    let mut files:Vec<Vec<PackageData>> = Vec::new();
    files.push(PackageData::get_file("test.txt".to_string(), &id));
    files.push(PackageData::get_file("test_1.txt".to_string(), &id_2));
    //let mut file:Vec<PackageData> = PackageData::get_file("test.txt".to_string(), &id);
    //file = PackageData::get_file("test_1.txt".to_string(), &id_2);
    let mut rng = rand::thread_rng();

    let socket = UdpSocket::bind("127.0.0.1:8000").expect("Could not bind client socket");
    socket.connect("127.0.0.1:8888").expect("Could not connect to server");

    let mut num_file:u32 = rng.gen_range(0, files.len() as u32);
    let mut num_pack:u32  = rng.gen_range(0, files[num_file as usize].len() as u32);
    loop {
        let mut buffer =[0u8;40];

        let mut bytes = bincode::serialize(&files[num_file as usize][num_pack as usize]).unwrap();

        bytes = [&bytes[0..17], &bytes[25..]].concat();
        socket.send(&bytes).expect("Failed to write to server");
        let mut crc_package= crc32c::crc32c(&bytes[17..]);

        if bytes.len() < PACKAGE_SIZE {
            bytes.resize(PACKAGE_SIZE + 1, 0);
            crc_package = crc32c::crc32c(&bytes[17..PACKAGE_SIZE]);
        }




        socket.recv_from(&mut buffer).expect("Could not read into buffer");

        let decoded: RequestData = bincode::deserialize(&buffer).unwrap();
        println!("Num pack {}", decoded.seq_number.to_string());
        println!("file_id{}", str::from_utf8(&decoded.id).unwrap() );
        println!("CRC_client {}", crc_package);

        //num = rng.gen_range(0, file.len());

        println!("CRC from server {}",deserialize_arr_to_u32(decoded.data.clone()));
       if deserialize_arr_to_u32(decoded.data.clone()) == crc_package{
           num_file = rng.gen_range(0, files.len() as u32);
           num_pack  = rng.gen_range(0, files[num_file as usize].len() as u32);
        }
        if (decoded.seq_number == files[num_file as usize][1].seq_total)
            && decoded.id == files[num_file as usize][1].id{
            println!("EXIT");
            break;
        }
    }
}

