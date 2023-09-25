#pragma once

#include <iostream>
#include <sys/types.h>
#include <chrono>

#define MSG_SIZE (10 + 1 + 200 + 1 + sizeof(std::chrono::system_clock::time_point))

 typedef struct message{
    std::string username;
    std::chrono::system_clock::time_point timestamp;
    std::string message_content;
}Message;