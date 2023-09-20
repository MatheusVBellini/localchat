#include "utils.h"
#include "../include/server.h"

int main(void) { 
    Server server;
    std::cout << server.getConnections() << std::endl;
    server.checkConnections();
    std::cout << server.getConnections() << std::endl;
}
