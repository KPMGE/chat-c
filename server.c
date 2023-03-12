#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENT_CONNECTIONS 10

// server -> socket -> bind -> listen -> accept -> read -> write

void handle_client(int new_socket_fd) {
  // once a connection has been established, keep parsing messages from the client
  for (;;) {
    // create a simple buffer to read data into
    char buffer[BUFFER_SIZE];
    // read received data into buffer
    int bytes_read = read(new_socket_fd, buffer, BUFFER_SIZE);

    // an error has occurred
    if (bytes_read == -1) {
      perror("error when reading");
      exit(1);
    }

    if (bytes_read == 0) {
      printf("connection closed by client\n");
      break;
    }

    printf("message received: %s\n", buffer);

    const char *message_to_client = "hello client, i've received your message, ty\n";
    send(new_socket_fd, message_to_client, strlen(message_to_client), 0);

    // clear buffer
    memset(buffer, 0, sizeof(buffer));
  }

  close(new_socket_fd);
}

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

  for (;;) {
    // awaits connections on the socket, generating a new socket descriptor 
    socklen_t address_len = sizeof(address);
    int new_socket_fd = accept(socket_fd, (struct sockaddr *) &address, &address_len);
    if (new_socket_fd < 0) {
      perror("accept");
      exit(1);
    }

    printf("new client connected: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    handle_client(new_socket_fd);
  }
}
