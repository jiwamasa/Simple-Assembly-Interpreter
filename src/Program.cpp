// Definition of Program class
// Facilitates running the program after it's been parsed

#include "Program.h"
#include "MisException.h"
#include "ThreadInstr.h"

// Default constructor 
Program::Program() {}

// Add a new instruction to the end of the instruction list
bool Program::addInstruction(Instruction* ins) {
  ins_list.push_back(ins);
  return true;
}

// Add a new thread to the end of the thread list
bool Program::addThread(Instruction* thread) {
  thread_list.push_back(thread);
  return true;
}

// Block until all threads are done
void Program::barrier() {
  for (auto it = thread_list.begin(); it != thread_list.end(); it++)
    ((ThreadInstr*)(*it))->waitForRunToFinish();
}

// Add a new variable to the variable list
// If variable already exists, print error and return false
bool Program::addVariable(std::string name, Variable* var) {
  // Check if name already exists
  if (checkVariable(name)) {
    throw(new MisException(name + ": variable already exists\n"));
    return false;
  }
  var_list[name] = var; // Add variable to list
  return true;
}

// Retrieve a variable by its name
// If it doesn't exist, print error and return NULL
Variable* Program::getVariable(std::string name) {
  // Check if variable doesn't exist
  if (!(checkVariable(name))) {
    throw(new MisException(name + ": variable doesn't exist\n"));
    return nullptr;
  }
  // Otherwise, get variable
  auto search = var_list.find(name);
  return (*search).second;
}

// Return true if variable exists
bool Program::checkVariable(std::string name) {
  if (var_list.find(name) == var_list.end()) return false;
  return true;
}

// Add a constant to program's constant list
// Used to delete constants at the end
void Program::addConst(Variable* var) {
  const_list.push_back(var);
}

// Add a new label to the label list
// If label already exists, print error and return false
bool Program::addLabel(std::string name, Variable* var) {
  // Check if name already exists
  if (checkLabel(name)) { 
    throw(new MisException(name + ": label already exists\n"));
    return false; 
  }
  label_list[name] = var; // Add variable to list
  return true;
}

// Retrieve a label by its name
// If it doesn't exist, print error and return NULL
Variable* Program::getLabel(std::string name) {
  // Check if variable doesn't exist
  if (!(checkLabel(name))) { 
    throw(new MisException(name + ": label doesn't exist\n"));
    return nullptr;
  }
  // Otherwise, get variable
  auto search = label_list.find(name);
  return (*search).second;
}

// Return true if label exists
bool Program::checkLabel(std::string name) {
  if (label_list.find(name) == label_list.end()) return false;
  return true;
}

// Add a line of tokens to program's ins_token list
void Program::addInsTokens(std::vector<std::string> &tokens) {
  ins_tokens.push_back(tokens);
}

// Get a line of tokens from instruction token list
std::vector<std::string> Program::getInsTokens(size_t line) {
  return ins_tokens[line];
}

// Add a line of tokens to program's token list
void Program::addTokens(std::vector<std::string> &tokens) {
  token_list.push_back(tokens);
}

// Get a line of tokens from token list
std::vector<std::string> Program::getTokens(size_t line) {
  return token_list[line];
}


// Add strings to the out string buffer
void Program::addOutBuf(std::vector<Variable*>* str_args) {
  for (auto it = str_args->begin(); it != str_args->end(); it++)
    obuf.push_back((*it)->toString());
}

// Add strings to the err string buffer
void Program::addErrBuf(std::vector<Variable*>* str_args) {
  for (auto it = str_args->begin(); it != str_args->end(); it++)
    ebuf.push_back((*it)->toString());
}

// Run instructions in instruction list 
// Returns false if fails
bool Program::execute(int flag) {
  dump();
  pc = 0; // Set current instruction to first instruction
  size_t exc_count = 0; // Number of executions run
  int prog_len = ins_list.size(); // Get length of program
  bool success = true; // If false, program failed
  while(pc < prog_len) {
    try {
      if ((ins_tokens[pc][1] == "LOCK")|| // Check if lock/unlock
	  (ins_tokens[pc][1] == "UNLOCK"))
	throw(new MisException("Can't lock/unlock in main program\n"));
      success = ins_list[pc]->doOperation(); // Perform instruction
    }
    catch (std::vector<Variable*>* print_args) { // Catch print output
      addOutBuf(print_args); // Add to out buffer
    }
    catch (int interrupt) { // Catch interrupts for jumps/special commands
      switch(interrupt) {
       case -20: // THREAD_BEGIN
	runThread(); // Start thread
	break;
       case -40: // BARRIER: wait for all threads to finish
	barrier();
        break;
       default: // Otherwise, must be jump
	pc = interrupt;
      }
    }
    catch (MisException *exc) { // Catch errors
      exc->addLine(ins_tokens[pc]); // Add on line to error message
      throw(exc); // Throw again to main
    }

    if(!(success)) break; // Stop execution if instruction failed
    exc_count++; // Add to execution count
    pc++; // Increment to next instruction
    // If instruction was a jump, then pc is goes to the label location
    if((flag == 0)&&(exc_count > 1024)) { // If particularly long exc time
      std::string err = "Particularly long execution time: ";
      err.append("exiting program\n");
      err.append("Does your code have an infinite loop?\n");
      err.append("To ignore check, run with -f flag\n");
      throw(new MisException(err));
      success = false;
      break;
    }
  }
  barrier(); // Wait for all detached threads to finish
  return success;
}

// Start a new thread
bool Program::runThread() {
  int thread_start_line = pc; // Get line where thread begins
  // Make a new thread object
  ThreadInstr* thread = new ThreadInstr();
  addThread(thread); // Add thread to thread list
  thread->setStart(thread_start_line); // Set beginning of thread
  // Go through all instructions in thread
  while (ins_tokens[pc][1] != "THREAD_END") {
    pc++; // Increment PC
    if (pc >= ins_list.size()) { // If no THREAD_END, throw error
      pc = thread_start_line; // Reset pc to where thread began
      throw(new MisException("THREAD_BEGIN with no matching THREAD_END\n",
			     ins_tokens[pc]));
      return false;
    }
    if (ins_tokens[pc][1] == "THREAD_BEGIN") { // Cannot nest threads
      throw(new MisException("Cannot nest threads\n", ins_tokens[pc]));
      return false;
    }
    if (ins_tokens[pc][1] != "THREAD_END") // If not the END instruction
      thread->addIns(ins_list[pc]); // Add instruction to thread
    
  }

  bool t_success = thread->runThread(&obuf, &ebuf); // Begin thread
  if (!t_success) { // If thread failed, add line number of thread
    std::string err_msg = "  Error in thread started at line " +
      ins_tokens[thread_start_line][0];
    ebuf.push_back(err_msg);
    return false;
  }
  
  return true;
}

// Dump program info for debugging
void Program::dump() {
  std::cout << "--variables--\n";
  for(auto it = var_list.begin(); it != var_list.end(); it++) { 
    std::string name = (*it).first;
    Variable* tmp = (*it).second;
    std::cout << name << " "; // Variable name
    switch(tmp->getType()) {
     case NUMERIC: std::cout << tmp->getInt(); break;
     case REAL: std::cout << tmp->getDouble(); break;
     case CHAR: std::cout << tmp->getChar(); break;
     case STRING: std::cout << tmp->getString(); break;
    }
    std::cout << "\n";
  }
  std::cout << "--labels--\n";
  for(auto it = label_list.begin(); it != label_list.end(); it++) 
    std::cout<<(*it).first<<" "<<((*it).second)->getInt()<<"\n";
  std::cout << "--lines--\n";
  for(auto ita = token_list.begin(); ita != token_list.end(); ita++) {
    for(auto itb = (*ita).begin(); itb != (*ita).end(); itb++) {
      std::cout << *itb << " ";
    }
    std::cout << "\n";
  }
}

// Destructor cleans up all allocated memory
Program::~Program() {
  // Delete instructions
  for(auto it = ins_list.begin(); it != ins_list.end(); it++) {
    delete(*it);
  }
  // Delete variables
  for(auto it = var_list.begin(); it != var_list.end(); it++) {
    delete((*it).second);
  }
  // Delete labels
  for(auto it = label_list.begin(); it != label_list.end(); it++) {
    delete((*it).second);
  }
  // Delete constants
  for(auto it = const_list.begin(); it != const_list.end(); it++) {
    delete(*it);
  }
}

