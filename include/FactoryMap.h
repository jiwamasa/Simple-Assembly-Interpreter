// Declaration of container class for middleware factory maps
// Handles creation and deletion of maps

#ifndef __FACTORY_MAP__
#define __FACTORY_MAP__

#include "OpFuncs.h"
#include "Instruction.h"

class FactoryMap {
 public:
  // Map for instructions; maps opcode to function pointer
  std::unordered_map<std::string, OpFunc> ins_map {
    /* Math instructions (found in MathFuncs.cpp) */
    {"ADD",          opAdd},
    {"SUB",          opSub},
    {"MUL",          opMul},
    {"DIV",          opDiv},
    /* String instructions (found in StringFuncs.cpp) */
    {"SET_STR_CHAR", opSet},
    {"GET_STR_CHAR", opGet},
    /* Address instructions (found in AddressFuncs.cpp) */
    {"JMP",          opJump},
    {"JMPZ",         opJumpZ},
    {"JMPNZ",        opJumpNZ},
    {"JMPGT",        opJumpGT},
    {"JMPLT",        opJumpLT},
    {"JMPGTE",       opJumpGTE},
    {"JMPLTE",       opJumpLTE},
    /* Misc instructions (found in MiscFuncs.cpp) */
    {"ASSIGN",       opAssign},
    {"OUT",          opOut},
    {"SLEEP",        opSleep},
    /* Thread instructions (found in ThreadFuncs.cpp) */
    {"THREAD_BEGIN", opThrBegin},
    {"THREAD_END",   opThrEnd},
    {"LOCK",         opLock},
    {"UNLOCK",       opUnlock},
    {"BARRIER",      opBarrier}
  };
  // Map for variables; maps typename to variable object
  std::unordered_map<std::string, Variable*> var_map {
    {"NUMERIC", new VarInt()},
    {"REAL",    new VarDouble()},
    {"CHAR",    new VarChar()},
    {"STRING",  new VarString()}
  };
  FactoryMap(); // Default constructor
  ~FactoryMap(); // Destructor handles map deletion
};

#endif
