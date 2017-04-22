// File containing miscellaneous OpFunc functions
// Linked to instructions to define functionality
// Implemented assign, out, sleep
// All functions return false if they fail

#include "OpFuncs.h"
#include "MisException.h"
#include "so_includes.h"

// Assign arguments[1] to arguments[0]
bool opAssign(std::vector<Variable*>* args) {
  if (args->size() != 2) // Check args num
    throw (new MisException("ASSIGN: invalid arg num\n"));
  // Check that types are the same
  if ((*args)[0]->getType() != (*args)[1]->getType()) {
    throw(new MisException(std::string("attempting to assign variables ") + 
			   std::string("of different types\n")));
    return false;
  }
  switch((*args)[0]->getType()) {
    case NUMERIC: (*args)[0]->set((*args)[1]->getInt()); break;
    case REAL: (*args)[0]->set((*args)[1]->getDouble()); break;
    case CHAR: (*args)[0]->set((*args)[1]->getChar()); break;
    case STRING: (*args)[0]->set((*args)[1]->getString()); break;
  }
  return true;
}

// Print out all arguments (up to 12)
bool opOut(std::vector<Variable*>* args) {
  if ((args->empty())||(args->size() > 12)) // Check args num
    throw (new MisException("OUT: invalid arg num\n"));
  throw(args); // Throw interrupt to signify print
  return true;
}

// Pauses program execution for arguments[0] seconds
// Assume either NUMERIC or REAL
bool opSleep(std::vector<Variable*>* args) {
  if (args->size() != 1) // Check args num
    throw (new MisException("SLEEP: invalid arg num\n"));
  int sleep_time = (*args)[0]->getInt();
  sleep(sleep_time);
  return true;
}
