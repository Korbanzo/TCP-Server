#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <signal.h>
#include <cstdlib>

constexpr int PORT = 54000;
const char* msg = "Hello from the server!";

int sockfd;

void close_on_interrupt_handler(int sig) {
    std::cout << "Server Shut Down Successfully." << std::endl;
    close(sockfd);
    exit(0);
}

int main() {
    signal(SIGINT, close_on_interrupt_handler);

    struct sockaddr_in my_addr, cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "0.0.0.0", &my_addr.sin_addr);

    // Get Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket not found." << std::endl;
        exit(1);
    }

    // Bind Socket to Port
    int binding = bind(sockfd, (const sockaddr*) &my_addr, sizeof(my_addr));

    if (binding < 0) {
        std::cerr << "Bind didn't work!!" << std::endl;
        exit(1);
    }

    // Await a call
    int listening = listen(sockfd, SOMAXCONN);

    if (listening < 0) {
        std::cerr << "Cannot listen." << std::endl;
        exit(1);
    }

    std::cout << "Listening on port " << PORT << std::endl;

    // Continuously accept clients
    while (true) {

        // Accept the call
        int clientSocket = accept(sockfd, (sockaddr*) &cli_addr, &cli_addr_len);

        if (clientSocket < 0) {
            std::cout << "Client cannot connect." << std::endl;
            exit(1);
        }

        // **************** CLIENT IS CONNECTED *************************
        std::cout << "Client connected!" << std::endl;

        ssize_t bytesSent = send(clientSocket, msg, strlen(msg), MSG_NOSIGNAL);

        if (bytesSent < 0) {
            std::cerr << "Could not send message." << std::endl;
        }

        std::cout << "Sent " << bytesSent << " bytes." << std::endl;

        close(clientSocket);
    }

    close(sockfd);

    return 0;
}
