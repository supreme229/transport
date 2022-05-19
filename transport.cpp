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
        exit(EXIT_FAILURE);
    }

    return true;
}

bool Transport::serverAddressSetup()
{
    bzero(&server_address, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = port;

    if (!inet_pton(AF_INET, ip_addr, &server_address.sin_addr))
    {
        fprintf(stderr, "inet_pton error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 1;
}

bool Transport::setup()
{
    socketSetup();
    serverAddressSetup();

    return 1;
}

bool Transport::sendSingleDatagram(Segment segment)
{
    int message_len = strlen(segment.send);

    if (sendto(sockfd, segment.send, message_len, 0, (struct sockaddr *)&server_address, sizeof(server_address)) != message_len)
    {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 1;
}

bool Transport::sendWindow()
{
    for (int i = 0; i < w.getCurrentWindowSize(); i++)
    {
        Segment segment = w.getSpecificSegment(i);
        sendSingleDatagram(segment);
    }

    return 1;
}

bool Transport::receivedInTime()
{
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500000;

    int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
    if(ready < 0){
        fprintf(stderr, "select error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (tv.tv_sec == 0 && tv.tv_usec == 0)
    {
        return 0;
    }

    return 1;
}

bool Transport::receivePacket(int *start)
{
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    char data[1040];

    bzero(&sender, sizeof(sender));
    bzero(data, 1040);

    int datagram_len = recvfrom(sockfd, data, IP_MAXPACKET, 0, (struct sockaddr *)&sender, &sender_len);

    if (datagram_len < 0)
    {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (sender.sin_port != port || strcmp(inet_ntoa(sender.sin_addr), ip_addr) != 0)
    {
        return 0;
    }

    char sender_ip_str[20];
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

    int bytes_received = 0;

    sscanf((const char *)data, "DATA %d %d\n", start, &bytes_received);

    int idx = w.findSegment(*start);

    if (idx < 0)
    {
        return 0;
    }

    int offset = datagram_len - bytes_received;

    w.fillSegment(*start, data, offset);

    return 1;
}

bool Transport::isAcked(int start)
{
    int idx = w.findSegment(start);

    bool is_acked = w.checkSegmentAck(idx);

    if (!is_acked)
    {
        w.switchSegmentAck(idx);

        if (idx == 0)
        {
            int deleted = w.deleteSegments(fd, &bytes_left);

            if (deleted)
            {
                w.addSegments(deleted);
            }
        }

        return 0;
    }

    return 1;
}

void Transport::receiveFile()
{
    while (bytes_left > 0)
    {
        sendWindow();

        if (receivedInTime())
        {
            int start = 0;

            if (receivePacket(&start))
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