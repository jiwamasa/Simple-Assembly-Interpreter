// Class declaration of threaded connection class
// Allows server to process mutiple requests at once
//
// Uses Thread Library provided by professor Sobh

#ifndef __CONNECTION__
#define __CONNECTION__

#include "Server.h"

class Connection : public Thread {
 private:
  Server* server;       // Server object contains sever socket
  TCPSocket* client_so; // Client socket
 public:
  Connection(Server* _server, TCPSocket* _client); // Constructor
  virtual void* threadMainBody(void* arg); // Thread logic
  ~Connection(); // Destructor
};

#endif
