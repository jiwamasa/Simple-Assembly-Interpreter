// File containing middleware factory maps for parsing

#include "FactoryMap.h"

// Default constructor
FactoryMap::FactoryMap() {}

// Deletes objects in variable map
FactoryMap::~FactoryMap() {
  for(auto it = var_map.begin(); it != var_map.end(); it++) 
    delete((*it).second);
}
