#include "util.hpp"
#include <iostream>
#include <arpa/inet.h>

void printAddrInfo(struct addrinfo *res)
{
    std::cout << "Address info results:" << std::endl;
    for (struct addrinfo *p = res; p != NULL; p = p->ai_next) {
        void *addr;
        char ipstr[INET6_ADDRSTRLEN];

        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            std::cout << "  IPv4: ";
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            std::cout << "  IPv6: ";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        std::cout << ipstr << std::endl;
    }
    std::cout << std::endl;
}