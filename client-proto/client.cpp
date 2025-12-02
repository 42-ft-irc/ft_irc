#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <poll.h>
#include <fcntl.h>

int main()
{
    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo("localhost", "6667", &hints, &res) != 0) {
        std::cerr << "getaddrinfo failed" << std::endl;
        return 1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        std::cerr << "socket failed: " << strerror(errno) << std::endl;
        freeaddrinfo(res);
        return 1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << "connect failed: " << strerror(errno) << std::endl;
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }

    freeaddrinfo(res);
    std::cout << "Connected to server! Type messages to send." << std::endl;

    // Set up poll to monitor both stdin and the socket
    struct pollfd fds[2];
    
    // Monitor stdin for user input
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    
    // Monitor socket for incoming messages
    fds[1].fd = sockfd;
    fds[1].events = POLLIN;

    char buffer[1024];
    bool running = true;

    while (running) {
        int poll_result = poll(fds, 2, -1);
        if (poll_result < 0) {
            std::cerr << "poll failed: " << strerror(errno) << std::endl;
            break;
        }

        // Check for user input from stdin
        if (fds[0].revents & POLLIN) {
            ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                // Send the message to the server
                if (send(sockfd, buffer, bytes_read, 0) < 0) {
                    std::cerr << "send failed: " << strerror(errno) << std::endl;
                }
            } else if (bytes_read == 0) {
                // EOF on stdin (Ctrl+D)
                running = false;
            }
        }

        // Check for incoming messages from the server
        if (fds[1].revents & POLLIN) {
            ssize_t bytes_recv = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_recv > 0) {
                buffer[bytes_recv] = '\0';
                std::cout << "Server: " << buffer << std::endl;
            } else if (bytes_recv == 0) {
                std::cout << "Server closed connection" << std::endl;
                running = false;
            } else {
                std::cerr << "recv failed: " << strerror(errno) << std::endl;
                running = false;
            }
        }

        // Check for socket errors
        if (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            std::cerr << "Socket error" << std::endl;
            running = false;
        }
    }

    close(sockfd);
    return 0;
}