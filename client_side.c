#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

int main() {
  // create a socket which returns a socket
  // socket api takes ip addr format , which in
  // this case is AF_INET or ipv4, it also takes which
  // transfer protocol to use , we here are using tcp
  // thats why its set to SOCK_STREAM, the last argument
  // is which protocol to use underlying our current layer
  // we have tcp in our transport layer so we would like to have ip
  // as out underlying layer and for that we chose 0.
  int socketFD =
      socket(AF_INET, SOCK_STREAM, 0); // here AF stands for address Family

  // sockets are implemented as file descriptors from the os point
  // of view , so it can read and write to a socket just like,
  // it does on a file through its file descriptors.
  // a FD is nothing but just a token in int given to us by the OS,
  // which represents the opened file . the OS keep a track of all the
  // opened I/O files in the system. using the FD we can connect to some
  // socket sitting idle.

  // here our ip is in a presentation format but
  // the connect api takes the ip in unsigned int binary forrmat
  // which is understandable by the os , inet presentation to
  // network does that and it takes the address of the buffer space
  // in memory to store that binary presentation

  char *ip = "142.250.76.174"; // this is google.com
  struct sockaddr_in address;
  address.sin_family = AF_INET;

  // htons is the function to convert between the host and
  // the network byte order. network expects a big endian byte order
  // but some devices store it in little endian.
  address.sin_port = htons(80); // for http request
  inet_pton(AF_INET, ip, &address.sin_addr.s_addr);

  // here we will make a connection of the client side socket
  // to a server to with its complete address.
  // info has been encapsulated in the address struct.
  int result =
      connect(socketFD, (const struct sockaddr *)&address, sizeof address);
  if (result == 0) {
    printf("connection was successful!!! \n");
  }
}
