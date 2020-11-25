#include <iostream>
#include "udp_server.h"
#include "interaction.h"

using namespace std;

int main()
{
    std::string in_buff;
    std::string addr;
    int port;
    udp_server::DataProcessor::data_from_client ask_to_client;

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
    printf("Client : %s\n", msg);
    udp_server::DataProcessor   Client_message;
    ask_to_client = Client_message.presence_file(reinterpret_cast<const udp_server::DataProcessor::data_from_client &>(msg));
    Server.send((char *)&ask_to_client.type, 1024);
    printf("Client : %s\n", msg);
    //Server.send(msg, 100);

}


