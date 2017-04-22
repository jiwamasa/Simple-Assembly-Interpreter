// File containing Math related OpFunc functions
// Linked to instructions to define functionality
// Implemented add, mul, sub, div
// All in the form of: OPCODE dest, src, src, [...]
// All arguments must be either NUMERIC or REAL
// Note that we do all operations with doubles since results are implicitely 
//   converted back to their appropiate type on setting
// All functions return false if they fail

#include "OpFuncs.h"
#include "MisException.h"

// Add arguments[1-12] and store in arguments[0]
bool opAdd(std::vector<Variable*>* args) {
  if ((args->size() < 3)||(args->size() > 12)) // Check number of args
    throw (new MisException("ADD: invalid number of arguments\n"));
  double sum = 0; 
  for (auto iter = args->begin()+1; iter != args->end(); iter++)
    sum += (*iter)->getDouble();
  (*args)[0]->set(sum); 
  return true;
}

// Subtract arguments[2] from arguments[1]; store into arguments[0]
bool opSub(std::vector<Variable*>* args) {
  if (args->size() != 3) // Check number of args
    throw (new MisException("SUB: invalid number of arguments\n"));
  (*args)[0]->set((*args)[1]->getDouble() - (*args)[2]->getDouble()); 
  return true;
}

// Multiply arguments[1-12] and store in arguments[0]
bool opMul(std::vector<Variable*>* args) {
  if ((args->size() < 3)||(args->size() > 12)) // Check number of args
    throw (new MisException("MUL: invalid number of arguments\n"));
  double product = 1; 
  for (auto iter = args->begin()+1; iter != args->end(); iter++) {
    product *= (*iter)->getDouble();
  }
  (*args)[0]->set(product); 
  return true;
}

// Divide arguments[1] by arguments[2]; store into arguments[0]
bool opDiv(std::vector<Variable*>* args) {
  if (args->size() != 3) // Check number of args
    throw (new MisException("DIV: invalid number of arguments\n"));
  if ((*args)[2]->getDouble() == 0) { // Check for divide by 0
    throw (new MisException("divide by 0\n"));
    return false;
  }
  (*args)[0]->set((*args)[1]->getDouble() / (*args)[2]->getDouble()); 
  return true;
}
