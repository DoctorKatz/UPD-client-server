#include <iostream>
#include "udp_server.h"
#include "interaction.h"

using namespace std;

int main()
{
    std::string in_buff;
    std::string addr;
    int port;
    uint32_t crc = 0;
    udp_server::DataProcessor::data_from_client ask_to_client;
    std::ifstream in_stream("../test.txt");

    if(!in_stream.good())
    {
        std::cerr << "Error opening file!" << std::endl;
        //More error handling code goes here
    }

    std::vector<std::string> lines;
    std::string file_test;
    while(!in_stream.eof())
    {
        int ch = in_stream.get();
        file_test +=char(ch);
    }
    in_stream.close();
    //1366882825 416359221

    uint32_t len = file_test.size() - 1;
    const char *test1 = "12345";
    crc = udp_server::DataProcessor::FileSystem::crc32c(0, test1, len);
    std::cout << crc;

    char msg[MAX_DATA_SIZE];

    //*std::cout << "Write IP address\n";
    /*getline(cin, in_buff);
    addr = in_buff;

    std::cout << "Write port\n";
    getline(cin, in_buff);
    port = stoi(in_buff);*/

    //udp_server::UdpServer(addr, port);
    udp_server::UdpServer Server(8888);
    cout << Server.get_port();
    cout << "\n";
    cout << Server.get_socket();
    cout << "\n";

    Server.timed_recv(msg, MAX_DATA_SIZE, 10);
   /* for (int i = 0; i < MAX_DATA_SIZE; i++) {
        cout << msg[i];
    }*/
    udp_server::DataProcessor   Client_message;

    ask_to_client = Client_message.presence_file(reinterpret_cast<const udp_server::DataProcessor::data_from_client &>(msg));
    char *temp = (char*)&ask_to_client;
   //memcpy(temp, &ask_to_client, sizeof(udp_server::DataProcessor::data_from_client));
    Server.send(temp, MAX_DATA_SIZE);
    printf("Client : %s\n", msg);
    //Server.send(msg, 100);

}


