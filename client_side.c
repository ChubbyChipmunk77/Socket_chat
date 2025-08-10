// header contains all the imported headers and the declarations of the
// functions; the .c file has all the declared funciton implementations.

#include "socketutil.c"
#include "socketutil.h"

int main() {

  char *ip = "172.217.174.78";
  int port = 80;
  int socketFD = createSockTcpIPv4();
  struct sockaddr_in *address = createSockAddr(ip, port);
  // here we will make a connection of the client side socket
  // to a server to with its complete address.
  // info has been encapsulated in the address struct.
  int result =
      connect(socketFD, (const struct sockaddr *)address, sizeof *address);
  if (result == 0) {
    printf("connection was successful!!! \n");
  }

  char *message = "GET / HTTP/1.1\r\nHost: google.com\r\n\r\n";
  char buffer[1024];
  send(socketFD, message, strlen(message), 0);
  recv(socketFD, buffer, 1024, 0);
  printf("The response is \n%s", buffer);
}
