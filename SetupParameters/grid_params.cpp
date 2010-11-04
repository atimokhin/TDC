#include "grid_params.h"

int GridParams::_NX=0;

/** 
 * Reads data from the input file and sets pulsar gap parameters
 * 
 */
void GridParams::SetupFromConfig(FileInput &in)
{
  // read config file -----------------
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup();
  // Read parameters from "Grid" group
  in.ChangeGroup("GRID");

  _NX = static_cast<int>(in.get_param("NumberOfCells"));

  in.ChangeGroup(p_current_group);
}





/** 
 * Print Pulsar Gap parameters
 * 
 */
std::ostream& GridParams::Print(std::ostream& s) const
{
  s<<"\n Grid parameters:\n\n";
  s<<"   number of cells = "<<std::setw(10)<<_NX<<"\n";
  s<<"\n";
  s<<std::flush;

  return s;
}






