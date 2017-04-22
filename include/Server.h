// Declaration of Server class
// Receives a .mis file from client and exectutes it
// It then sends output back to client

#ifndef __SERVER__
#define __SERVER__

#include "TCPSocket.h"
#include "TCPServerSocket.h"
#include "MisException.h"
#include "Parse.h"
#include "Program.h"

class Server {
 private:
  TCPServerSocket* server_so; // Server socket
 public:
  // Default constructor sets up the TCPServerSocket
  // Also takes in the name of the program
  Server(char* address, int port, int b_log);
  // Wait for a connection
  TCPSocket* connect();
  // Take in connection, read/parse/run file, return to client output
  void processRequest(TCPSocket* connection);
  // Get lines from client, insert them as tokens into token list of program
  void receiveTokens(TCPSocket* connection, Program* prog);
  void parse(Program* prog); // Parse program
  // Return results to client
  void returnResults(TCPSocket* connection, Program* prog);
  void run(int flag, Program* prog); // Run program w/ flags
  // Destructor closes socket and deletes program
  ~Server();
};

#endif
