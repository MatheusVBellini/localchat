#pragma once

#include <iostream>
#include <sys/types.h>
#include <chrono>

 typedef struct message{
    std::string username;
    std::chrono::system_clock::time_point timestamp;
    std::string message_content;
}Message;