#include "particle_id.h"

int ParticleID::_Counter=0;
int ParticleID::_Timestep=0;
int ParticleID::_StartTimestep=0;

/** 
 * 
 * 
 * @param it local number of the timestep 
 */
void ParticleID::NewTimestep(int it) 
{ 
  _Timestep=it+_StartTimestep; 
  _Counter=0;
};

void ParticleID::SetStartTimestep(int it_start) 
{ _StartTimestep=it_start; };
