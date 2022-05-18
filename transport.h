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

#define MAX_DATA_LEN 1000

using namespace std;

class Transport
{
public:
    Transport(char *argv[]);
    void receiveFile();
    bool sendDatagram(int position);
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
    bool receivePacket(int *bytes_received);
    ofstream file;
    int bytes_left;


};