#include "window.h"

#include <cmath>

Window::Window(int file_size) : file_size(file_size)
{
    window_size = min((int)(ceil(file_size / 1000.0)), MAX_WINDOW_SIZE);
    window = deque<Segment>(window_size);

    start = 0;
    bytes_left = file_size;
    deleted = 0;

    for (int i = 0; i < window_size; i++)
    {
        int bytes_to_receive = calculateBytes(bytes_left);
        char message[40];
        sprintf(message, "GET %d %d\n", start, bytes_to_receive);

        Segment segment;

        segment.is_ack = false;
        segment.bytes = bytes_to_receive;
        segment.start = start;
        strcpy(segment.send, message);

        window[i] = segment;

        bytes_left -= bytes_to_receive;
        start += bytes_to_receive;
    }
}

int Window::getStart(){
    return start;
}

void Window::incrementStart(int offset){
    start += offset;
}

int Window::getBytesLeft(){
    return bytes_left;
}

void Window::decrementBytesLeft(int bytes){
    bytes_left -= bytes;
}

void Window::setDeleted(int deleted){
    this->deleted += deleted;
}


void Window::setSegmentAck(int start)
{
    for (auto &segment : window)
    { 
        if (segment.start == start)
        {
            segment.is_ack = true;
        }
    }
}

int Window::getFirstNotAck()
{
    for(uint i = 0; i < window.size();i++){
        if(!window[i].is_ack){
            return i;
        }
    }

    return -1;
}

int Window::findSegment(int start){
    int result = floor(start / 1000.0) - deleted;

    if(result < 0){
        return -1;
    }

    if(window[result].start != start){
        return -1;
    }

    return result;
}

int Window::calculateBytes(int bytes_left)
{
    return min(bytes_left, min(file_size, MAX_DATA_LEN));
}