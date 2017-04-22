// Main for MIS client
// Processes the input file and sends it to the server for running

#include "Client.h"

using namespace std;

int main(int argc, char** argv) {
  // Make new Client object, which opens the socket
  Client* my_client = new Client(argc, argv); 
  my_client->processRequest();
  delete(my_client);
  return 0;
}
