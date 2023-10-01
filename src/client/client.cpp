#include "../../include/client.hpp"

Client::Client(std::string username, std::string server_ip, int server_port) {
    this->setUsername(username);
    this->is_connected = false;
    

    // Socket configuration
    this->client_socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
    if (this->client_socket  < 0) {
        error("Failed to create socket from client");
    }

    // Set socket to non-blocking mode
    int flags = fcntl(this->client_socket, F_GETFL, 0);
    if (flags == -1) {
        error("client fcntl get failed");
    }
    int non_block_result = fcntl(this->client_socket, F_SETFL, flags | O_NONBLOCK);
    if (non_block_result == -1) {
        error("client fcntl set failed");
    }

    this->server_addr.sin_family = AF_INET;
    // this->server_addr.sin_addr.s_addr = INADDR_ANY; // is this really necessary?
    this->server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip.c_str(), &this->server_addr.sin_addr); // Convert IPv4 and IPv6 addresses from text to binary form

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            error("could not connect client to server");
            return;
    }else{
        debug("client connected")
        this->is_connected = true;
    }


}

int Client::getClientSocket(){
    return this->client_socket;
}

void Client::setUsername(std::string usr){
    // this makes the usr always the same size (10 char  '\0')
    if(usr.size() > 10){
        usr = usr.substr(0,10);
    }
    else if(usr.size() < 10){
        usr = usr.append('\0' , 10 - usr.size());
    }
    this->username = usr;
}

