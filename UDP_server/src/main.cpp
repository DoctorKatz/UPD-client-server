#include <iostream>
#include "../inc/udp_server.h"
#include "../inc/interaction.h"
#include "../inc/lib.h"

using namespace std;

int main()
{
    udp_server::DataProcessor::data_from_client request_client;

    char msg[PACKAGE_SIZE];

    udp_server::UdpServer Server(8888);
    cout << Server.get_port() << endl;
    cout << Server.get_socket() << endl;

    udp_server::DataProcessor ClientMessages;
    while(io::kbhit() != 1) { //push key for exit
        memset(msg, 0, PACKAGE_SIZE);
        Server.timed_recv(msg, PACKAGE_SIZE, 1000);
        request_client = ClientMessages.presence_file(
                reinterpret_cast<const udp_server::DataProcessor::data_from_client &>(msg));

        Server.send((char *)&request_client, PACKAGE_SIZE);

        if (ClientMessages.m_ready_file){
            ClientMessages.write_file();
            cout << ClientMessages.m_crc << endl;
            cout << "File is write" << endl;
        }
    }
}


