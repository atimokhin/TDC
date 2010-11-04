#include "time_params.h"

double TimeParams::_DT=0;
int    TimeParams::_N=0; 

/** 
 * Reads data from the input file and sets pulsar gap parameters
 * 
 */
void TimeParams::SetupFromConfig(FileInput &in)
{
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup();

  // Read parameters from "Time" group
  in.ChangeGroup("TIME");
  _DT = in.get_param("dT");
  _N  = static_cast<int>( in.get_param("NumberOfTimeSteps"));

  in.ChangeGroup();
}


/** 
 * Print Pulsar Gap parameters
 * 
 */
std::ostream& TimeParams::Print(std::ostream& s) const
{
  s<<"\n Time parameters:\n\n";
  s<<"                  dt = "<<std::setw(10)<<_DT<<"\n";
  s<<" Number of Timesteps = "<<std::setw(10)<<_N<<"\n";
  s<<"\n";
  s<<std::flush;

  return s;
}
