// Container class declaration with functions related to parsing
// parseProgram() will fill a program object with neccessary instructions and
//   variables.

#ifndef PARSE_H
#define PARSE_H

#include "common.h"
#include "Program.h"
#include "Instruction.h"
#include "ThreadInstr.h"
#include "Variable.h"

class Parse {
 public:
  // Parses entire program
  // Program should have tokenized lines inside of it
  static void parseProgram(Program* new_program);

  // Parse a variable
  static Variable* parseVariable(const std::vector<std::string> &tokens,
                                 std::string &var_name,
                                 std::unordered_map<std::string, 
                                                    Variable*> &var_map);
  // Parse an instruction
  static Instruction* parseInstruction(Program* new_program, 
                                       std::vector<std::string> &tokens,
                                       std::unordered_map<std::string, 
                                                          OpFunc> &ins_map);
  // Determine label 
  static bool checkAllCaps(const std::string &word);
  static bool checkAllCaps(const char* word);
  
  // Type syntax checking
  static bool checkInt(const std::string &word);
  static bool checkDouble(const std::string &word);
  static bool checkChar(const std::string &word);
  static bool checkString(const std::string &word);
  
  // Return proper Variable type of constant
  static Variable* checkConstType(const std::string &word);
  
  // Leading and trailing whitespace removal
  static std::string ltrim(const std::string &line);
  static std::string rtrim(const std::string &line);
  // Remove first and last quotes
  static std::string trimQuotes(const std::string &word, const char quote);
  
  // Takes a line and parses the first word by a space delimiter
  // and the remaining words by a comma delimiter.
  // Puts the original linenumber as the first element
  static void tokenize(std::vector<std::string> &tokens, 
                       const std::string &line, size_t linenum);
  
  // Takes a file and tokenizes each line (depricated)
  static void tokenizeFile(std::vector<std::vector<std::string> > 
                           &token_file, const std::string &file, int flag);
};

#endif
