// header contains all the imported headers and the declarations of the
// functions; the .c file has all the declared funciton implementations.

#include "socketutil.c"
#include "socketutil.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stddef.h>
#include <unistd.h>

void recvAndPrint(int clientSocket);
void recvSeparateThread(int clientSocket);
int main() {

  char *ip = "127.0.0.1";
  int port = 2000;
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

  recvSeparateThread(socketFD);
  char buffer[1024];
  while (1) {
    char *line = NULL;
    size_t linelen = 0;
    size_t charCount = getline(&line, &linelen, stdin);
    line[charCount] = '\0';

    if (charCount > 0) {
      if (strcmp(line, "exit\n") == 0) {
        break;
      } else {
        size_t amountSent = send(socketFD, line, charCount, 0);
      }
    }
  }

  close(socketFD);
  printf("The channel has been closed :(\n");
  // http get request => "GET / HTTP/1.1\r\nHost: google.com\r\n\r\n";
}

void *recvAndPrint_wrapper(void *arg) {
  int clientSocket = *(int *)arg;
  free(arg);
  recvAndPrint(clientSocket);
  return NULL;
}

void recvSeparateThread(int clientSocket) {
  pthread_t tid;
  int *sockPtr = malloc(sizeof(int)); // must be heap-allocated
  *sockPtr = clientSocket;

  if (pthread_create(&tid, NULL, recvAndPrint_wrapper, sockPtr) != 0) {
    perror("pthread_create");
    free(sockPtr);
    return;
  }

  pthread_detach(tid); // <-- key: no join needed
}

void recvAndPrint(int clientSocket) {

  char buffer[1024];
  while (1) {

    // As for now accept is not here as it is a blocking code !!!
    // currently we will only deal with single connections.

    int bytesRead = recv(clientSocket, buffer, 1024, 0);
    if (bytesRead > 0) {
      buffer[bytesRead] = '\0'; // null terminate at the end of the new data
      printf("Received message -> %s\n", buffer);
    } else if (bytesRead == 0) {
      break;
    }
  }
  close(clientSocket);
}
