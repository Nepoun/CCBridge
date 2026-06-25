#pragma once
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

inline std::string GetLocalIP()
{
    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    getaddrinfo(hostname, nullptr, &hints, &res);

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &((sockaddr_in*)res->ai_addr)->sin_addr, ip, sizeof(ip));
    freeaddrinfo(res);

    return std::string(ip);
}