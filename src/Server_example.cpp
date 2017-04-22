#include "TCPSocket.h"
#include "TCPServerSocket.h"

//Server socket
int main(int argc, char** argv) {
  std::cout << "Starting server...\n";
  TCPServerSocket* my_server = new TCPServerSocket(argv[1], 9999, 10);
  if (!(my_server->initializeSocket())) {
    std::cout << "Socket init failed\n";
    exit(1);
  }

  TCPSocket* my_connection = my_server->getConnection(0, 0, 1024, 1024);
  if (my_connection == NULL) {
    std::cout << "Couldn't make connection\n";
    exit(1);
  }

  char* buffer = (char*)calloc(1024,sizeof(char));
  if (my_connection->readFromSocket(buffer, 1024) == -1) {
    std::cout << "Couldn't read from connection\n";
    exit(1);
  }

  std::cout << "Received:\n" << buffer;
  
  free(buffer);
  delete (my_connection);
  delete (my_server);
  return 0;
}
