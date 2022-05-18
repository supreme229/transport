#include "utils.h"

bool validateInput(int argc, char *argv[]) {
  if (argc != 5) {
    return false;
  }

  struct sockaddr_in sa;
  int is_correct_ip_addr = inet_pton(AF_INET, argv[1], &(sa.sin_addr));

  if (is_correct_ip_addr != 1) {
    return false;
  }

  int port_number = -1, file_size = -1;

  try {
    port_number = stoi((string)(argv[2]));
    file_size = stoi((string)(argv[4]));
  } catch (invalid_argument const &ex) {
    return false;
  }

  if (!(port_number >= 0 && port_number <= 65535)) {
    return false;
  }

  if (file_size < 0) {
    return false;
  }

  return true;
}