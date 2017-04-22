// Multi pass parser for MIS language
// To prepare, we tokenize the text file and store it into a 2D vector 
// Then the First pass allocates all the variables and sets the labels
//   We also remove all Variable and Label lines in the First Pass
// Finally, the Second pass creates the instruction list

#include "Parse.h"
#include "MisException.h"
#include "FactoryMap.h"

// Parse the program from a text file
// Store the variables, labels, and instructions in Program object
void Parse::parseProgram(Program* new_program) {
  std::vector<std::vector<std::string> >* token_file; // Tokenized file
  token_file = &(new_program->token_list); // Get tokens from program
  FactoryMap* fmap = new FactoryMap(); // Initialize middleware factory map
  size_t linenum = 0; // Keeps track of current line

  std::cout << "First pass\n";
  // First pass: read in variables and labels
  bool v_state = true; // Start in variable parsing state
  while(linenum < token_file->size()) { // Go through each line
    std::vector<std::string> tokens = (*token_file)[linenum]; // Current line
    // Go into label parsing state if no more VAR instructions
    if(tokens[1] != "VAR") v_state = false;
    
    try { // Check for variable/label parsing errors
    if(v_state == true) { // Variable parsing state
      std::string var_name; // Variable identifier
      Variable* new_var = parseVariable(tokens, var_name, fmap->var_map); 
      new_program->addVariable(var_name, new_var); // Add variable to program
      token_file->erase(token_file->begin()); // Remove line from token list
    
    } else { // Label parsing state
      if(tokens[1] == "LABEL") { // Check if label instruction
	std::string label_name = tokens[2]; // Name of the label
	if(tokens.size() != 3) // Ensure proper syntax
	  throw(new MisException("LABEL syntax incorrect!\n"));
	// Set label location to 1 before (since pc increments every cycle)
	Variable* new_label = new VarInt(linenum - 1);
	new_program->addLabel(label_name, new_label); // Add label
	token_file->erase(token_file->begin()+linenum); // Remove line 
      } else { ++linenum; } // Skip non-label instructions
    }
    } catch (MisException *exc) { // Catch error 
      exc->addLine(tokens); // Add line to error message
      throw(exc); // Throw again to main
    }
  }
  
  // Dump instructions into program field ins_tokens (for error trace)
  for(auto it = token_file->begin(); it != token_file->end(); it++) 
    new_program->addInsTokens(*it);

  std::cout << "Second pass\n";
  // Second Pass: read in instructions
  for(linenum = 0; linenum < token_file->size(); ++linenum) {
    Instruction* new_instr =
      parseInstruction(new_program, (*token_file)[linenum], fmap->ins_map);
    new_program->addInstruction(new_instr);
  }
  delete(fmap); // Clean up middleware maps
}

// Returns a pointer to an appropriate derived Variable class.
// Name of the associated variable can be found in var_name.
Variable* Parse::parseVariable(const std::vector<std::string> &tokens,
			       std::string &var_name,
			       std::unordered_map<std::string,
			                          Variable*> &var_map) {
  // Ensure proper variable declaration syntax
  // Takes into accout special case of string decl with size (5 args)
  if((tokens.size() < 5) || (tokens.size() > 6) || 
     ((tokens[2]).at(0) != '$')) {
    throw(new MisException("Bad variable declaration: \n"));
    return nullptr;
  }
  var_name = tokens[2]; // Get variable name
  std::string type = tokens[3]; // Type of the variable
  std::string value = tokens[4]; // Get variable argument

  // Attempt to find variable type in middleware map
  if(var_map.find(type) == var_map.end()) // Check if valid type
    throw(new MisException(type + " is not a valid type\n"));
  Variable* new_var = (var_map[type])->clone();

  // Special case of string decl w/ size
  if((tokens.size() == 6)&&(type == "STRING")) {
    // Reallocate new_var as string with appropiate fields
    delete(new_var); 
    try { new_var = new VarString(trimQuotes(tokens[5], '\"'), 
				  std::stoi(value)); }
    catch (std::invalid_argument ia) { // Catch error if improper size
      throw(new MisException("not a valid string declaration\n"));
    }
    
  } else { // Otherwise, set the value of the variable normally
    // Trim quotes as necessary
    if(type == "CHAR") value = trimQuotes(value, '\'');
    if(type == "STRING") value = trimQuotes(value, '\"');
    new_var->set(value); 
  } 
  return new_var;
}

// Returns a pointer to a Instruction class, generated by with 
// opcode string and Variable* argument vector.
Instruction* Parse::parseInstruction(Program* new_program, 
				     std::vector<std::string> &tokens,
				     std::unordered_map<std::string,
				                        OpFunc> &ins_map) {
  // Get the function that corresponds to the opcode
  auto search = ins_map.find(tokens[1]);
  if(search == ins_map.end()) // If instruction was not found...
    throw(new MisException(tokens[1] + " is not a valid instruction\n",
			   tokens)); // Throw error

  Instruction* new_instr = new Instruction((*search).second); // Set ins
  try { // Catch errors
  for(unsigned int i = 2; i < tokens.size(); i++) { // Go through arguments
    if(tokens[i].at(0) == '$') { // If argument is a variable...
      new_instr->addArgument(new_program->getVariable(tokens[i])); // Add var
    
    } else if(checkAllCaps(tokens[i])) { // If argument is a label...
      if(tokens[1].substr(0,3) != "JMP") // If instruction isn't a jump...
	throw(new MisException("can only use labels in jumps\n"));
      new_instr->addArgument(new_program->getLabel(tokens[i])); // Add label

    } else { // If argument is constant/error...
      // Check if trying to put a constant as jump label
      if((i == 2)&&(tokens[1].substr(0,3) == "JMP")) 
	throw(new MisException("can't use constant as label\n"));
      Variable* new_const = checkConstType(tokens[i]); // Parse as constant 
      new_instr->addArgument(new_const); // Add constant to instruction
      new_program->addConst(new_const); // Add constant to program
    }
  }
  } catch (MisException *exc) {
    exc->addLine(tokens); // Add line to error
    throw(exc); // Throw to main
  }
  return new_instr;
}

// Check if input string is all capital letters (label)
bool Parse::checkAllCaps(const std::string &word) {
  for(auto it = word.begin(); it != word.end(); ++it) 
    // Check if letter lies outside the ASCII range of 'A' and 'Z'
    if((*it < 65) || (*it > 90)) return false;
  return true;
}

// Parses and returns appropiate constant type
Variable* Parse::checkConstType(const std::string &word) {
  std::regex rgx_int("^\\d+$");
  std::regex rgx_double("^(\\d+\\.\\d*|\\d*\\.\\d+)$");
  std::regex rgx_char("^\'.\'$");
  std::regex rgx_string("^\".*\"$");

  std::string match_word = ltrim(rtrim(word));
  Variable* new_const;

  try { // Check for int/double overflow
  if(regex_match(match_word, rgx_int)) {
    new_const = new VarInt(std::stoll(word));
  } else if(regex_match(match_word, rgx_double)) {
    new_const = new VarDouble(std::stod(word));
  } else if(regex_match(match_word, rgx_char)) {
    new_const = new VarChar(trimQuotes(word, '\'').at(0));
  } else if(regex_match(match_word, rgx_string)) {
    new_const = new VarString(trimQuotes(word, '\"'));
  } else { // Constant does not match any defined type
    throw(new MisException(word + " is invalid argument\n"));  
  }
  } catch (std::out_of_range err) { // Throw overflow error
    throw(new MisException(word + " it too large to fit in 8 byte number\n"));
  }
  // Make Variable* hold a "constant" value
  new_const->makeConst();
  return new_const;
}

// Remove leading whitespace
// If line is empty, index falls off end of string, so return empty string
std::string Parse::ltrim(const std::string &line) {
  size_t index = line.find_first_not_of(" \t");
  return index != std::string::npos ? line.substr(index) : "";
}
// Remove trailing whitespace
std::string Parse::rtrim(const std::string &line) {
  size_t index = line.find_last_not_of(" \t\n\r");
  return line.substr(0, index+1);
}

// Remove first and last quotation character (either " or ')
std::string Parse::trimQuotes(const std::string &word, const char quote) {
  size_t first = 0;
  size_t last = word.length()-1;
  if((word.at(first) == quote) && (word.at(last) == quote)) {
    ++first;
    --last;
    return word.substr(first, last);
  } // Otherwise, throw error
  throw(new MisException("invalid quotes\n"));
}

// Parse an instruction line into tokens with the opcode and arguments
void Parse::tokenize(std::vector<std::string> &tokens,
		     const std::string &line,
	      size_t linenum) {
  std::istringstream iss(ltrim(rtrim(line))); // Get rid of extra whitespace
  std::string new_token = "";
  // Insert line number as first argument
  tokens.push_back(std::to_string(linenum)); 

  std::getline(iss, new_token, ' '); // Get the opcode
  tokens.push_back(ltrim(rtrim(new_token)));

  // Get remaining tokens as arguments
  while(std::getline(iss, new_token, ',')) {
    std::string arg_token = ltrim(new_token);
    // Check for string literals with ',' in them
    if((arg_token.at(0) == '\"') && 
       (arg_token.at(arg_token.size()-1) != '\"')) {
      std::getline(iss, new_token, '\"'); // Get rest of string
      arg_token += "," + new_token; // Append rest of string 
      if(iss.eof()) { // If the string is unterminated, throw error
        tokens.push_back(rtrim(arg_token)); // Push on bad token
        throw(new MisException("Unterminated string: " + 
                               arg_token + "\n", tokens));
      } else { arg_token += "\""; } // Otherwise, add on terminating quote
    }
    tokens.push_back(rtrim(arg_token)); // Add token        
  }
}
