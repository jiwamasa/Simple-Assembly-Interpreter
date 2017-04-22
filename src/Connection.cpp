// Implementation of Connection class

#include "Connection.h"

// Constructor
Connection::Connection(Server* _server, TCPSocket* _client):
  server(_server), client_so(_client) {}

// Processes the request within a detached thread
void* Connection::threadMainBody(void* arg) {
  server->processRequest(client_so); // Process request
  return nullptr;
}

// Destructor
// Deletion is handled in Server object itself
Connection::~Connection() {}
