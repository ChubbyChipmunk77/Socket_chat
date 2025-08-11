#include "socketutil.c"
#include "socketutil.h"
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h>

int serverSocketFD;
struct sockaddr_in *serverSocketAddr;

// metadata about a client's connection to the server
struct clientConnectMeta {
  // its actually the client's socket file descriptor , bad naming scheme!!
  int clientSocket;
  struct sockaddr_in clientAddr;
  bool connectionEstablished;
  int err;
};

struct clientConnectMeta *createClientConnectMeta(int socketFD);
void rwFromClientSock(int clientSocket);
void acceptConnectionRW(int serverSocketFD);
pthread_t acceptIncomingConnections(int serverSocketFD);
void *acceptConnectionRW_wrapper(void *arg);
void rwIncomingDataSeparateThread(struct clientConnectMeta *clientinfo);

int main() {
  serverSocketFD = createSockTcpIPv4();
  serverSocketAddr = createSockAddr("", 2000);

  int result = bind(serverSocketFD, (const struct sockaddr *)serverSocketAddr,
                    sizeof *serverSocketAddr);
  if (result < 0) {
    perror("bind failed");
    return 1;
  }

  printf("server has been binded with a socket addr\n");
  int listenResult = listen(serverSocketFD, 10);
  if (listenResult == 0) {
    printf("Server is listening at 127.0.0.1:2000 !!\n");
  } else {
    printf("Server couldnt listen, maybe port is occupied !!\n");
  }

  // had to return pid cause what i did earlier was had a
  // uninitialized pid in the main function and created a thread
  // using copy over that value in a function , so thread_create
  // filled the function's copy over value with the pid not the
  // original one in the main function, which resulted in
  // segfault errors due to pthread_join using and uninitialized pid.

  // I also closed the clientScoketFD which was previously
  // an uninitialized global variable made for some testing
  // fixed that with changing it into clientSocket
  pthread_t pid = acceptIncomingConnections(serverSocketFD);

  pthread_join(pid, NULL);
  // shutdown(serverSocketFD, SHUT_RDWR);
  printf("The channel has been closed :( \n");
}

// adapter function for our threadable function.
void *acceptConnectionRW_wrapper(void *arg) {
  int serverSocketFD = *(int *)arg; // cast back to int
  free(arg);                        // if you malloc it in the caller
  acceptConnectionRW(serverSocketFD);
  return NULL;
}

pthread_t acceptIncomingConnections(int serverSocketFD) {
  int *arg =
      malloc(sizeof(int)); // allocate so it's valid after function returns
  *arg = serverSocketFD;
  pthread_t pid;
  pthread_create(&pid, NULL, acceptConnectionRW_wrapper, arg);
  return pid;
}

void acceptConnectionRW(int serverSocketFD) {

  while (1) {
    struct clientConnectMeta *clientSocketinfo =
        createClientConnectMeta(serverSocketFD);
    if (clientSocketinfo->connectionEstablished) {
      printf("A new connection arrives!\n");
    }
    rwIncomingDataSeparateThread(clientSocketinfo);
  }
}

void *rwFromClientSock_wrapper(void *arg) {
  int clientSocket = *(int *)arg;
  free(arg);
  rwFromClientSock(clientSocket);
  return NULL;
}

void rwIncomingDataSeparateThread(struct clientConnectMeta *clientinfo) {
  pthread_t pid;
  int *arg = malloc(sizeof(int));
  *arg = clientinfo->clientSocket;
  pthread_create(&pid, NULL, rwFromClientSock_wrapper, arg);
}

void rwFromClientSock(int clientSocket) {

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

struct clientConnectMeta *createClientConnectMeta(int socketFD) {
  struct clientConnectMeta *clientMeta =
      malloc(sizeof(struct clientConnectMeta));
  struct sockaddr_in clientAddress; // local variable with actual memory
  socklen_t clientAddrsize = sizeof(clientAddress);

  int clientSocketFD =
      accept(socketFD, (struct sockaddr *)&clientAddress, &clientAddrsize);

  clientMeta->clientSocket = clientSocketFD;
  clientMeta->clientAddr = clientAddress;
  clientMeta->connectionEstablished = clientSocketFD > 0;

  if (clientSocketFD < 0)
    clientMeta->err = clientSocketFD;

  return clientMeta;
}
