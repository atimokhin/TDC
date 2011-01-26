#include "ns_consts.h"

double NSConsts::_RNS_6  = 1;


void NSConsts::SetupFromConfigGroup(FileInput &in)
{
  if ( in.param_is_set("RNS_6"))
    _RNS_6 = in.get_param("RNS_6");
}


/** 
 * Print NS parameters
 * 
 */
ostream& NSConsts::Print(ostream& s) const
{
  using namespace std;

  s<<" Neutron Star parameters:\n";
  s<<"                            R_NS = "<<setw(13)<<this->_RNS_6<<" 10^6 [cm]\n";
  s<<std::flush;

  return s;
}
