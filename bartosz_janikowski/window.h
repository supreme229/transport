// Bartosz Janikowski, 315489

#pragma once

#include <deque>
#include <string.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cmath>

#include "utils.h"

#define MAX_WINDOW_SIZE 200

using namespace std;

class Window
{
    // construction
public:
    Window(int file_size);

    // getter
public:
    int getCurrentWindowSize();

    // segments operations
public:
    Segment getSpecificSegment(int idx);
    Segment buildSegment();
    void addSegments(int max_new_elements);
    int deleteSegments(int fd, int *bytes_left);
    int findSegment(int start);
    void fillSegment(int start, char *data, int off);
    bool checkSegmentAck(int idx);
    void switchSegmentAck(int idx);

private:
    int file_size;
    int window_size;

private:
    deque<Segment> window;

private:
    int start;
    int bytes_left;
    int deleted;
};
