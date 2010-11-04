#include "formats.h"

namespace fmt
{

  Types Format::_Type = LONG;

  void  Format::SetDefaultFormat()   { _Type = LONG; };

  void  Format::SetFormat( Types T ) { _Type = T; };
  Types Format::GetFormat()          { return _Type; };

}
