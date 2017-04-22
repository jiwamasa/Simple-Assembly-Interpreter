// MIS Server
// Accepts mis program from a client, processes it, and returns the results
// Can accept mutliple requests in parallel via multihreading
//
// Usage: MIS_server [address] port
//
// Address is optional. If none specified, Server will listen on all
// available addresses.

#include "MisException.h"
#include "Connection.h"

using namespace std;

int main(int argc, char** argv) {
  char* address = (char*)calloc(32, sizeof(char)); // Server Address
  int port; // Port number

  if (argc == 3) { // If address is specified, set accordingly
    strcpy(address, argv[1]);
    port = std::stoi(argv[2]);
  } else if (argc == 2) { // If only port is specified, address = NULL
    free(address);
    address = NULL;
    port = std::stoi(argv[1]);
  } else { // Otherwise, throw error
    std::cerr << "Usage: MIS_server [address] port\n";
    exit(1);
  }
  
  // Make a new server  with backlog of 10
  // NULL address means listen on all addresses
  Server* my_server = new Server(address, port, 10);
  for (;;) { // Accept arbitrary number of clients
    TCPSocket* connection = my_server->connect();
    Connection* c = new Connection(my_server, connection);
    c->start(); // Start thread
  }

  if (address != NULL) free(address);
  delete(my_server);
  return 0;
}
