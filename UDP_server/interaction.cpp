//
// Created by 79164 on 18.11.2020.
//
#include "interaction.h"
#include <algorithm>

using namespace udp_server;

DataProcessor::DataProcessor() = default;
DataProcessor::~DataProcessor() = default;

DataProcessor::data_from_client DataProcessor::presence_file( DataProcessor::data_from_client package) {
    data_node                       new_data;
    std::array<byte, 8>             package_id = std::to_array<byte>(package.id);
    std::array<byte, MAX_DATA_SIZE> package_data = std::to_array<byte>(package.data); //may be to use better <vector?>


    for (int i = 0; i < rfiles.size(); i++){                //for(auto & rfile : rfiles){}
        if (rfiles[i].file_id == package_id){
            //Add in old list;
            new_data.data = package_data;
            new_data.seq_number = package.seq_number;       //add idempotent in OriginList.sort()
            rfiles[i].quantity_package++;
            rfiles[i].Origin->push_in(new_data, new_data.seq_number); //push data to seq_number cell

            if (rfiles[i].quantity_package == rfiles[i].seq_total)  //check full file n
                return reinterpret_cast<const data_from_client &>(rfiles[i]);
        }
        /*else {
            //Make new vector
            data_properties new_recieved_file;

            new_data.seq_number = package.seq_number;
            new_data.data = package_data;

            new_recieved_file.file_id = package_id;
            new_recieved_file.quantity_package = package.seq_total;

            PackageVector     OriginVector(new_data);
            new_recieved_file.Origin = &OriginVector;
            rfiles.push_back(new_recieved_file);
            return 1;
        }*/
    }
    data_properties new_recieved_file;

    new_data.seq_number = package.seq_number;
    new_data.data = package_data;

    new_recieved_file.file_id = package_id;
    new_recieved_file.quantity_package = 0;
    new_recieved_file.seq_total = package.seq_total;

    PackageVector     OriginVector(new_data, new_data.seq_number);
    new_recieved_file.Origin = &OriginVector;
    rfiles.push_back(new_recieved_file);
    return package;
}




DataProcessor::FileSystem::FileSystem(char *name, char *data)
{
    file.open(name);
    file << data;
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


DataProcessor::PackageVector::PackageVector(data_node package_data, uint32_t seq_number) {
    package.push_back(package_data);
    DataProcessor::PackageVector::push_in( package_data, seq_number);
}

DataProcessor::PackageVector::~PackageVector(){
    package.clear();
}

bool DataProcessor::PackageVector::push_in(data_node package_data, uint32_t seq_number) {
    package[seq_number] = package_data;
    return true;
}


