#include "utils.h"

bool validateInput(int argc, char *argv[])
{
  if (argc != 5)
  {
    return 0;
  }

  struct sockaddr_in sa;
  int is_correct_ip_addr = inet_pton(AF_INET, argv[1], &(sa.sin_addr));

  if (is_correct_ip_addr != 1)
  {
    return 0;
  }

  int port_number = -1, file_size = -1;

  try
  {
    port_number = stoi((string)(argv[2]));
    file_size = stoi((string)(argv[4]));
  }
  catch (invalid_argument const &ex)
  {
    return 0;
  }

  if (!(port_number >= 0 && port_number <= 65535))
  {
    return 0;
  }

  if (file_size < 0)
  {
    return 0;
  }

  return 1;
}

int calculateBytes(int bytes_left, int file_size)
{
  return min(bytes_left, min(file_size, MAX_DATA_LEN));
}