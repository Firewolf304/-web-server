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
#include <sstream>
#include <vector>

sockaddr_in servAddr;           // address data
sock_handle sock;               // socket handle
uint port = 80;                 // port
char * ipAddr = "127.0.0.1";    // ip
namespace requester{
    typedef struct about{
        vector<string> body;
        int lenght;
    } headers;

    typedef struct req{
        vector<string> data;
        int lenght;
    } request;

    struct request_data{
        bool empty = false;
        request request_info;
        headers headers_info;
    };

    char ** splitter(char * buffer, int size_buffer, int *lenght, char simb)    // утечка
    {
        *lenght = 0;
        char ** data = (char **)malloc(2);
        data[*lenght] = (char*)malloc(2);
        for(int i = 0, k = 0; i < ::strlen(buffer); i++, k++)
        {
            int s = sizeof(data);
            int j = sizeof(data[*lenght]);
            //cout  << i << " " << buffer[i] << " " << sizeof(buffer[i]) << " " << sizeof(data) + sizeof(buffer[i]) + 20 << " " << sizeof(data[*lenght]) + sizeof(buffer[i]) + 20 << endl;
            //data = (char**)realloc(data, sizeof(data) + sizeof(buffer[i]) + 20);
            //data[*lenght] = (char*)::realloc(data[*lenght], sizeof(data[*lenght]) + sizeof(buffer[i]) + 20);
            data = (char**)realloc(data, ((*lenght)+1) * sizeof(data) + 1);
            data[*lenght] = (char*)::realloc(data[*lenght], sizeof(data[*lenght]) * (k+1) + 1);
            if(buffer[i] == simb){
                k = -1;
                (*lenght)++;
                data[*lenght] = (char*)malloc(2);
            }
            else {

                data[*lenght][k] = buffer[i];
                //cout << "res-" << data[*lenght] << endl;
            }
        }
        (*lenght)++;
        return data;
    }
    std::vector<std::string> save_split (const std::string &s, char simp) {
        std::vector<std::string> result;
        std::stringstream ss (s);
        std::string item;

        while (getline (ss, item, simp)) {
            result.push_back (item);
        }

        return result;
    }

    int get_request(char * buffer, request_data * data)
    {
        try {
            //jthread outter;
            //co_await switch_to_new_thread(outter);
            //int split_info_lenght = 0;
            //char ** split_info =
            //char ** line = splitter(split_info[0], sizeof(split_info[0]), &lenght, '\n');
            headers info;
            info.body = save_split(buffer, '\n'); //splitter(buffer, sizeof(buffer), &info.lenght, '\n');

            request line;
            line.data = save_split(info.body[0], ' ');//splitter(info.body[0], sizeof(info.body[0]), &info.lenght, ' ');

            data->headers_info = info;
            data->request_info = line;

            //outter.detach();
            //outter.join();
        }
        catch (const std::exception e) {}
        return 1;
    }



}

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
    task log_chat(char * name, type_log type, basic_string<char> message, sockaddr_in * client_data = NULL)
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

    /*while(true)
    {
        char buffer[4096] = " GET /123 1.1\nHost: 127.0.0.1:81\nConnection: keep-alive\nCache-Control: max-age=0\nsec-ch-ua-mobile: ?0\nsec-ch-ua-platform: ";
        int lenght;
        vector<string> data = requester::save_split(buffer, '\n');
        //char **data = ::requester::splitter( buffer, sizeof(buffer), &lenght, '\n');
        cout << lenght << " " << data[1] << endl;
        //sleep(1);
    }
    exit(0);*/
    /*char buffer[20] = "adaw\nbzsc\nc\nj";
    int lenght;
    char **data = ::request::split( buffer, sizeof(buffer), &lenght, '\n');
    cout << lenght << data[lenght-1];*/
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

    //cout << "Coroutine started on thread: " << this_thread::get_id() << '\n';
    jthread out;
    co_await switch_to_new_thread(out);
    auto time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    char * timer = ctime( &time );
    char buffer [4096];
    recv(client_handle, &buffer, sizeof(buffer), 0);
    ::log::log_chat("CLIENT", log::type_log::DEBUG, "Connected client", &client);
    ::requester::request_data info;
    ::requester::get_request(buffer, &info);
    ::log::log_chat("CLIENT", log::type_log::DEBUG, (string)"Request data: Method - " + (string)(info.request_info.data[0]) +
                                                    ", path - " + (string)(info.request_info.data[1]) + ", protocol - " + (string)(info.request_info.data[2]),&client);
    //::log::log_chat("CLIENT", log::type_log::MESSAGE, buffer, &client);
    //cout << "TIME NOW: " << timer << endl;
    //recv(client_handle, timer, sizeof(timer), MSG_CONFIRM);
    write(client_handle, timer, strlen(timer));
    close(client_handle);
    // end
    out.detach();
    out.join();

}

/*
        стратегия:
        GET /123 HTTP/1.1 -> split(" ") => check => [0] - метод [1] - path [2] - protocol
        header: *body*


 */

