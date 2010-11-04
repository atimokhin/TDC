/**
 * \file formats.h this file contains functions and classes necessary
 * for custom switches in output streams
 * 
 */

#ifndef FORMATS_H
#define FORMATS_H

#include <iostream>


/**
 * \namespace fmt 
 * all functions/switches are in this namespace
 */
namespace fmt
{
  enum Types { SHORT, LONG };

  class Format
  {
  public:
    static void  SetFormat( Types T );
    static Types GetFormat();
    static void  SetDefaultFormat();
  
  private:
    static Types _Type;
  };


  template<class Ch, class Tr >
  std::basic_ostream<Ch,Tr>& short_format( std::basic_ostream<Ch,Tr>& s)
  {
    Format::SetFormat(SHORT);
    return s;
  }

  template<class Ch, class Tr >
  std::basic_ostream<Ch,Tr>& long_format( std::basic_ostream<Ch,Tr>& s)
  {
    Format::SetFormat(LONG);
    return s;
  }
}

#endif
