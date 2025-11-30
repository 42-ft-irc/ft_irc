#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "util.hpp"

int main()
{
    struct addrinfo hints, *res;
    int sockfd;

    // first, load up address structs with getaddrinfo():

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo("localhost", "6667", &hints, &res);

    // print the results list:
    // printAddrInfo(res);

    // make a socket:

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // connect!

    connect(sockfd, res->ai_addr, res->ai_addrlen);

    std::cout << "Connected to server!" << std::endl;

    // send a message to the server
    const char *msg = "Hello, World!";
    int bytes_sent = send(sockfd, msg, strlen(msg), 0);
    std::cout << "Sent " << bytes_sent << " bytes: " << msg;

    // cleanup
    freeaddrinfo(res);
    close(sockfd);

    return 0;
}