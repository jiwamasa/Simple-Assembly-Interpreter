// File containing address related OpFunc functions
// Linked to instructions to define functionality
// Implemented jmp, jmpz, jmpnz, jmpgt, jmplt, jmpgte, jmplte
// All in form of: OPCODE jump_location, [arg, arg]
// Throws new pc to main program to switch control
// All functions return false if they fail

#include "OpFuncs.h"
#include "MisException.h"

// Conditionless jump to arguments[0]
bool opJump(std::vector<Variable*>* args) {
  if (args->size() != 1) // Check args size
    throw(new MisException("JMP: invalid argument number\n"));
  int new_pc = (*args)[0]->getInt();
  throw(new_pc);
  return true;
}

// Jump if arguments[1] == 0
bool opJumpZ(std::vector<Variable*>* args) {
  if (args->size() != 2) // Check args size
    throw(new MisException("JMPZ: invalid argument number\n"));
  if((*args)[1]->getDouble() == 0) {
    int new_pc = (*args)[0]->getInt();
    throw(new_pc);
  }
  return true;
}

// Jump if arguments[1] != 0
bool opJumpNZ(std::vector<Variable*>* args) {
  if (args->size() != 2) // Check args size
    throw(new MisException("JMPNZ: invalid argument number\n"));
  if((*args)[1]->getDouble() != 0) {
    int new_pc = (*args)[0]->getInt();
    throw(new_pc);
  }
  return true;
}

// Jump if arguments[1] > arguments[2]
bool opJumpGT(std::vector<Variable*>* args) {
  if (args->size() != 3) // Check args size
    throw(new MisException("JMPGT: invalid argument number\n"));
  if((*args)[1]->getDouble() > (*args)[2]->getDouble()) {
    std::cout << "GT!!!\n";
    int new_pc = (*args)[0]->getInt();
    throw(new_pc);
  }
  return true;
}

// Jump if arguments[1] < arguments[2]
bool opJumpLT(std::vector<Variable*>* args) {
  if (args->size() != 3) // Check args size
    throw(new MisException("JMPLT: invalid argument number\n"));
  if((*args)[1]->getDouble() < (*args)[2]->getDouble()) {
    int new_pc = (*args)[0]->getInt();
    throw(new_pc);
  }
  return true;
}

// Jump if arguments[1] >= arguments[2]
bool opJumpGTE(std::vector<Variable*>* args) {
  if (args->size() != 3) // Check args size
      throw(new MisException("JMPGTE: invalid argument number\n"));
  if((*args)[1]->getDouble() >= (*args)[2]->getDouble()) {
    int new_pc = (*args)[0]->getInt();
    throw(new_pc);
  }
  return true;
}

// Jump if arguments[1] <= arguments[2]
bool opJumpLTE(std::vector<Variable*>* args) {
  if (args->size() != 3) // Check args size
    throw(new MisException("JMPLTE: invalid argument number\n"));
  if((*args)[1]->getDouble() <= (*args)[2]->getDouble()) {
    int new_pc = (*args)[0]->getInt();
    throw(new_pc);
  }
  return true;
}
