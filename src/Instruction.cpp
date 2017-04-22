// Implementation of Instruction interface

#include "Instruction.h"

// Defalt empty constructor
Instruction::Instruction() {}

// Default Constructor sets the functionality of instruction
Instruction::Instruction(OpFunc f):operation(f) {}

// Perform operation; returns false if fails
// For some technically complex instructions, we perform interrupts:
//   10: OUT
bool Instruction::doOperation() { return operation(&arguments); }

// Set operations
void Instruction::setOperation(OpFunc f) { operation = f; };

// Add an argument to arguments
void Instruction::addArgument(Variable* arg) { arguments.push_back(arg); }

// Return arguments as a vector of strings
std::vector<std::string> Instruction::getStringArgs() {
  std::vector<std::string> string_args;
  for (auto it = arguments.begin(); it != arguments.end(); it++)
    string_args.push_back((*it)->toString());
  return string_args;
}

// Print all arguments for debugging and calls to OUT
// Acts kind of like a system call: program is interrupted to run
void Instruction::print_args() {
  for (auto iter = arguments.begin(); iter != arguments.end(); iter++) 
    (*iter)->print();
}

// Destructor
Instruction::~Instruction(){
  // Argument deletion is done in Program
  // Program deletes all variables at once when program finishes
}
