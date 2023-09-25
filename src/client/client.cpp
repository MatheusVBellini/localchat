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
            error("coudl not connect client to server");
            return;
    }else{
        debug("client connected")
        this->is_connected = true;
    }


}

void Client::setUsername(std::string usr){
    if(usr.size() > 10){
        std::string new_usr;
        new_usr = usr.substr(0,10);
        this->username = new_usr;
    }else{
        this->username = usr;
    }
}

Message Client::buildMessage(std::string content){
    if(content.size() > 200){
        content = content.substr(200,0);
    }

    Message msg;
    msg.message_content = content;
    msg.username = this->username;
    msg.timestamp = std::chrono::system_clock::now();

    return msg;
}

bool Client::sendMessage(std::string message){
    Message msg_to_send = this->buildMessage(message);
    
    char serialized_msg[sizeof(Message)];
    std::memcpy(serialized_msg, &msg_to_send, sizeof(Message));

    send(client_socket, serialized_msg,sizeof(serialized_msg), 0);
}