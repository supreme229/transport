#include "transport.h"

Transport::Transport(char *argv[]) : ip_addr(argv[1]), port(stoi((string)argv[2])), file_name(argv[3]), file_size(stoi((string)argv[4])), bytes_left(file_size)
{
    file = fopen(file_name, "w");
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
    server_address.sin_port = htons(port);

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

bool Transport::sendDatagram(int position)
{
    int bytes_to_receive = calculateBytes();

    char message[40];
    sprintf(message, "GET %d %d\n", position, bytes_to_receive);

    int message_len = strlen(message);

    if (sendto(sockfd, message, message_len, 0, (struct sockaddr *)&server_address, sizeof(server_address)) != message_len)
    {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool Transport::receivedInTime()
{
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

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

    *datagram_len = recvfrom(sockfd, data, IP_MAXPACKET, 0, (struct sockaddr *)&sender, &sender_len);

    if (*datagram_len < 0)
    {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    char sender_ip_str[20];
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

    sscanf((const char *)data, "DATA %d %d\n", start, bytes_received);

    return EXIT_SUCCESS;
}

void Transport::receiveFile()
{
    int bytes_read = 0;
    while (bytes_left > 0)
    {
        sendDatagram(bytes_read);
        if (receivedInTime())
        {
            int bytes_received = 0, datagram_len =0 , start = 0;
            char data[1040];

            if (receivePacket(&start, &datagram_len, &bytes_received, data) == EXIT_SUCCESS)
            {
                if (start != bytes_read) continue;
                bytes_left -= bytes_received;
                bytes_read += bytes_received;
                fwrite(data + datagram_len - bytes_received, sizeof(char), bytes_received, file);
            }
        }
    }
    fclose(file);
}