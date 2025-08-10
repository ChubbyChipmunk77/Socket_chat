#include <arpa/inet.h>
#include <malloc.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int createSockTcpIPv4();
struct sockaddr_in *createSockAddr(char *ip, int port);
