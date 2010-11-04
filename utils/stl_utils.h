#include <iostream>

template <class Container>
inline void PrintSTLContainer( std::ostream& s, const Container& c )
{
  s<<std::flush;
  for ( typename Container::const_iterator iter = c.begin(); iter != c.end(); iter++ )
    { 
      s<<*iter<<",  ";
    }
  s<<"\n"<<std::flush;
};


