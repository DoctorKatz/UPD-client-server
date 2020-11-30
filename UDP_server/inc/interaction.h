//
// Created by 79164 on 18.11.2020.
//

#ifndef UDP_SERVER_INTERACTION_H
#define UDP_SERVER_INTERACTION_H

#include <stddef.h>
#include <stdint.h>
#include <iterator>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>

#define POLY 0x82f63b78
#define ACK 0
#define PUT 1

#define MAX_DATA_SIZE   23//1472
#define PACKAGE_SIZE    (MAX_DATA_SIZE + 17)

#define ID_SIZE         8

typedef unsigned char byte;

namespace udp_server {

    class DataProcessor {
    private:
        struct data_node{
            uint32_t                        seq_number;
            std::array<byte, MAX_DATA_SIZE> data;
        };

    public:

#pragma pack(1)
        struct data_from_client {
            uint32_t    seq_number;
            uint32_t    seq_total;
            uint8_t     type;
            byte        id[ID_SIZE];
            byte        data[MAX_DATA_SIZE];
        };
#pragma pack(pop)
        
        struct data_to_file_system{
            std::vector<byte>               data;
            std::array<byte, ID_SIZE>       name;
        };

        data_to_file_system File;

        //struct data_from_client package{};
        int                     m_crc;
        int                     m_num_full_file;
        bool                    m_ready_file;
        

        bool                            check_data_id();
        bool                            check_data_type(data_from_client package);
        void                            delete_file(int num_file);
        int                             check_full_file(int number_file);
        DataProcessor::data_from_client request_package(DataProcessor::data_from_client package,
                                                        uint32_t quantity_package, uint32_t crc);





        class PackageVector{
        private:
            //std::vector<data_node> package;

        public:
            //PackageVector(data_node package_data, uint32_t seq_number);
            PackageVector(uint32_t seq_total,  uint32_t seq_number, std::array<byte, MAX_DATA_SIZE> package_data);
            ~PackageVector();

            std::vector<std::array<byte, MAX_DATA_SIZE>> package;
            bool push_in(std::array<byte, MAX_DATA_SIZE> package_data, uint32_t seq_number);
            std::array<byte, MAX_DATA_SIZE> get_package(uint32_t seq_number);

        };

        /*class PackageList {

        public:
            std::list<data_node> node;

            explicit PackageList(data_node package_data);
            ~PackageList();

            char *push_in_list(data_node package_data);
            char *pop_from_list(data_node package_data);
            char *sort_package_list();
            void parce_packege(data_node &package_data);
            static bool my_compare(DataProcessor::data_from_client, const data_node &node2);
        };*/

    private:
        struct data_properties{
            uint32_t                        quantity_package;
            uint32_t                        seq_total;
            std::array<byte, 8>             file_id;
            PackageVector                   *Origin;
        };
        std::vector<data_properties>        rfiles;
        std::vector<byte>                   concantenate_packages(data_properties, std::vector<byte> concantenated_data);

    public:
        DataProcessor();
        ~DataProcessor();
        DataProcessor::data_from_client     presence_file(data_from_client package);
        bool readiness_file();
        bool write_file();

        class FileSystem {
        public:
            FileSystem(std::array<byte, 8> file_name, std::vector<byte> data);
            ~FileSystem();

            int save_package(byte *data);
            static uint32_t crc32c(uint32_t crc, const char *buf, size_t len);
            bool check_crc32c(uint32_t crc_input_file, uint32_t crc_output_file);


        private:
            std::ofstream file;
        };
    };
}
#endif //UDP_SERVER_INTERACTION_H
