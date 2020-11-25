//
// Created by 79164 on 18.11.2020.
//

#ifndef UDP_SERVER_INTERACTION_H
#define UDP_SERVER_INTERACTION_H

#include <stddef.h>
#include <stdint.h>
#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>

#define POLY 0x82f63b78
#define ACK 0
#define PUT 1
#define MAX_DATA_SIZE   40//1472

typedef unsigned char byte;

namespace udp_server {

    class DataProcessor {
    private:
        struct data_node{
            uint32_t                        seq_number;
            std::array<byte, MAX_DATA_SIZE> data;
        };

    public:
        struct data_from_client {
            uint32_t    seq_number;
            uint32_t    seq_total;
            uint8_t     type;
            byte        id[8];
            byte        data[MAX_DATA_SIZE];
        };
    public:
        struct data_from_client package{};
        struct data_node        data{};

        bool    check_data_id();
        bool    check_data_type(data_from_client package);




        class PackageVector{
        private:
            std::vector<data_node> package;

        public:
            PackageVector(data_node package_data, uint32_t seq_number);
            ~PackageVector();

            bool push_in(data_node package_data, uint32_t seq_number);
            data_node get_package();

        };

        class PackageList {
        private:



        public:
            std::list<data_node> node;

            explicit PackageList(data_node package_data);
            ~PackageList();

            char *push_in_list(data_node package_data);
            char *pop_from_list(data_node package_data);
            char *sort_package_list();
            void parce_packege(data_node &package_data);
            static bool my_compare(DataProcessor::data_from_client, const data_node &node2);
        };

    private:
        struct data_properties{
            std::array<byte, 8>             file_id;
            uint32_t                        quantity_package;
            uint32_t                        seq_total;
            PackageVector*                  Origin;
        };
        std::vector<data_properties>        rfiles;

    public:
        DataProcessor();
        ~DataProcessor();
        DataProcessor::data_from_client     presence_file(data_from_client package);

        class FileSystem {
        public:
            FileSystem(char *name, char *data);
            ~FileSystem();

            int save_package(byte *data);
            uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len);
            bool check_crc32c(uint32_t crc_input_file, uint32_t crc_output_file);


        private:
            std::ofstream file;
        };
    };
}
#endif //UDP_SERVER_INTERACTION_H
