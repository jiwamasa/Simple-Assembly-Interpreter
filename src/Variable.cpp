// Definition of Variable class

#include "Variable.h"
#include "MisException.h"


/***** Variable base class *****/
Variable::Variable() {
  pthread_mutex_init(&mutex,NULL);
  pthread_cond_init(&cond,NULL);
}
Variable::Variable(VarType type) : m_type(type) {
  pthread_mutex_init(&mutex,NULL);
  pthread_cond_init(&cond,NULL);
}

VarType Variable::getType() { return m_type; }
// Selectors (should not be called, return value is useless)
// these are called if trying to get from wrong type, so throw an error
long long int Variable::getInt() {
  throw(new MisException("not an int\n"));
  return 0;
}
double Variable::getDouble() {
  throw(new MisException("not a double\n"));
  return 0;
}
char Variable::getChar() {
  throw(new MisException("not a char\n"));
  return '\0';
}
char* Variable::getString() {
  throw(new MisException("not a string\n"));
  return nullptr;
}

// Identify if the calling variable is a constant
bool Variable::checkConst() { return is_const; }
// Identify if the calling variable is locked
bool Variable::checkLock() {
  return (owner != nullptr);
}

// Modifiers (should not be called)
// these are called if trying to set with wrong type
void Variable::set(long long int value) { 
  throw(new MisException("trying to set " + std::to_string(value) +
			 " which is not an int\n"));
}
void Variable::set(double value) { 
  throw(new MisException("trying to set " + std::to_string(value) +
			 " which is not a double\n"));
}
void Variable::set(char value) { 
  throw(new MisException("trying to set " + std::to_string(value) +
			 " which is not a char\n"));
}
void Variable::set(char* value) { 
  throw(new MisException("trying to set " + std::string(value) +
			 " which is not a string\n"));
}
void Variable::set(std::string value) {
  throw(new MisException("trying to set " + value +
			 " which is not a string\n"));
}

// Identify the calling variable as a constant
void Variable::makeConst() { is_const = true; }

// Lock by setting ownership to a thread
void Variable::lock(pthread_t new_owner) {
  std::cout << "locking...\n";
  if (owner == new_owner) // If owner is trying to double lock, throw error
    throw(new MisException("Double locking is prohibited\n"));
  pthread_mutex_lock(&mutex);
  if ((owner != nullptr)&&(owner != new_owner)) // Block other threads
    pthread_cond_wait(&cond, &mutex); // Only one thread can lock at a time
  owner = new_owner;
  pthread_mutex_unlock(&mutex);
}
// Remove lock 
void Variable::unlock() {
  owner = nullptr; // Remove ownership of variable
  pthread_cond_signal(&cond); // Signal other threads to go
}

bool Variable::checkInt(const std::string &word) {
  std::regex rgx("^\\d+$");
  return (regex_match(word, rgx)) ? true : false;
}
bool Variable::checkDouble(const std::string &word) {
  std::regex rgx("^(\\d+\\.\\d*|\\d*\\.\\d+)$");
  return (regex_match(word, rgx)) ? true : false;
}
bool Variable::checkChar(const std::string &word) {
  std::regex rgx("^.$");
  return (regex_match(word, rgx)) ? true : false;
}
bool Variable::checkString(const std::string &word) {
  std::regex rgx("^.*$");
  return (regex_match(word, rgx)) ? true : false;
}

Variable::~Variable() {}


/***** VarInt derived class *****/
VarInt::VarInt() : Variable(NUMERIC) {}
VarInt::VarInt(long long int value) : Variable(NUMERIC), m_value(value) {}

// Make an empty copy of the NUMERIC class, return as base class pointer
Variable* VarInt::clone() {
  Variable* new_copy = new VarInt();
  return new_copy;
}

// Return integer value for NUMERIC
long long int VarInt::getInt() {
  pthread_mutex_lock(&mutex); // Lock critical section
  if ((owner != nullptr)&&(owner != pthread_self())) // Block other threads
    pthread_cond_wait(&cond, &mutex); // Only one thread can access
  long long int item = m_value; // Set value
  pthread_mutex_unlock(&mutex); // Unlock critical section
  return item;
}
// For NUMERIC -> REAL conversion
double VarInt::getDouble() { return (double)getInt(); } 
void VarInt::print() { std::cout << getInt(); }
std::string VarInt::toString() { return std::to_string(getInt()); }

// Ensure long long int return given either long long int or double 
void VarInt::set(long long int value) { 
  if(is_const == true) throw(new MisException("can't update constant!\n"));
  pthread_mutex_lock(&mutex); // Lock critical section
  if ((owner != nullptr)&&(owner != pthread_self())) // Block other threads
    pthread_cond_wait(&cond, &mutex); // Only one thread can access
  m_value = value; // Set value
  pthread_mutex_unlock(&mutex); // Unlock critical section 
}
void VarInt::set(double value) { set((long long int)value); }
void VarInt::set(std::string value) {
  try { // Try to parse string as int 
    if(checkInt(value)) set(std::stoll(value)); 
    else throw(new MisException("not an int\n"));
  }
  catch(std::out_of_range err) { // Check if int is too big
    throw(new MisException(value + " doesn't fit in " +
			   "NUMERIC type (8 bytes)\n"));
  }
}

VarInt::~VarInt() {};


/***** VarDouble derived class *****/
VarDouble::VarDouble() : Variable(REAL) {}
VarDouble::VarDouble(double value) : Variable(REAL), m_value(value) {}

// Make an empty copy of the REAL class, return as base class pointer
Variable* VarDouble::clone() {
  Variable* new_copy = new VarDouble();
  return new_copy;
}

// Return float value for REAL
double VarDouble::getDouble() {
  pthread_mutex_lock(&mutex); // Lock critical section
  if ((owner != nullptr)&&(owner != pthread_self())) // Block other threads
    pthread_cond_wait(&cond, &mutex); // Only one thread can access
  double item = m_value; // Set value
  pthread_mutex_unlock(&mutex); // Unlock critical section
  return item;
}
// For REAL -> NUMERIC conversion
long long int VarDouble::getInt() { return (long long int)getDouble(); }
void VarDouble::print() { std::cout << getDouble(); }
std::string VarDouble::toString() { return std::to_string(getDouble()); }

// Ensure double return given either double or long long int parameter
void VarDouble::set(double value) {
  if(is_const == true) throw(new MisException("can't update constant\n"));
  pthread_mutex_lock(&mutex); // Lock critical section
  if ((owner != nullptr)&&(owner != pthread_self())) // Block other threads
    pthread_cond_wait(&cond, &mutex); // Only one thread can access
  m_value = value; // Set value
  pthread_mutex_unlock(&mutex); // Unlock critical section
}
void VarDouble::set(long long int value) { set((double)value); }
void VarDouble::set(std::string value) {
  try { // Try to parse string as double
    if(checkDouble(value)) set(std::stod(value)); 
    else throw(new MisException("not a double\n"));
  }
  catch (std::out_of_range err) {
    throw(new MisException(value + " doesn't fit in " +
			   "REAL type (8 bytes)\n"));
  }
}

VarDouble::~VarDouble() {};


/***** VarChar derived class *****/
VarChar::VarChar() : Variable(CHAR) {}
VarChar::VarChar(char value) : Variable(CHAR), m_value(value) {}
VarChar::VarChar(char* value) { set(value); }

// Make an empty copy of the CHAR class, return as base class pointer
Variable* VarChar::clone() {
  Variable* new_copy = new VarChar();
  return new_copy;
}

// Return char value for CHAR
char VarChar::getChar() {
  pthread_mutex_lock(&mutex); // Lock critical section
  if ((owner != nullptr)&&(owner != pthread_self())) // Block other threads
    pthread_cond_wait(&cond, &mutex); // Only one thread can access
  char item = m_value; // Set value
  pthread_mutex_unlock(&mutex); // Unlock critical section
  return item;
}
void VarChar::print() { std::cout << getChar(); }
std::string VarChar::toString() { return std::string(1, getChar()); }

void VarChar::set(char value) { 
  if(is_const == true) throw(new MisException("can't update constant\n"));
  pthread_mutex_lock(&mutex); // Lock critical section
  if ((owner != nullptr)&&(owner != pthread_self())) // Block other threads
    pthread_cond_wait(&cond, &mutex); // Only one thread can access
  m_value = value; // Set value
  pthread_mutex_unlock(&mutex); // Unlock critical section 
}
void VarChar::set(std::string value) {
  // Check if escape character
  if((value.size() == 2)&&(value.at(0) == '\\')) { 
    switch(value.at(1)) {
    case 't':
      set('\t');
      break;
    case 'n':
      set('\n');
      break;
    case 'r':
      set('\r');
      break;
    }
  } else if(checkChar(value)) { 
    set(value.at(0));
  } else { throw(new MisException(value + " is not a char\n")); } 
}

VarChar::~VarChar() {};


/***** VarDouble derived class *****/
VarString::VarString(unsigned int size) : Variable(STRING) {
  setMaxSize(size);
}
VarString::VarString(char* value, unsigned int size) : Variable(STRING) {
  setMaxSize(size);
  set(value);
}
VarString::VarString(std::string value, unsigned int size) : Variable(STRING) {
  setMaxSize(size);
  set((char*)value.c_str());
}

// Make an empty copy of the STRING class, return as base class pointer
Variable* VarString::clone() {
  Variable* new_copy = new VarString();
  return new_copy;
}

// Since returned char* is the class variable itself, improvements depend on
// usage of getString function in operation.
// Return char array for STRING
char* VarString::getString() {
  pthread_mutex_lock(&mutex); // Lock critical section
  if ((owner != nullptr)&&(owner != pthread_self())) // Block other threads
    pthread_cond_wait(&cond, &mutex); // Only one thread can access
  char* item = m_value; // Set value
  pthread_mutex_unlock(&mutex); // Unlock critical section
  return item;
}

// Ensure char array size (including \0) is smaller than maximum string size
bool VarString::checkSize() {
  return checkSize(m_value);
}

bool VarString::checkSize(char* value) {
  return ( strlen(value) < m_size ) ? true : false;
}

// Print string; takes into account escape characters
void VarString::print() {
  std::cout << toString();
}

// Convert string into proper string
std::string VarString::toString() {
  std::string buf = getString();
  std::string tmp;
  for(size_t i = 0; i < buf.size(); ++i) {
    char c = getString()[i];
    if((c == '\\')&&(i < buf.size() - 1)) { // If it's an escape character...
      char look_ahead = getString()[i+1]; // Check next character
      switch(look_ahead) {
      case 'n': tmp.append("\n"); break; // \n = Newline
      case 'r': tmp.append("\r"); break; // \r = Newline for other systems
      case 't': tmp.append("\t"); break; // \t = Tab
      case '\\': tmp.append("\\"); break; // \\ = literal "\"
      default: throw(new MisException("\\" + std::string(1, look_ahead) + 
				      " not valid escape charater!\n"));
      }
      ++i;
      continue; // Move on to next character
    } 
    tmp.append(std::string(1, getString()[i]));
  }
  return tmp;
}

// Set string value
void VarString::set(char* value) {
  if(is_const == true)
    throw(new MisException("can't update constant\n"));
  if(!(checkSize(value)))
    throw(new MisException(std::string(value) + " is too large: " +
			   "max is 256 characters\n"));
  pthread_mutex_lock(&mutex); // Lock critical section
  if ((owner != nullptr)&&(owner != pthread_self())) // Block other threads
    pthread_cond_wait(&cond, &mutex); // Only one thread can access
  doDelete(); // Clean up residue memory before setting new char array
  m_value = (char*)calloc(strlen(value) + 1, sizeof(char));
  strcpy(m_value, value);
  pthread_mutex_unlock(&mutex); // Unlock critical section  
}

void VarString::set(std::string value) {
  set((char*)value.c_str());
}

// Set m_size that determines maximum size of string buffer
void VarString::setMaxSize(unsigned int size) {
  if(size > MAX_STRING_SIZE) {
    throw(new MisException("Error: " + std::to_string(size)
                           + " exceeds max string size of "
                           + std::to_string(MAX_STRING_SIZE) + "!\n"));
  }
  m_size = size;
}

// Make sure char array is not empty before freeing
void VarString::doDelete() {
  if(m_value != nullptr) { free(m_value); }
}

VarString::~VarString() {
  doDelete();
}
