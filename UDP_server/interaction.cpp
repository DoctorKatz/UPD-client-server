//
// Created by 79164 on 18.11.2020.
//
#include "interaction.h"
#include <string.h>
#include <algorithm>

using namespace udp_server;

DataProcessor::DataProcessor() {
    m_ready_file = false;
};
DataProcessor::~DataProcessor() = default;

DataProcessor::data_from_client DataProcessor::presence_file(DataProcessor::data_from_client package) { //TODO: rename file
    //data_node                       new_data;
    std::array<byte, ID_SIZE>       package_id   = std::to_array<byte>(package.id);
    std::array<byte, MAX_DATA_SIZE> package_data = std::to_array<byte>(package.data); //may be to use better <vector?>
    uint32_t crc = 0;   //can use to check incomplete file
    m_ready_file = false;


    for (int i = 0; i < rfiles.size(); i++){                //for(auto & rfile : rfiles){}
        if (rfiles[i].file_id == package_id){
            //Add in old list;
            //new_data.data = package_data;
            //new_data.seq_number = package.seq_number;
            //rfiles[i].quantity_package++;
            if (rfiles[i].Origin->push_in(package_data, package.seq_number)){
                rfiles[i].quantity_package++;
            } //push data to seq_number cell

            if (rfiles[i].quantity_package == rfiles[i].seq_total) {  //Is file full? May be put in method?
                m_ready_file = true;                                  //may be to use struct for finish parameters (id, crc)
                m_num_full_file = i;
                File.name = rfiles[i].file_id;
                DataProcessor::concantenate_packages(rfiles[i], File.data);
                DataProcessor::delete_file(i);
                crc = DataProcessor::FileSystem::crc32c(0, reinterpret_cast<const char *>(File.data.data()), File.data.size());
            }
            package = DataProcessor::request_package(package, package.seq_number, crc);
            return package;
        }
    }


    data_properties new_recieved_file;
    new_recieved_file.quantity_package = 1;
    new_recieved_file.seq_total        = package.seq_total;
    static PackageVector               OriginVector(package.seq_total, package.seq_number, package_data);

    new_recieved_file.file_id = package_id;

    new_recieved_file.Origin = &OriginVector;
    rfiles.push_back(new_recieved_file);

    package = DataProcessor::request_package(package, package.seq_number, crc);
    return package;
}

void DataProcessor::delete_file(int num_file){
    rfiles.erase(rfiles.begin() + num_file);
}

bool DataProcessor::readiness_file(){
    return m_ready_file;
}

DataProcessor::data_from_client DataProcessor::request_package(DataProcessor::data_from_client receive_package,
                                                               uint32_t quantity_package, uint32_t crc) {
    receive_package.type = ACK;
    receive_package.seq_total = quantity_package;
    if (crc != 0) {
        //Big endian
        receive_package.data[0] = crc >> 24;
        receive_package.data[1] = crc >> 16;
        receive_package.data[2] = crc >> 8;
        receive_package.data[3] = crc;
    }
    else
        memset(receive_package.data, 0, sizeof(byte)*MAX_DATA_SIZE);
    return receive_package;
}

std::vector<byte> DataProcessor::concantenate_packages(DataProcessor::data_properties file_arr, std::vector<byte> concantenated_data) {
    for (int i = 0; i < file_arr.seq_total; i++){
        std::copy(file_arr.Origin->package[i].begin(), file_arr.Origin->package[i].begin(), concantenated_data.begin());
    }
    return concantenated_data;
}

bool DataProcessor::write_file(){
    FileSystem(File.name, File.data);
    return true;
}

DataProcessor::FileSystem::FileSystem(std::array<byte, 8> file_name, std::vector<byte> data)
{
    file.open((char*)file_name.data());
    file << data.data();
}

DataProcessor::FileSystem::~FileSystem() {
    file.close();
}

uint32_t DataProcessor::FileSystem::crc32c(uint32_t crc, const char *buf, size_t len)
{
    int k;

    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
    }
    return ~crc;
}

/*DataProcessor::PackageList::PackageList(data_node package_data){
    node.push_front(package_data);
}

DataProcessor::PackageList::~PackageList(){
    node.clear();
}

char* DataProcessor::PackageList::push_in_list(data_node package_data){
    node.push_front(package_data);
}

char *DataProcessor::PackageList::sort_package_list(){
    node.sort(&PackageList::my_compare);
}

void DataProcessor::PackageList::parce_packege(data_node& package_data){
    package_data = node.front();
}

bool DataProcessor::PackageList::my_compare(DataProcessor::data_from_client data, const data_node& node2)
{
    return data.seq_number < node2.seq_number;//ask gosha!!
}*/


DataProcessor::PackageVector::PackageVector(uint32_t seq_total,  uint32_t seq_number, std::array<byte, MAX_DATA_SIZE> package_data) {

    package.resize(seq_total);
    DataProcessor::PackageVector::push_in( package_data, seq_number);
}

DataProcessor::PackageVector::~PackageVector(){
    package.clear();
}

std::array<byte, MAX_DATA_SIZE> DataProcessor::PackageVector::get_package(uint32_t seq_number) {
    return package[seq_number];
}

bool DataProcessor::PackageVector::push_in(std::array<byte, MAX_DATA_SIZE> package_data, uint32_t seq_number) {
    if (seq_number > package.size()) //not used??
        package.resize(seq_number);
    if (package[seq_number - 1] == package_data)
        return false;
    package[seq_number - 1] = package_data;
    return true;
}


