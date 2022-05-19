#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <chrono>
#include <fcntl.h>

#include "window.h"

#define MAX_DATA_LEN 1000

using namespace std;

class Transport
{
    // main build
public:
    Transport(char *argv[]);
    bool setup();
    void receiveFile();

    // data handling
public:
    bool sendWindow();
    bool sendSingleDatagram(Segment segment);
    void receiveData();

    // socket data
private:
    bool socketSetup();
    int sockfd;
    fd_set descriptors;

    // server address data
private:
    bool serverAddressSetup();
    struct sockaddr_in server_address;

    // input data
private:
    char *ip_addr;
    int port;
    char *file_name;
    int file_size;

    // file receiving
private:
    bool receivedInTime();
    bool receivePacket(int *start);
    bool isAcked(int start);
    int fd;
    int bytes_left;

    // window data
private:
    void addDatagrams(int max_new_elements);
    Window w;
    int deleted;
};