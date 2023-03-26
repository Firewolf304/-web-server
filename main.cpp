#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "coroutine_source.cpp"
#include <iostream>

#include <time.h>
#include <type_traits>
using namespace::std;
typedef int sock_handle;


sockaddr_in servAddr;           // address data
sock_handle sock;               // socket handle
uint port = 80;                 // port
char * ipAddr = "127.0.0.1";    // ip


namespace log{

    enum class type_log : int{
        LOG = 0,
        DEBUG = 1,
        MESSAGE = 2,
        WARNING = 3,
        ERROR = 4,
        TEST = 5
    };
    char * check_type(type_log type)
    {
        switch (type) {
            case type_log::LOG:
                return "LOG";
                break;
            case type_log::DEBUG:
                return "DEBUG";
                break;
            case type_log::MESSAGE:
                return "MESSAGE";
                break;
            case type_log::WARNING:
                return "WARNING";
                break;
            case type_log::ERROR:
                return "ERROR";
                break;
            case type_log::TEST:
                return "TEST";
                break;

        }
    }
    task log_chat(char * name, type_log type, char * message, sockaddr_in * client_data = NULL)
    /*Logging system, for type: LOG=0|DEBUG=1|MESSAGE=2|WARNING=3|ERROR=4|TEST=5*/
    {
        jthread out;
        co_await switch_to_new_thread(out);
        auto time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        char timer[100];
        strftime(timer, 100, "%a %b %d %T %Y", localtime(&time));
        cout << "[" << timer << "] " << check_type(type) << " ";
        if(client_data != NULL)
        {
            cout << "[" << inet_ntoa(client_data->sin_addr) << ":" << ntohs(client_data->sin_port) << "]";
        }

        cout << " => " << message << "\n";
        out.detach();
        out.join();
    }
}

task check( sockaddr_in client, int client_handle );

int main(int len, char * argv[]) {
    //ipAddr = argv[1];
    //port = ::atoi(argv[2]);
    std::cout << "Hello, World!" << std::endl;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ipAddr);
    servAddr.sin_port = htons(port);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(bind(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("Cant bind socket");
        exit(-1);
    }
    cout << "Binded ip address: " << "http://" <<  inet_ntoa(servAddr.sin_addr) << ":" << ntohs(servAddr.sin_port) << "\n";
    listen(sock, 5);
    while(1)
    {
        sockaddr_in client;
        socklen_t clientSize = sizeof(client);
        int client_handle = accept(sock, (sockaddr *)&client, &clientSize);
        check(client, client_handle);
    }
    return 0;
}



task check( sockaddr_in client, int client_handle )
{
    try {
        //cout << "Coroutine started on thread: " << this_thread::get_id() << '\n';
        jthread out;
        co_await switch_to_new_thread(out);
        //cout << "test coroutine for " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << "\n";
        auto time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        char * timer = ctime( &time );
        char buffer [4096];
        recv(client_handle, &buffer, sizeof(buffer), 0);
        ::log::log_chat("CLIENT", log::type_log::DEBUG, "Connected client", &client);
        //cout << "TIME NOW: " << timer << endl;
        //recv(client_handle, timer, sizeof(timer), MSG_CONFIRM);
        write(client_handle, timer, strlen(timer));
        close(client_handle);
        // end
        out.detach();
        out.join();
    }
    catch (const std::exception e) {}
}

/*
 int a = 1; 0xFFFFFFFF
 int a();   0xFFFFFFF1
    ///////
     ///////
 */

