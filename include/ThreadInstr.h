// Class declaration for Thread within a mis program
// Contains a list of instructions to run
// Executes them sequentially in a detached thread
//
// Uses Thread Library provided by professor Sobh

#ifndef THREADINSTR_H
#define THREADINSTR_H

#include "common.h"
#include "Instruction.h"
#include "Thread.h"

class ThreadInstr : public Instruction, public Thread {
 private:
  // List of instructions needed to passed through to a single thread
  std::vector<Instruction*> instr_list;
  size_t thread_start; // Line number of the beginning of the thread
  size_t thread_pc; // Program counter for thread
  bool thread_success; // Whether thread succeeded
  std::vector<std::string>* obuf; // Output buffer
  std::vector<std::string>* ebuf; // Error buffer
  
 public:
  ThreadInstr();

  // Add new instruction to the instruction list 
  // When evoked from parent Instruction, typecast to access function
  bool addIns(Instruction* instr);
  
  void setStart(int line); // Set starting point of thread
  int getPC(); // Get thread's pc
  
  // Calls doOperation on the list of individual instructions
  // Takes in output buffer
  bool runThread(std::vector<std::string>* _obuf,
		 std::vector<std::string>* _ebuf);

  // Thread main body called when thread is run
  virtual void* threadMainBody(void* arg = nullptr);

  // Empty instr_list
  void cleanup();

  ~ThreadInstr();
};

#endif
