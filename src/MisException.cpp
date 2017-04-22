// Definition of Exception class

#include "MisException.h"

// Default constructor for single message
MisException::MisException(std::string message_):message(message_) {}

// Default constructor for message and line
MisException::MisException(std::string message_, 
			   std::vector<std::string> tokens_):
  message(message_), tokens(tokens_) {}

// Add onto error message
void MisException::addMessage(std::string message_) {
  message += message_;
}

// Add token line
void MisException::addLine(std::vector<std::string> tokens_) {
  for(auto it = tokens_.begin(); it != tokens_.end(); it++) 
    tokens.push_back(*it);
}

// Print error info
void MisException::print() {
  std::cerr << message; // Print base message
  if(tokens.empty()) return; // If it's a simple error, there is no line 
  std::cerr << "  line " << std::stoi(tokens[0])
         << ": " << tokens[1] << " ";
  if(tokens.size() > 2) { // Print out other args if available
    for(auto it = tokens.begin()+2; it != tokens.end(); it++) {
      std::cerr << (*it);
      if(it != tokens.end()-1) std::cerr << ",";
    }
  }
  std::cerr << "\n";
}

// Print error info to a string buffer
void MisException::printToBuf(std::vector<std::string> &str_buf) {
  str_buf.push_back(message); // Print base message
  if(tokens.empty()) return; // If it's a simple error, there is no line 
  str_buf.push_back("  line " + tokens[0] +
                    ": " + tokens[1] + " ");
  if(tokens.size() > 2) { // Print out other args if available
    for(auto it = tokens.begin()+2; it != tokens.end(); it++) {
      str_buf.push_back(*it);
      if(it != tokens.end()-1) str_buf.push_back(",");
    }
  }
  str_buf.push_back("\n");
}

MisException::~MisException() {}; // Destructor
