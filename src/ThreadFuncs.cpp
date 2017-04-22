// File containing thread OpFunc functions
// Linked to instructions to define functionality
// Implemented thread_begin, thread_end, lock, unlock, barrier
// All functions return false if they fail

#include "OpFuncs.h"
#include "MisException.h"

// Throw exception to start thread program separation
bool opThrBegin(std::vector<Variable*>* args) {
  if(!args->empty()) {
    throw(new MisException("THREAD_BEGIN: should not have an argument\n"));
    return false;
  }
  throw(-20); // Throw interrupt to signal program to break off thread
  return true;
}

// Throw exception to end thread program separation
bool opThrEnd(std::vector<Variable*>* args) {
  if(!args->empty()) {
    throw(new MisException("THREAD_END: should not have an argument\n"));
    return false;
  }
  return true;
}

// Locks args[0] for calling thread
bool opLock(std::vector<Variable*>* args) {
  if(args->empty() || (args->size() > 1)) {
    throw(new MisException("LOCK: invalid number of arguments\n"));
    return false;
  }
  pthread_t curr = pthread_self();
  (*args)[0]->lock(curr);
  return true;
}

// Unlocks args[0]
bool opUnlock(std::vector<Variable*>* args) {
  if(args->empty() || (args->size() > 1)) {
    throw(new MisException("LOCK: invalid number of arguments\n"));
    return false;
  }
  if(!(*args)[0]->checkLock()) {
    throw(new MisException("UNLOCK: variable is not locked\n"));
    return false;
  }
  (*args)[0]->unlock();
  return true;
}

// Wait for all threads to finish
bool opBarrier(std::vector<Variable*>* args) {
  if(!args->empty()) {
    throw(new MisException("BARRIER: should not have an argument\n"));
    return true;
  }
  throw(-40); // Throw interrupt to wait for run to finish
  return false;
}
