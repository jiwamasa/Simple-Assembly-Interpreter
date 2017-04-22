// File containing String related OpFunc functions
// Linked to instructions to define functionality
// Implemented get_str_char, set_str_char
// All in the form of: OPCODE string, index, character
// All functions return false if they fail

#include "OpFuncs.h"
#include "MisException.h"

// Set character at arguments[1] in arguments[0] to arguments[3]
bool opSet(std::vector<Variable*>* args) {
  if (args->size() != 3) // Check args size
    throw(new MisException("SET_STR_CHAR: invalid argument number\n"));
  // Load values into tmp variables
  const char* strtmp = (*args)[0]->getString();
  int index = (*args)[1]->getInt();
  char chr = (*args)[2]->getChar();
  size_t strtmp_len = strlen(strtmp);

  // Detect index out of bounds/invalid index
  if ((index < 0)||(index >= (int)strtmp_len)) {
    throw(new MisException("SET: indexing outside of string: " + 
			   std::string(strtmp) + "\n"));
    return false;
  } 
  char* tmp = (char*)calloc(strtmp_len+1, sizeof(char));
  strcpy(tmp, strtmp); // Copy string
  tmp[index] = chr;    // Set char in tmp string
  (*args)[0]->set(tmp);       // Set new string
  free(tmp);           // free tmp sring
  return true; 
}

// Get character at arguments[1] in arguments[0] to arguments[3]
bool opGet(std::vector<Variable*>* args) {
  if (args->size() != 3) // Check args size
    throw(new MisException("GET_STR_CHAR: invalid argument number\n"));
  // Load values into tmp variables
  const char* strtmp = (*args)[0]->getString();
  int index = (*args)[1]->getInt();
  size_t strtmp_len = strlen(strtmp);

  // Detect index out of bounds/invalid index
  if ((index < 0)||(index >= (int)strtmp_len)) {
    throw(new MisException("GET: indexing outside of string: " + 
			   std::string(strtmp) + "\n"));
    return false;
  }
  (*args)[2]->set(strtmp[index]); // Set character
  return true;
}
