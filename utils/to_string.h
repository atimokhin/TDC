#ifndef TO_STRING
#define TO_STRING

#include <string>
#include <sstream>

template<class T>
inline std::string to_string(T val)
{
  std::ostringstream ost;
  ost<<val;
  return ost.str();  
}


#endif 
