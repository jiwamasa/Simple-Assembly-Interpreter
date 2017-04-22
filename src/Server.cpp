// Definition of MIS Server class
// Receives a program file from client
//   Parses it
//   Executes it
//   and resends output to client

#include "Server.h"

// Default constructor initializes fields of socket
// also creates a program object of name "name"
Server::Server(char* address, int port, int b_log) {
  server_so = new TCPServerSocket(address, port, b_log); // Make new socket
  if (!(server_so->initializeSocket())) // Initialize socket
    throw(new MisException("Failed to initialize socket\n"));
}

// Listen for a client and make connection
TCPSocket* Server::connect() {
  // Connect with client
  TCPSocket* connection = server_so->getConnection(0, 0, 1024, 512);
  if (connection == NULL) { // If connection fails, stop request
    std::cerr << "MIS_server: Failed to connect to client:\n";
  }
  return connection;
}

// Takes a connection to client and performs full request:
//   Reads in lines
//   Parses lines
//   Executes program
//   Sends over network the outputs
void Server::processRequest(TCPSocket* connection) {
  // Buffer for the -f flag
  char* flag_buf = (char*)calloc(1, sizeof(char));
  // Read flag from client
  connection->readFromSocketWithTimeout(flag_buf, 1, 10, 0);  
  int flag = std::stoi(flag_buf);
  
  Program* prog = new Program(); // Make a new program
  
  try {
    receiveTokens(connection, prog); // Read from client, store into program
    std::cout << "parsing...\n";
    parse(prog); // Parse program
    std::cout << "running...\n";
    run(flag, prog); // Execute program
  } catch (MisException* e) {
    std::cerr << "MIS_server: There was an error in the program\n";
    e->printToBuf(prog->ebuf); // Print error to error buffer
  }
  returnResults(connection, prog); // Send results back to client
  delete(prog);       // Delete program
  delete(connection); // Close connection
}

// Receive data line by line from client
// Insert tokens into token list of program
void Server::receiveTokens(TCPSocket* connection, Program* prog) {
  std::string file_buffer; // Full file buffer
  char* buffer = new char[1024]; // Buffer for one packet
  for (;;) { // Keep going until no more packets come though
    memset(buffer, 0, 1024); // zero out buffer
    // Attempt to read in from socket 
    int bytes = connection->readFromSocketWithTimeout(buffer, 1024, 1, 0); 
    if (bytes == -1) // If read failed, throw error
      throw(new MisException("MIS_server: read from socket failed\n"));
    if (bytes == 0) break; // Or if we're not reading in anymore, break
    file_buffer.append(std::string(buffer)); // Add packet to file
  }
  free(buffer);

  // Tokenize lines of files
  std::istringstream file_stream(file_buffer);
  std::string readline;
  for (size_t ln = 0; getline(file_stream, readline); ln++) {
    if ((Parse::ltrim(Parse::rtrim(readline)).empty())||
	(Parse::ltrim(Parse::rtrim(readline)) == "\r"))
	continue; // Skip empty lines
    std::vector<std::string> tokens;
    Parse::tokenize(tokens, readline, ln); // Tokenize line
    prog->addTokens(tokens); // Insert tokenized line into program
  }
}

// Parse the program object
void Server::parse(Program* prog) {
  Parse::parseProgram(prog);
}

// Run the program
void Server::run(int flag, Program* prog) {
  prog->execute(flag);
}

// Send the outputs back to the client
// Sends back token by token, so max size is 256 bytes
void Server::returnResults(TCPSocket* connection, Program* prog) {
  char* buffer = (char*)calloc(512,sizeof(char)); // Character buffer
  // Send normal output
  for (auto it = prog->obuf.begin(); it != prog->obuf.end(); it++) {
    memset(buffer, 0, 512); // Reset buffer
    sprintf(buffer, "%s", (char*)(*it).c_str() ); // Write to buffer
    // Attempt to write to buffer, counting number of bytes sent
    int bytes = connection->writeToSocket(buffer, 512);  
    if (bytes < 0) {  // If write failed, error out
      std::cerr << "MIS_server: Write to client failed\n";
      break;
    }
  }

  // Send sentinel signifying that we're sending error messages now
  // This sentinel is just 260 '!'s
  // Since a token can't be longer than 256, signal is unique
  char* err_sig = (char*)std::string(260, '!').c_str();
  if (connection->writeToSocket(err_sig, 512) < 0)
    std::cerr << "MIS_server: Failed to send err signal\n";
  
  // Send error output
  for (auto it = prog->ebuf.begin(); it != prog->ebuf.end(); it++) {
    memset(buffer, 0, 512); // Reset buffer
    sprintf(buffer, "%s", (char*)(*it).c_str() ); // Write to buffer
    // Attempt to write to buffer, counting number of bytes sent
    int bytes = connection->writeToSocket(buffer, 512);  
    if (bytes < 0) {  // If write failed, error out
      std::cerr << "MIS_server: Write to client failed\n";
      break;
    }
  }
  free(buffer);
}

Server::~Server() {
  delete(server_so);
}
