#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include "util.hpp"

int main()
{
    struct addrinfo hints, *res;
    int sockfd;

    // first, load up address structs with getaddrinfo():

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo("localhost", "6667", &hints, &res) != 0) {
        std::cerr << "getaddrinfo failed" << std::endl;
        return 1;
    }

    // print the results list:
    // printAddrInfo(res);

    // make a socket:

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        std::cerr << "socket failed: " << strerror(errno) << std::endl;
        freeaddrinfo(res);
        return 1;
    }

    // connect!

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "connect failed: " << strerror(errno) << std::endl;
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    // send a message to the server
    const char *msg = "Hello, World!";
    int bytes_sent = send(sockfd, msg, strlen(msg), 0);
    if (bytes_sent == -1) {
        std::cerr << "send failed: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Sent " << bytes_sent << " bytes: " << msg << std::endl;
    }

    // cleanup
    freeaddrinfo(res);
    close(sockfd);

    return 0;
}