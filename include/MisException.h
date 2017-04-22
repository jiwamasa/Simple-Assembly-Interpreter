// Exception class declaration
// Contains error information 
// Exceptions are typically thrown bottom up through hiearchy to main, where 
//   they are then printed. 

#ifndef __MIS_EXCEPTION__
#define __MIS_EXCEPTION__

#include "common.h"

class MisException {
 protected:
  std::string message; // Error message
  std::vector<std::string> tokens; // Tokenized line where error occured
 public:
  MisException(std::string); // Default constructor for simpler errors
  // Default constructor for erros that reference a line
  MisException(std::string, std::vector<std::string>); 
  void print(); // Print formatted error
  void printToBuf(std::vector<std::string> &str_buf); // Print error to buffer
  void addMessage(std::string); // Add onto message
  void addLine(std::vector<std::string>); // Add a tokenized line
  ~MisException(); // Destructor
};

#endif
