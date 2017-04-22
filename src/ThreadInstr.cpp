#include "ThreadInstr.h"
#include "MisException.h"

extern int pc; 

ThreadInstr::ThreadInstr() : Instruction(), Thread(&run) {}

// Add new instruction to the new instruction list at certain line
bool ThreadInstr::addIns(Instruction* instr) {
  instr_list.push_back(instr);
  return true;
}

// Set starting line of thread in relation to main program
void ThreadInstr::setStart(int line) { thread_start = line; }
// Get thread's PC
int ThreadInstr::getPC() { return thread_pc; }

// Starts the thread 
bool ThreadInstr::runThread(std::vector<std::string>* _obuf,
			    std::vector<std::string>* _ebuf) {
  obuf = _obuf; // Set output buffer
  ebuf = _ebuf; // Set error buffer
  thread_success = true;
  start(); // Start thread
  return thread_success; // Return if thread succeeded
}

// Thread main body called when thread is run
void* ThreadInstr::threadMainBody(void* arg) {
  size_t exec_count = 0; // Number of executions
  MisException* err = nullptr; // Placeholder for thread errors
  // Iterate through all instructions
  for(thread_pc = 0; thread_pc < instr_list.size(); ++thread_pc) {
    // Call each Instruction doOperation
    try { thread_success = instr_list[thread_pc]->doOperation(); }
    // Catch interrupts; for detached threads, should only be jumps
    catch (int jump) { 
      // If -20, then trying to make thread within thread, so error
      if (jump == -20)
	err = new MisException("Can't make thread within thread\n");
      // If -40, then trying to barrier within thread, so error
      if (jump == -40)
	err = new MisException("Can't BARRIER from within thread\n");
      // If trying to jump outside of thread, error
      if ((jump < thread_start)||(jump > thread_start+instr_list.size()))
	err = new MisException("Invalid jump outside of thread\n");

      // Otherwise, set new pc for jump
      // Scaled down since thread's instruction list starts at 0
      thread_pc = jump - thread_start - 1;
    }
    catch (std::vector<Variable*>* print_args) { // Catch print output
      for (auto it = print_args->begin(); it != print_args->end(); it++)
	(*obuf).push_back((*it)->toString());
    }
    if (err != nullptr) { // If there was an error
      err->printToBuf(*ebuf);
      thread_success = false;
    }
    if (!(thread_success)) break; // Break if failed
    exec_count++; // Increment execution count
    if (exec_count > 1024*128) { // If long thread execution
      std::string err = "Particularly long thread execution:";
      err.append(" exiting program\n");
      throw(new MisException(err));
      thread_success = false;
      break;
    }
  }
  return nullptr;
}

// Return ThreadInstr to its initial empty state
void ThreadInstr::cleanup() {
  for(size_t i = 0; i < instr_list.size(); ++i) {
    delete(instr_list[i]);
  }
  instr_list.clear();
}

// Delete all sub instrs, vars are deleted in higher level
ThreadInstr::~ThreadInstr() {
  cleanup();
}
