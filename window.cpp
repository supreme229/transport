#include "window.h"

Window::Window(int file_size) : file_size(file_size), window_size(min((int)(ceil(file_size / 1000.0)), MAX_WINDOW_SIZE)), window(window_size), start(0), bytes_left(file_size), deleted(0)
{
    for (int i = 0; i < window_size; i++)
    {
        Segment segment = buildSegment();

        window[i] = segment;
    }
}

Segment Window::buildSegment()
{
    int bytes_to_receive = calculateBytes(bytes_left, file_size);

    char message[40];

    int characters = sprintf(message, "GET %d %d\n", start, bytes_to_receive);

    if(characters < 0){
        fprintf(stderr, "sprintf error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    Segment segment;

    segment.is_ack = false;
    segment.bytes = bytes_to_receive;
    strcpy(segment.send, message);
    segment.start = start;

    bytes_left -= bytes_to_receive;
    start += bytes_to_receive;

    return segment;
}

int Window::getCurrentWindowSize()
{
    return window.size();
}

Segment Window::getSpecificSegment(int idx)
{
    return window[idx];
}

void Window::addSegments(int max_new_elements)
{
    if (bytes_left == 0)
    {
        return;
    }

    for (int i = 0; i < max_new_elements; i++)
    {
        Segment segment = buildSegment();

        window.push_back(segment);

        if (bytes_left == 0)
        {
            break;
        }
    }
}

int Window::deleteSegments(int fd, int *bytes_left)
{
    int elements_deleted = 0;

    while (window.size())
    {
        if (!window[0].is_ack)
        {
            break;
        }

        elements_deleted++;

        int written = write(fd, &window[0].data, window[0].bytes);

        if(written < 0){
            fprintf(stderr, "write error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        *bytes_left -= window[0].bytes;

        window.pop_front();
    }

    deleted += elements_deleted;

    return elements_deleted;
}

int Window::findSegment(int start)
{
    int result = floor(start / 1000.0) - deleted;

    if (result < 0)
    {
        return -1;
    }

    return result;
}

void Window::fillSegment(int start, char *data, int offset)
{
    int idx = findSegment(start);

    memcpy(window[idx].data, &data[offset], window[idx].bytes);
}

bool Window::checkSegmentAck(int idx)
{
    return window[idx].is_ack;
}

void Window::switchSegmentAck(int idx)
{
    window[idx].is_ack = true;
}
