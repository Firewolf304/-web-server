//
// Created by root on 25.04.23.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#
#include <time.h>
#include <type_traits>
using namespace::std;

typedef int sock_handle;
#include <sstream>
#include <vector>
#include <iostream>
#include <filesystem>
#include <zlib.h>
#include <fstream>
#include <functional>

#include "nlohmann/json.hpp"
using namespace::nlohmann;
namespace firewolf::responser{
    typedef struct body{
        vector<vector<string>> headers;
    };
    class response{
    public:
        string header_body = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n";
        string body_text;
        string make_request(){
            return header_body + "Content-Length: " + to_string(body_text.length()) + "\r\n\r\n" + body_text;
        }
    };

}
using namespace::firewolf;
namespace firewolf::requester{

    typedef struct req{
        string path = "";
        string protocol = "";
        string method = "";
        string media_path = "";
    } request_head;

    struct request_data{
        bool empty = true;
        request_head request_info;
        json headers;
        string body;
    };
    /*typedef struct about{
        vector<string> body;
        int lenght;
    } headers;

    typedef struct req{
        vector<string> data;
        int lenght;
    } request;

    struct request_data{
        bool empty = true;
        request request_info;
        headers headers_info;
    };*/


    /*char ** splitter(char * buffer, int size_buffer, int *lenght, char simb)    // утечка
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
    }*/

    inline vector<std::string> save_split (const std::string &s, char simp) {
        std::vector<std::string> result;
        std::stringstream ss (s);
        std::string item;
        while (getline (ss, item, simp)) {
            result.push_back (item);
        }
        return result;
    }
    inline string get_line_ready(string text, string simb) {
        return text.substr(text.find(simb)+simb.length(), text.length()); }
    inline string replacer(string str, const string from, const string to) {
        size_t start_pos = str.find(from);
        return str.replace(start_pos, from.length(), to);
    }
    inline json headers_to_json(string text) {
        string hed = text.substr(text.find("\r\n")+2, text.find("\r\n\r\n") - text.find("\r\n")-2); // input only headers
        json js;
        //cout << hed << endl;
        int pos = 0;
        //cout << "lenght = " << hed.length() << endl;
        while(true)
        {
            string result;
            result = hed.substr(pos, hed.find('\r\n', pos) - pos);
            result = replacer(result, "\r", "");
            result = replacer(result, "\n", "");
            cout << "new_pose = " << pos << " " <<  pos << " " << result << endl;
            string save = result.substr(0, result.find(": ") );
            int l = result.find(": ");
            js.emplace(result.substr(0, l ), result.substr(l + 2, result.length() - l));
            cout << result.substr(0, l ) << " => " << js[result.substr(0, l )].get<string>() << endl;
            pos = hed.find('\r\n', pos) + 1;
            if(hed.find('\r\n', pos-1) == string::npos) break;
        }
        return js;
    }
    inline int get_request(char * buffer, request_data * data) {
        try {
            vector<string> body = save_split(buffer, '\n');
            /*string req; stringstream s_req(req);
            getline (s_req, req, '\n');
            cout << "123" << req << endl;*/
            vector<string> info = save_split(body[0], ' ');
            //stringstream().swap(s_req);
            //req.clear();
            data->request_info.method = info[0];
            data->request_info.path = info[1];
            data->request_info.protocol = info[2];
            vector<string> deflare_path = save_split(data->request_info.path, '?');
            if(deflare_path.size() > 1){
                data->request_info.path = deflare_path[0];
                data->request_info.media_path = deflare_path[1];
            }
            vector<string>().swap(deflare_path); // free up memory

            data->body = get_line_ready((string)buffer, "\r\n\r\n");
            data->headers = headers_to_json((string)buffer);

            //cout << data->headers << endl;
            /*headers info;
            info.body = save_split(buffer, '\n'); //splitter(buffer, sizeof(buffer), &info.lenght, '\n');

            request line;
            line.data = save_split(info.body[0], ' ');//splitter(info.body[0], sizeof(info.body[0]), &info.lenght, ' ');

            data->headers_info = info;
            data->request_info = line;
            data->empty = false;*/
        }
        catch (const std::exception e) {}
        return 1;
    }

    inline json media_to_json(string media) {
        json convert;
        vector<string> med = save_split(media, '&');
        for(string a : med)
        {
            vector<string> d = save_split(a, '=');
            //convert[d[0]] = d[1];
            convert.emplace(d[0], d[1]);
        }
        return convert;
    }
    inline json cookie_to_json(string cookie) {
        cookie.replace(cookie.begin(), cookie.end(), ";", "");
        json convert;
        vector<string> med = save_split(cookie, ' ');
        for(int i = 1; i < med.size(); i++)
        {
            vector<string> d = save_split(med[i], '=');
            convert[d[0]] = d[1];
        }
        return convert;
    }
    /*[[maybe_unused]] Generator<json> async_get_json(string media)
    {
        json convert;
        try{
        vector<string> med = save_split(media, '&');
        for(string a : med)
        {
            vector<string> d = save_split(a, '=');
            convert[d[0]] = d[1];
            co_yield convert;
        }}
        catch(exception a){}
    }*/

    class check_path{
    public:
        string path = "";
        bool is_dir()
        {
            const filesystem::path fpath(path); // Constructing the path from a string is possible.
            std::error_code ec;
            return is_directory(fpath);
        }
    };


}
class api{
public:
    unordered_map<string, function<void(string*, responser::response*, requester::request_data)>> routes;
    api()
    {
        routes["/api"] = [](string * s, responser::response *rep, requester::request_data req){
            if(req.request_info.method == "POST")
            {
                rep->header_body += "Set-Cookie: token=";
            }
        };
        routes["/rand"] = [](string * s, responser::response* rep, requester::request_data req){
            json media;
            if(req.request_info.method == "POST") {
                /*for(auto & j : media.items())
                {
                    switch()
                }*/

                unordered_map<string, function<void(json * media, string body)>> check{
                        {
                                "application/json", [](json * media, string str) {
                            (*media) = json::parse(str.c_str()); }},
                        {
                                "application/x-www-form-urlencoded",[](json * media, string str) {
                            *media = requester::media_to_json(str); }
                        }
                };
                try{
                    check[req.headers["Content-Type"].get<string>()](&media, req.body);
                    //media = json::parse(req.body);
                }
                catch(exception e){
                    media = requester::media_to_json(req.request_info.media_path);
                }

            }
            else if(req.request_info.method == "GET" || req.request_info.method == "PUT"){
                media = requester::media_to_json(req.request_info.media_path);
            }
            try {
                *s = (media.size() > 0) ? to_string(1 + rand() % stoi(media["to"].get<string>())) : to_string(rand());
            }
            catch (exception e){
                *s = to_string(rand());
            }
        };
    }

    bool is_api(string path)
    {
        if(routes.find(path) != routes.end())
        {
            return true;
        }
        else{
            return false;
        }
    }

};

