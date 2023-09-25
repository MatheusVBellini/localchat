#pragma once

#include <iostream>
#include <sys/types.h>
#include <chrono>

 typedef struct message{
    std::string username;
    std::chrono::system_clock::time_point timestamp;
    std::string message_content;

    /* trying to solve warning of memcpy, didnt work yet
    // Copy constructor
    struct message(const struct message& other) {
        username = other.username;
        timestamp = other.timestamp;
        message_content = other.message_content;
    }
    

    // Copy assignment operator
    struct message& operator=(const struct message& other) {
        if (this != &other) {
            username = other.username;
            timestamp = other.timestamp;
            message_content = other.message_content;
        }
        return *this;
    }
    */
}Message;