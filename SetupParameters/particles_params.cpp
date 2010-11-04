#include "particles_params.h"

int ParticlesParams::_NGJ=1;


/** 
 * Reads data from the input file and sets pulsar gap parameters
 * 
 */
void ParticlesParams::SetupFromConfig(FileInput &in)
{
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup();

  // Read parameters from "PARTICLES" group
  in.ChangeGroup("PARTICLES");
  _NGJ  = static_cast<int>(in.get_param("NGJ"));

  in.ChangeGroup(p_current_group);
}


/** 
 * Print ParticlesParams  parameters
 * 
 */
std::ostream& ParticlesParams::Print(std::ostream& s) const
{
  s<<"\n Particles parameters:\n\n";
  s<<"   NGJ "<<std::setw(10)<<_NGJ;
  s<<"\n";

  s<<std::flush;

  return s;
}
