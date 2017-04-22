// Implementation of Client class
// Processes the input file and sends it to the server for running

#include "Client.h"

Client::Client(int c_argc, char** c_args) {
  int argi = checkFlags(c_argc, c_args); // Check options\set flags
  char* address = c_args[argi];          // Get server address
  int port = std::stol(c_args[argi+1]);  // Get port number
  fname = std::string(c_args[argi+2]);   // Get filename
  // Check filename and get basename
  try { basename = getBasename(); }
  catch (...) { // Catch filename parsing error
    std::cerr << "MIS_client: " << fname 
	 << ": Bad filename; format is \"filename.mis\"\n";
    exit(1);
  }  
  // Make new client socket 
  client_so = new TCPSocket(address, port, 1024, 1024);
}

// Takes in args and sees if any options are specified:
//   -f "free mode"  : can have long files and long output
// Returns position of first non option argument
int Client::checkFlags(int c_argc, char** c_args) {
  if ((c_argc < 4)||(c_argc > 5)) { 
    std::cerr << "Usage: MIS [-f] server_address server_port filename\n"; 
    exit(1); 
  }
  if (std::string(c_args[1]) == "-f") { // Check if option
    flag = 1; // Set option flag
    return 2;
  } // Otherwise, there are no options
  return 1;
}

// Check filename validity and return basename
std::string Client::getBasename() {
  size_t dir_pos = fname.find_last_of('/'); // Get position of directory
  size_t ext_pos = fname.find_last_of('.'); // Get position of extension
  // Check extension to make sure it's ".mis"
  std::string ext = fname.substr(ext_pos, 4);
  if (ext != ".mis") { // If bad filename, throw error
    throw(new MisException("MIS_client: Bad filename: " + fname)); 
    return nullptr;
  }
  return fname.substr(dir_pos + 1,ext_pos - dir_pos - 1);
}

// Send lines of text to server
void Client::sendLines(std::ifstream &infile) {
  // Get length of file
  infile.seekg(0, infile.end);
  long length = infile.tellg(); 
  infile.seekg(0, infile.beg);

  // If longer than ~1024 lines, send warning
  // Can be ignored if -f flag set
  if ((length > 1024*128)&&(flag == 0)) {
    std::cerr << "Warning: Especially long file: stopping execution...\n"
	      << "To ignore warning, use -f flag\n";
    exit(1);
  }
  
  char* buffer = new char[length];         // Make buffer for whole file
  infile.read(buffer, length);             // Read file into buffer
  long bytes = client_so->writeToSocket(buffer, length); // Send file
  if (bytes < 0)
    throw(new MisException("Mis_client: Failed to send file\n"));

  free(buffer);
}

// Receive results of execution from Server
// Put into two categories: normal output and errors
// When to switch is signified by receiving a sentinel packet which is
//   just a window of 260 with all '!'
// This won't conflict, since longest normal token length is 256
// Note that error messages may be longer than 256 per token  
void Client::receiveResults(std::ofstream &out, std::ofstream &err) {
  char* buffer = (char*)calloc(512,sizeof(char)); // Buffer 
  std::ofstream* curr = &out; // Current file we're writing to
  for (;;) { // Keep reading until input is done
    memset(buffer, 0, 512); //clear buffer
    // Attempt to read in from socket
    // Timeout if execution takes longer than 60 seconds
    //   Can be turned off with -f flag (NOT IMPLEMENTED YET)
    int bytes = client_so->readFromSocketWithTimeout(buffer, 512, 60, 0);
    if (bytes == -1) // If read failed, throw error
      throw(new MisException("failed to read from socket\n"));
    if (bytes == 0) break; // If we're done, break
    if (buffer[259] == '!') { // If we received switch sentinel
      curr = &err; // Switch to printing to error file
      continue; 
    }
    *curr << buffer; // Print to file
  } 
  free(buffer);
}

// Process user's full request
void Client::processRequest() {
  std::ifstream infile(fname); // Open .mis file
  if((infile.bad())||(infile.fail())) { // Check if file is bad
    std::cerr << "Bad file\n";
    exit(1);
  }

  // Convert flag to char* for sending
  char* flag_buf = (char*)(std::to_string(flag)).c_str();
  client_so->writeToSocket(flag_buf, 1); // Send flag
  
  sendLines(infile); // Send text data line by line
  infile.close();    // Close file

  // Create output files in current directory
  std::ofstream outf("./" + basename + ".out");
  std::ofstream errf("./" + basename + ".err");
  receiveResults(outf, errf); // Receive results from server
  outf.close(); // Close output files
  errf.close();
}

// Destructor closes client socket
Client::~Client() { delete(client_so); }
