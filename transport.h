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
public:
    Transport(char *argv[]);
    void receiveFile();
    bool sendWindowDatagrams();
    bool sendSingleDatagram(Segment segment);
    void receiveData();
    bool setup();

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
    int calculateBytes();
    bool receivedInTime();
    bool receivePacket(int *start, int *datagram_len, int *bytes_received, char *data);
    bool isAcked(int start);
    int fd;
    int bytes_left;

    // window data
private:
    int deleteDatagrams();
    void addDatagrams(int max_new_elements);
    Window w;
    int deleted;
};