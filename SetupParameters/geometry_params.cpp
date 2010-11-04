#include "geometry_params.h"

double GeometryParams::mW=0;
double GeometryParams::mL=0;

double  GeometryParams::mKsi=1;   

/** 
 * Reads data from the input file and sets pulsar gap parameters
 * 
 */
void GeometryParams::SetupFromConfig(FileInput &in)
{
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup();

  // Read parameters from "Geometry" group
  in.ChangeGroup("GEOMETRY");
  mW = in.get_param("WIDTH");
  mL = in.get_param("LENGTH");

  mKsi = in.get_param("THETA");

  in.ChangeGroup(p_current_group);
}




/** 
 * Print Pulsar Gap parameters
 * 
 */
std::ostream& GeometryParams::Print(std::ostream& s) const
{
  s<<"\n Geometry parameters:\n\n";
  s<<"    Width = "<<std::setw(10)<<mW<<"\n";
  s<<"   Lenght = "<<std::setw(10)<<mL<<"\n";
  s<<"\n";
  s<<std::flush;

  return s;
}
