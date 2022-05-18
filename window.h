#pragma once

#include <deque>
#include "utils.h"
#include <string.h>

#define MAX_WINDOW_SIZE 100

using namespace std;

class Window
{
public:
    Window(int file_size);
    char* getSegmentMessage(int idx);
    int getSegmentBytesToReceive(int idx);
    int getSegmentStart(int idx);
    Segment getFirstNotAck();
    void setSegmentAck(int start);

private:
    int calculateBytes(int bytes_left);
    int file_size;
    int window_size;
    deque<Segment> window;
};