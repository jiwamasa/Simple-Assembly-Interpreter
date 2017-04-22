// Interface for Instruction class
// Contains arguments to instruction and pointer to function

#ifndef __INSTRUCTION__
#define __INSTRUCTION__

#include "common.h"
#include "Variable.h"
#include "OpFuncs.h"

// Definition of type pointer to function
// This defines the functionality of an instruction
// It takes in the argument vector of the instruction as an argument
// Returns false if fails
// Functions declarations are found in header file OpFuncs.h 
// Specific implementations are defined in cpp files:
//   MathFuncs.cpp, StringFuncs.cpp, AddressFuncs.cpp, MiscFuncs.cpp
typedef bool (*OpFunc)(std::vector<Variable*>* args);

class Instruction {
 private:
  std::vector<Variable*> arguments; 
  OpFunc operation;
 public:
  Instruction(); // Default empty constructor
  Instruction(OpFunc f); // Default Constructor takes function as argument
  virtual bool doOperation(); // Run operation from operation field
  void setOperation(OpFunc f); // Set operations
  void addArgument(Variable* arg); // Add a new argument to the end
  std::vector<std::string> getStringArgs(); // Get arguments as strings
  void print_args(); // Prints all arguments 
  virtual ~Instruction(); // Destructor
};

#endif
