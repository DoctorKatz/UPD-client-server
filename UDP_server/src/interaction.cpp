//
// Created by 79164 on 18.11.2020.
//
#include "../inc/interaction.h"
#include <string.h>
#include <algorithm>
#include <iostream>

using namespace udp_server;

DataProcessor::DataProcessor() {
    m_ready_file = false;

};
DataProcessor::~DataProcessor() = default;

DataProcessor::data_from_client DataProcessor::presence_file(DataProcessor::data_from_client package) { //TODO: rename file
    std::array<byte, ID_SIZE>       package_id   = std::to_array<byte>(package.id);
    std::array<byte, MAX_DATA_SIZE> package_data = std::to_array<byte>(package.data); //may be to use better <vector?>
    uint32_t crc = 0;   //can use to check incomplete file
    m_ready_file = false;


    for (int i = 0; i < rfiles.size(); i++){                //for(auto & rfile : rfiles){}
        if (rfiles[i].file_id == package_id){
            if (rfiles[i].Origin->push_in(package_data, package.seq_number)){
                rfiles[i].quantity_package++;
            } //push data to seq_number cell
            crc = DataProcessor::FileSystem::crc32c(0,
                                                    reinterpret_cast<const char *>(package_data.data()),
                                                    package_data.size());

            std::cout << crc << std::endl;
            if (rfiles[i].quantity_package == rfiles[i].seq_total && rfiles[i].file_id == package_id) {  //Is file full? May be put in method?
                m_ready_file = true;                                  //may be to use struct for finish parameters (id, crc)
                File.name = rfiles[i].file_id;
                File.data = DataProcessor::concantenate_packages(rfiles[i], File.data);
                DataProcessor::delete_file(i);
                crc = DataProcessor::FileSystem::crc32c(0,
                                                        reinterpret_cast<const char *>(File.data.data()),
                                                        File.data.size());
            }
            package = DataProcessor::request_package(package,  rfiles[i].quantity_package, crc);
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
        m_crc = crc;
        receive_package.data[0] = crc >> 24;
        receive_package.data[1] = crc >> 16;
        receive_package.data[2] = crc >> 8;
        receive_package.data[3] = crc;
        memset(receive_package.data + 4 * sizeof(byte), 0, sizeof(byte)*MAX_DATA_SIZE - 4);
    }
    else
        memset(receive_package.data, 0, sizeof(byte)*MAX_DATA_SIZE);
    return receive_package;
}

std::vector<byte> DataProcessor::concantenate_packages(DataProcessor::data_properties file_arr, std::vector<byte> concantenated_data) {
    for (int i = 0; i < file_arr.seq_total; i++){
        //File.data.resize(file_arr.seq_total * MAX_DATA_SIZE);
        std::copy(file_arr.Origin->package[i].begin(), file_arr.Origin->package[i].end(), back_inserter(concantenated_data));
    }
    return concantenated_data;
}

bool DataProcessor::write_file(){
    FileSystem(File.name, File.data);
    return true;
}

DataProcessor::FileSystem::FileSystem(std::array<byte, 8> file_name, std::vector<byte> data)
{
    std::string str_file_name (std::begin(file_name), std::end(file_name));

    std::ofstream output_file("str_file_name");
    std::string chars = "\\";
    for (char c: chars) {
        str_file_name.erase(std::remove(str_file_name.begin(), str_file_name.end(), c), str_file_name.end());
    }
    std::ostream_iterator<byte> output_iterator(output_file, "");
    std::copy(data.begin(), data.end(), output_iterator);
    output_file.close();
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
    bool flag;

    if (seq_number > package.size()) //not used??
        package.resize(seq_number);

    if (package[seq_number] != package_data)
        flag = true;
    else
        flag = false;

    package[seq_number] = package_data;
    return flag;
}


