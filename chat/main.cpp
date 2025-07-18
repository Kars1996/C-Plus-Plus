#include "ChatClient.h"
#include "ChatServer.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [server|client] [options]\n";
    std::cout << "  server <port>           - Start chat server on specified port\n";
    std::cout << "  client <host> <port>    - Connect to chat server\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "server") {
        if (argc != 3) {
            printUsage(argv[0]);
            return 1;
        }

        int port = std::stoi(argv[2]);
        ChatServer server(port);
        
        std::cout << "Starting chat server on port " << port << "...\n";
        server.start();
        
        std::string input;
        std::cout << "Press Enter to stop server...\n";
        std::getline(std::cin, input);
        
        server.stop();
    }
    else if (mode == "client") {
        if (argc != 4) {
            printUsage(argv[0]);
            return 1;
        }

        std::string host = argv[2];
        int port = std::stoi(argv[3]);
        
        ChatClient client(host, port);
        
        std::cout << "Connecting to chat server at " << host << ":" << port << "...\n";
        
        if (client.connect()) {
            std::cout << "Connected! Type messages and press Enter to send. Type 'quit' to exit.\n";
            client.run();
        } else {
            std::cout << "Failed to connect to server.\n";
            return 1;
        }
    }
    else {
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
