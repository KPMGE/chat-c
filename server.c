#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENT_CONNECTIONS 10

// server -> socket -> bind -> listen -> accept -> read -> write

int main (int argc, char *argv[]) {
  // AF_INET     -> ipv4
  // SOCK_STREAM -> tcp protocol
  // 0           -> default for tcp
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    perror("socket failed");
    exit(1);
  }

  // struct for internet socket address
  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_addr.s_addr = INADDR_ANY,
    .sin_port = htons(PORT)
  };

  // bind socket to address
  if (bind(socket_fd, (const struct sockaddr *) &address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(1);
  }

  // prepare to accept connections on the socket
  if (listen(socket_fd, MAX_CLIENT_CONNECTIONS) < 0) {
    perror("listen");
    exit(1);
  }

  // awaits connections on the socket, generating a new socket descriptor 
  socklen_t address_len = sizeof(address);
  int new_socket_fd = accept(socket_fd, (struct sockaddr *) &address, &address_len);
  if (new_socket_fd < 0) {
    perror("accept");
    exit(1);
  }

  // create a simple buffer to read data into
  char buffer[BUFFER_SIZE];
  // read received data into buffer
  read(new_socket_fd, buffer, BUFFER_SIZE);

  printf("message received: %s\n", buffer);

  // close connected socket
  close(new_socket_fd);
  // shut server down
  shutdown(socket_fd, SHUT_RDWR);
}
