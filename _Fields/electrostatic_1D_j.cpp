#include "electrostatic_1D_j.h"


double Electrostatic_1D_J::operator() ( double t)
{
  if ( _Type == "None" )
    {
      return 0;
    }
  else if ( _Type == "Constant" )
    {
      return _J0;
    }
  else
    {
      std::cout<<"Wrong Electrostatic_1D_J type: "<<_Type<<"\n";
      exit(1);
    }
}


void Electrostatic_1D_J::SetupFromConfigGroup(FileInput& in)
{
  _Type = in.get_name("Type");

  if ( _Type == "Constant" )  
    {
      _J0 = in.get_param("J0");
    }
}
