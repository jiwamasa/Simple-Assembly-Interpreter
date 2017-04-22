// Declaration of variable class
// Represents 4 MIS types: NUMERIC, REAL, CHAR, STRING 
// Utilizes polymorphism to handle errors by having an ecompassing parent 
//   class (Variable) for all of the derived child classes. The child classes 
//   only implement the appropiate functions, and whenever an unimplemented 
//   function is called, it goes to Variable, which throws and error

#ifndef VARIABLE_H
#define VARIABLE_H

#include "common.h"

const unsigned int MAX_STRING_SIZE = 256; // Character array type max size

// Used to differentiate between different data types when selecting operation
enum VarType {NUMERIC, REAL, CHAR, STRING};

class Variable {
 protected:
  // Variable value type deteremined/defined by derived class
  VarType m_type; // Variable type defined in all classes
  bool is_const = false;
  pthread_t owner = nullptr; // For variable locking; if null => unlocked
  pthread_mutex_t mutex; // Mutex for locking
  pthread_cond_t cond; // Lock condition
 public:
  Variable();
  Variable(VarType type);
  virtual Variable* clone() = 0; // Make an empty copy of itself

  // Selectors
  VarType getType();

  // Virtual functions should be defined by appropriate derived classes.
  // Throw an error if called from parent class.
  virtual long long int getInt();
  virtual double getDouble();
  virtual char getChar();
  virtual char* getString();
  virtual void print() = 0; // Print out member value contained in class
  virtual std::string toString() = 0; // Convert variable to string
  bool checkConst(); // Find out if variable is a "constant"
  bool checkLock(); // Find out if variable is locked

  // Modifiers
  virtual void set(long long int value);
  virtual void set(double value);
  virtual void set(char value);
  virtual void set(char* value);
  virtual void set(std::string value);
  void makeConst(); // Make the variable a "constant"
  void lock(pthread_t new_owner); // Lock variable
  void unlock(); // Remove the locked state of the variable

  // Ensures whether the passed string is of a given type
  virtual bool checkInt(const std::string &word);
  virtual bool checkDouble(const std::string &word);
  virtual bool checkChar(const std::string &word);
  virtual bool checkString(const std::string &word);

  virtual ~Variable();
};

// NUMERIC non-decimal integer value (8-bytes)
class VarInt : public Variable {
 private:
  long long int m_value;
 public:
  VarInt();
  VarInt(long long int value);
  virtual Variable* clone(); // Make an empty copy of itself

  // Selectors
  virtual long long int getInt();
  virtual double getDouble(); // For conversions
  virtual void print();
  virtual std::string toString(); // Convert variable to string
  
  // Modifiers
  virtual void set(long long int value);
  virtual void set(double value);
  virtual void set(std::string value);

  virtual ~VarInt();
};

// REAL decimal float value (8-bytes)
class VarDouble : public Variable {
 private:
  double m_value;
 public:
  VarDouble();
  VarDouble(double value);
  virtual Variable* clone(); // Make an empty copy of itself

  // Selectors
  virtual double getDouble();
  virtual long long int getInt(); // For conversions
  virtual void print();
  virtual std::string toString(); // Convert variable to string
  
  // Modifiers
  virtual void set(double value);
  virtual void set(long long int value);
  virtual void set(std::string value);

  virtual ~VarDouble();
};

// CHAR character value (1-byte)
class VarChar : public Variable {
 private:
  char m_value;
 public:
  VarChar();
  VarChar(char value);
  VarChar(char* value); // For escape characters
  virtual Variable* clone(); // Make an empty copy of itself

  // Selectors
  virtual char getChar();
  virtual void print();
  virtual std::string toString(); // Convert variable to string
  
  // Modifiers
  virtual void set(char value);
  virtual void set(std::string value);

  virtual ~VarChar();
};

// STRING character array value (1-256 bytes)
class VarString : public Variable {
 private:
  char* m_value = nullptr;
  unsigned int m_size;
 public:
  VarString(unsigned int size = MAX_STRING_SIZE);
  VarString(char* value, unsigned int size = MAX_STRING_SIZE);
  VarString(std::string value, unsigned int size = MAX_STRING_SIZE);
  virtual Variable* clone(); // Make an empty copy of itself

  // Selectors
  virtual char* getString();
  bool checkSize(); // Calls checkSize on member cstring value
  bool checkSize(char* value); // Ensure cstring value fits max size
  virtual void print();
  virtual std::string toString(); // Convert variable to string
  
  // Modifiers
  virtual void set(char* value);
  virtual void set(std::string value);
  void setMaxSize(unsigned int size);
  void doDelete(); // Clean up cstring memory

  ~VarString();
};

#endif
