// Declaration of Program class
// Contains the parsed instructions and global variables
//
// Instructions are stored sequentially in an expandable vector
//
// Variables are stored in a map from strings to Variables
// The Variable list also include jump labels
//
// Constants are stored as Variables in a vector
//

#ifndef __PROGRAM__
#define __PROGRAM__

#include "common.h"
#include "Instruction.h"
#include "Variable.h"

class Program {
 private:
  std::vector<Instruction*> ins_list; // Instructions
  std::vector<Instruction*> thread_list; // List of detached threads
  std::unordered_map<std::string, Variable*> label_list; // Labels
  std::unordered_map<std::string, Variable*> var_list; // Variables
  std::vector<Variable*> const_list; // Holds constants for deletion
  // Contains tokenized instruction lines (no variable/labels)
  // Used primarily for runtime error tracing
  std::vector<std::vector<std::string> > ins_tokens;
  size_t pc; // Program counter; keeps track of current instruction
  
 public:
  // Contains tokenized file
  std::vector<std::vector<std::string> > token_list; 
  std::vector<std::string> obuf; // Normal output buffer
  std::vector<std::string> ebuf; // Error output buffer
  
  // Default constructor 
  Program();
  bool addInstruction(Instruction* ins); // Add new instruction
  bool addThread(Instruction* thread);     // Add new thread

  bool runThread(); // Start a new thread
  void barrier(); // Block until all threads are done
  
  bool addVariable(std::string name, Variable* var); // Add variable
  Variable* getVariable(std::string name); // Return variable by name
  bool checkVariable(std::string name); // Check if a variable exists
  void addConst(Variable* var); // Add const (which is implemented as var)
  
  bool addLabel(std::string name, Variable* var); // Add label
  Variable* getLabel(std::string name); // Return label by name
  bool checkLabel(std::string name); // Check if a label exists

  // Insert line of tokens to program's internal instruction token buffer
  void addInsTokens(std::vector<std::string> &tokens);
  // Get a line from program's internal instruction buffer
  std::vector<std::string> getInsTokens(size_t line);
  // Insert line of tokens to program's token list
  void addTokens(std::vector<std::string> &tokens);
  // Get a line from program's token list  
  std::vector<std::string> getTokens(size_t line);

  // Add strings to the out string buffer
  void addOutBuf(std::vector<Variable*>* str_args);
  // Add strings to the err string buffer
  void addErrBuf(std::vector<Variable*>* str_args);
  bool execute(int flag); // Run program; takes in option flag
  void dump(); // Dump program
  ~Program(); // Destructor deletes all instructions and variables as well
};

#endif
