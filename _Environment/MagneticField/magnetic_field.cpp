#include "magnetic_field.h"

std::ostream& MagneticField::Print(std::ostream& s) const
{
  s<<"\n\n Magnetic Field : "<<_ClassName<<" <<<<<<<<<<<<< \n\n";
  s<<std::flush;
  return s;
}
