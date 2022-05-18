#include "window.h"

#include <cmath>

Window::Window(int file_size) : file_size(file_size), window_size(ceil(file_size / 1000.0)), window(window_size)
{
    int bytes_left = file_size;
    int position = 0;

    for (int i = 0; i < window_size; i++)
    {
        int bytes_to_receive = calculateBytes(bytes_left);
        char message[40];
        sprintf(message, "GET %d %d\n", position, bytes_to_receive);

        Segment segment;
        segment.is_ack = false;
        segment.bytes = bytes_to_receive;
        strcpy(segment.send, message);
        segment.start = position;

        window[i] = segment;

        bytes_left -= bytes_to_receive;
        position += bytes_to_receive;

        //  cout << "WINDOW[" << i << "]" << endl;
        //  cout << "IS_ACK: " << window[i].is_ack <<  endl;
        //  cout << "BYTES: " << window[i].bytes << endl;
        //  cout << "MESSAGE: " << window[i].send;
        //  cout << endl;
    }
}

int Window::getSegmentBytesToReceive(int idx){
    return window[idx].bytes;
}

char* Window::getSegmentMessage(int idx){
    return window[idx].send;
}

int Window::getSegmentStart(int idx){
    return window[idx].start;
}

void Window::setSegmentAck(int start){
    for(auto& segment : window){
        if(segment.start == start){
            segment.is_ack = true;
        }
    }
}

Segment Window::getFirstNotAck(){
    for(auto& segment : window){
        if(!segment.is_ack){
            return segment;
        }
    }
    return window[0];
}

int Window::calculateBytes(int bytes_left)
{
    return min(bytes_left, min(file_size, MAX_DATA_LEN));
}