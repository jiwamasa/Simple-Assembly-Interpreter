// Declaration of client class
// Facilitates client requests and processing:
//   -Takes in command line arguments and file from user
//   -Checks flags and options
//   -Sends program to server for parsing/execution
//   -Receives output from server when done and prints to file

#ifndef __CLIENT__
#define __CLIENT__

#include "MisException.h"
#include "TCPSocket.h"

class Client {
 private:
  TCPSocket* client_so;          // Client socket
  std::string fname;             // Name of input file
  std::string basename;          // Basename of input file
  int flag;                      // "Free" -f option status (1 if set)
 public:
  // Constructor initializes socket from command line arguments
  Client(int c_argc, char** c_args);
  // Takes in args and sees if any options are specified:
  //   -f "free mode; nothing held back; can have very long files"
  // Returns position of first non-option argument
  int checkFlags(int c_argc, char** c_args);
  // Check filename validity and return basename
  std::string getBasename();
  // Send lines of text to server
  void sendLines(std::ifstream &infile);
  // Receive results of execution from Server
  void receiveResults(std::ofstream &out, std::ofstream &err);
  // Process request by user:
  //   -Send file to server
  //   -Receive results and print to .out and .err files
  void processRequest();
  // Destructor closes socket
  ~Client();
};

#endif
