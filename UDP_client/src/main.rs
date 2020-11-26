use std::net::UdpSocket;
use std::{str, io, fs, mem};
use std::slice;
use rand::Rng;
use serde::{Deserialize, Serialize};
use std::io::Read;
use byteorder::{BigEndian, WriteBytesExt, LittleEndian};
use crc32c;
use bincode;

/*use std::io;

fn main() {
    println!("Start UDP client\nWrite IP address\n");
    let mut addr = String::new();
    io::stdin().read_line(&mut addr).expect("Can't read string");
    udp_client(addr);
}

fn udp_client(addr: String,  ){
    use std::net::UdpSocket;

    let mut input = String::new;
    let mut buf = [0; 1500];

    let socket = UdpSocket::bind(addr.clone()).expect("couldn't bind to address");
    socket.send(addr.clone()).expect("connect function failed");

    match socket.send(&mut buf) {
        Ok(received) => println!("received {} bytes {:?}", received, &buf[..received]),
        Err(e) => println!("recv function failed: {:?}", e),
    }
}*/

/*fn main() {
    let contents = fs::read_to_string("test.txt").expect("Can't read file");
    println!("{}",contents);

    let socket = UdpSocket::bind("127.0.0.1:8000").expect("Could not bind client socket");
    socket.connect("127.0.0.1:8888").expect("Could not connect to server");
    loop {
        let mut input = String::new();
        let mut buffer = [0u8; 1500];
        io::stdin().read_line(&mut input).expect("Failed to read from stdin");
        socket.send(input.as_bytes()).expect("Failed to write to server");

        socket.recv_from(&mut buffer).expect("Could not read into buffer");
        print!("{}", str::from_utf8(&buffer).expect("Could not write buffer as string"));
    }
}*/


const MAX_DATA: usize = 35;
const ACK: u8 = 0;
const PUT: u8 = 1; //TODO: check it in protocol

fn read_struct<T, R: Read>(mut read: R) -> io::Result<T> {
    let num_bytes = ::std::mem::size_of::<T>();
    unsafe {
        let mut s = ::std::mem::uninitialized();
        let buffer = slice::from_raw_parts_mut(&mut s as *mut T as *mut u8, num_bytes);
        match read.read_exact(buffer) {
            Ok(()) => Ok(s),
            Err(e) => {
                ::std::mem::forget(s);
                Err(e)
            }
        }
    }
}

#[derive(Serialize, Deserialize)]
struct PackageData{
    seq_number: u32,
    seq_total: u32,
    i_type: u8,
    id: [u8; 8],
    data:Vec<u8>
}
#[derive(Serialize, Deserialize, PartialEq, Debug)]
struct RequestData{
    seq_number: u32,
    seq_total: u32,
    i_type: u8,
    id: [u8; 8],
    data:u32
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
        let seq_total: u32 = (len / MAX_DATA) as u32;
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

    pub fn get_format_file(filename: String, id_arr: &[u8; 8]) -> Vec<PackageData> {
        let mut contents = fs::read_to_string(filename)
            .expect("Something went wrong reading the file");
        let mut len = contents.len();
        let mut seq_number: u32 = 0;
        let seq_total: u32 = (len / MAX_DATA) as u32;
        let mut file:Vec<PackageData> = vec![];

        while len > MAX_DATA {
            let first = contents.split_off(MAX_DATA);
            len = first.len();
            println!("Text: {}\n", contents);

            let package = PackageData {
                seq_number: seq_number,
                seq_total: seq_total,
                i_type: 0,
                id: *id_arr,
                data: contents.into_bytes()
            };
            file.push(package);
            seq_number+=1;

            contents = first;
            println!("Text: {}\n", contents);
        }

        if contents.len() > 0 {
            let package = PackageData {
                seq_number,
                seq_total,
                i_type: 0,
                id: *id_arr,
                data: contents.into_bytes()
            };
            file.push(package);
            seq_number+=1;
        }
        file
    }
}

fn main() {

    let id:[u8; 8] = [0,0,0,0,0,0,0,1];
    let file_crc = fs::read_to_string("test.txt").expect("Can't read");
    println!("CRC:{}", file_crc.len());
    let mut crc = crc32c::crc32c("12345".as_bytes());
    println!("CRC:{}", crc);

    let file:Vec<PackageData> = PackageData::get_file("test.txt".to_string(), &id);
    let mut rng = rand::thread_rng();
   // let num = rng.gen_range(0, file.len());
    file[num].get_seq_total();

    let socket = UdpSocket::bind("127.0.0.1:8000").expect("Could not bind client socket");
    socket.connect("127.0.0.1:8888").expect("Could not connect to server");
    loop {
        let num = rng.gen_range(0, file.len());
        let mut input = "hello".to_string();
        let mut buffer = [0u8; 40];
        //io::stdin().read_line(&mut input).expect("Failed to read from stdin");
        let bytes = bincode::serialize(&file[num]).unwrap();
        println!("{}", bytes[1]);
        socket.send(&bytes).expect("Failed to write to server");

        socket.recv_from(&mut buffer).expect("Could not read into buffer");
       /* let mut package = PackageData {
            seq_number: buffer[0] as u32,
            seq_total: (buffer[1] as u32) << 32,
            i_type: (buffer[2] as u8) << 64 ,
            id:[buffer[3],buffer[4],buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10]],
            data: vec![0;MAX_DATA]
         };*/
        //let temp = buffer.to_vec();
        //let s = str::from_utf8(temp).expect("sdsdsdsd");

        let decoded: RequestData = bincode::deserialize(&buffer).unwrap();
        print!("{}", decoded.seq_number);
        if

        print!("{}", str::from_utf8(&buffer).expect("Could not write buffer as string"));
    }
}

