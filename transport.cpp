#include "transport.h"

Transport::Transport(char *argv[]) : ip_addr(argv[1]), port(htons(stoi((string)argv[2]))), file_name(argv[3]), file_size(stoi((string)argv[4])), bytes_left(file_size), w(file_size), deleted(0)
{
    fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC);
}

bool Transport::socketSetup()
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool Transport::serverAddressSetup()
{
    bzero(&server_address, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = port;

    if (!inet_pton(AF_INET, ip_addr, &server_address.sin_addr))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool Transport::setup()
{
    if (socketSetup() | serverAddressSetup())
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int Transport::calculateBytes()
{
    return min(bytes_left, min(file_size, MAX_DATA_LEN));
}

bool Transport::sendSingleDatagram(Segment segment)
{
    int message_len = strlen(segment.send);

    if (sendto(sockfd, segment.send, message_len, 0, (struct sockaddr *)&server_address, sizeof(server_address)) != message_len)
    {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool Transport::sendWindowDatagrams()
{
    for (int i = 0; i < w.window.size(); i++)
    {
        if (sendSingleDatagram(w.window[i]) == EXIT_FAILURE)
            return false;
    }

    return true;
}

int Transport::deleteDatagrams()
{
    int elements_deleted = 0;

    while (w.window.size())
    {
        if (w.window[0].is_ack)
        {
            elements_deleted++;

            write(fd, &w.window[0].data, w.window[0].bytes);

            bytes_left -= w.window[0].bytes;

            w.window.pop_front();
        }
        else
        {
            break;
        }
    }

    w.setDeleted(elements_deleted);

    return elements_deleted;
}

void Transport::addDatagrams(int max_new_elements)
{
    if (w.getBytesLeft() == 0)
    {
        return ;
    }

    int counter = 0;

    for (int i = 0; i < max_new_elements; i++)
    {
        int bytes_to_receive = w.calculateBytes(w.getBytesLeft());
        char message[40];

        int start = w.getStart();

        sprintf(message, "GET %d %d\n", start, bytes_to_receive);

        Segment segment;
        segment.is_ack = false;
        segment.bytes = bytes_to_receive;
        strcpy(segment.send, message);
        segment.start = start;

        w.window.push_back(segment);

        w.decrementBytesLeft(bytes_to_receive);
        w.incrementStart(bytes_to_receive);


        if (w.getBytesLeft() == 0)
        {
            break;
        }
    }
}

bool Transport::receivedInTime()
{
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);

    if (tv.tv_sec == 0 && tv.tv_usec == 0)
    {
        return false;
    }

    return true;
}

bool Transport::receivePacket(int *start, int *datagram_len, int *bytes_received, char *data)
{
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    bzero(&sender, sizeof(sender));
    bzero(data, 1040);

    *datagram_len = recvfrom(sockfd, data, IP_MAXPACKET, 0, (struct sockaddr *)&sender, &sender_len);

    if (sender.sin_port != port || strcmp(inet_ntoa(sender.sin_addr), ip_addr) != 0)
    {
        return EXIT_FAILURE;
    }

    if (*datagram_len < 0)
    {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    char sender_ip_str[20];
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

    sscanf((const char *)data, "DATA %d %d\n", start, bytes_received);

    int idx = w.findSegment(*start);

    if (idx < 0)
    {
        return EXIT_FAILURE;
    }

    int start_point = *datagram_len - *bytes_received;

    memcpy(w.window[idx].data, &data[start_point], *bytes_received);

    return EXIT_SUCCESS;
}

bool Transport::isAcked(int start)
{
    int idx = w.findSegment(start);

    if (idx < 0)
    {
        throw new invalid_argument("zjebany is acked");
    }

    if (!w.window[idx].is_ack)
    {
        w.window[idx].is_ack = true;

        if (idx == 0)
        {
            int deleted = deleteDatagrams();
            addDatagrams(deleted);
        }

        return false;
    }

    return true;
}

void Transport::receiveFile()
{
    while (bytes_left > 0)
    {
        int start_checker = 0;

        //cout << w.window.size() << endl;

        sendWindowDatagrams();

        if (receivedInTime())
        {
            int bytes_received = 0, datagram_len = 0, start = 0;
            char data[1040] = "";

            if (receivePacket(&start, &datagram_len, &bytes_received, data) == EXIT_SUCCESS)
            {
                if (isAcked(start))
                {
                    continue;
                }
            }
        }
    }
    close(fd);
}