// Header file containing declarations of OpFunc functions
// These functions are passed to instruction objects as pointers to define 
//   their functionality. 
// Extension to instruction set can be achieved by adding declaration here, 
//   defining it in proper implementation file (MathFuncs.cpp, 
//   StringFuncs.cpp, etc), and putting it in the middleware map in 
//   FactoryMap.cpp. 

#ifndef __OP_FUNCS__
#define __OP_FUNCS__

#include "Instruction.h"

/* Math functions (found in MathFuncs.cpp) */

// Add/Mul args[1-12] store in args[0]
// Supports both NUMERIC and REAL types
bool opAdd(std::vector<Variable*>* args);
bool opMul(std::vector<Variable*>* args);
// Sub/Div args[1] by args[2] store in args[0]
// Supports both NUMERIC and REAL types
bool opSub(std::vector<Variable*>* args);
bool opDiv(std::vector<Variable*>* args);

/* String functions (found in StringFuncs.cpp) */

// Set the character at index args[1] in string args[0] to char args[2] 
bool opSet(std::vector<Variable*>* args);
// Store the character at index args[1] in string args[0] to char args[2]
bool opGet(std::vector<Variable*>* args);

/* Address functions (found in AddressFuncs.cpp) */

// All Jump to label specified in args[0]
// Unconditional jump
bool opJump(std::vector<Variable*>* args);
// Jump when args[1] == 0
bool opJumpZ(std::vector<Variable*>* args);
// Jump when args[1] != 0
bool opJumpNZ(std::vector<Variable*>* args);
// Jump when args[1] > args[2]
bool opJumpGT(std::vector<Variable*>* args);
// Jump when args[1] < args[2]
bool opJumpLT(std::vector<Variable*>* args);
// Jump when args[1] >= args[2]
bool opJumpGTE(std::vector<Variable*>* args);
// Jump when args[1] <= args[2]
bool opJumpLTE(std::vector<Variable*>* args);

/* Miscellaneous functions (found in MiscFuncs.cpp) */

// Assign args[0] to args[1] if they are of same type
bool opAssign(std::vector<Variable*>* args);
// Print out args[0-12]
bool opOut(std::vector<Variable*>* args);
// Pause current thread for args[0] seconds
bool opSleep(std::vector<Variable*>* args);

/* Thread functions (found in ThreadFuncs.cpp) */
// Throw exception to start thread program separation
bool opThrBegin(std::vector<Variable*>* args);
// Throw exception to end thread program separation
bool opThrEnd(std::vector<Variable*>* args);
// Take args[1] and assign lock flag
bool opLock(std::vector<Variable*>* args);
// Take args[1] and remove lock flag
bool opUnlock(std::vector<Variable*>* args);
// Wait for all threads to finish
bool opBarrier(std::vector<Variable*>* args);

#endif

