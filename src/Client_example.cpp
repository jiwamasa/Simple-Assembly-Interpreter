#include "TCPSocket.h"
#include "TCPServerSocket.h"

//Client socket
int main(int argc, char** argv) {
  std::cout << "Starting client...\n";
  TCPSocket* my_client = new TCPSocket(argv[1], 9999, 1024, 1024);
  std::cout << "My address: " << my_client->getMyAddress() << "\n"
            << "Remote address: " << my_client->getRemoteAddress() << "\n";
    
  char* buffer = (char*)calloc(1024,sizeof(char));
  sprintf(buffer, "%s", "Hello, sockets!\n");

  if (my_client->writeToSocket(buffer, 1024) == -1) {
    std::cout << "Couldn't write to socket\n";
    exit(1);
  }

  free(buffer);
  delete (my_client);
  return 0;
}
