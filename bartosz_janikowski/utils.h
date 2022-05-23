// Bartosz Janikowski, 315489

#pragma once

#include <arpa/inet.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string.h>
#include <unistd.h>

#define MAX_DATA_MESSAGE_LEN 1040 // 1000 (data) + 40 (DATA ... ...)
#define MAX_DATA_LEN 1000

#define MAX_SEND_MESSAGE_LEN 40

using namespace std;

struct Segment
{
    bool is_ack;
    int bytes;
    int start;
    char send[MAX_SEND_MESSAGE_LEN];
    char data[MAX_DATA_MESSAGE_LEN];
};

bool validateInput(int argc, char *argv[]);

int calculateBytes(int bytes_left, int file_size);
