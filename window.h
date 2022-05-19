#pragma once

#include <deque>
#include "utils.h"
#include <string.h>

#define MAX_WINDOW_SIZE 200

using namespace std;

class Window
{
public:
    Window(int file_size);
    int getFirstNotAck();
    void setSegmentAck(int start);
    int findSegment(int start);
    int getStart();
    void incrementStart(int offset);
    int getBytesLeft();
    void decrementBytesLeft(int bytes);
    int calculateBytes(int bytes_left);
    void setDeleted(int deleted);

private:
    int file_size;
    int window_size;
    int start;
    int bytes_left;
    int deleted;

public:
    deque<Segment> window;
};