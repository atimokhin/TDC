#ifndef PARTICLE_ID_H
#define PARTICLE_ID_H


/**
 * \ingroup particles_grp
 * 
 * \defgroup id_grp  {LPT} Large Particle Tools 
 */



namespace ParticleID_Const
{
  const int N_PATCH_BASE = 1000000;
}


/**
 * \ingroup id_grp
 *
 * \class ParticleID This class provides methods for generating
 * particle ID's
 *
 * -Particle ID   is formed as patch*N_PATCH_BASE+_Counter 
 * -Particle IDTS is simply the number of the timestep
 *
 * So, each particle stores the timestep# where it was created IDTS
 * and unique ID which can be generated in each patch independently as
 * the base of unique ID in each patch is separated by
 * ParticleID_Const::N_PATCH_BASE from that in the adjustent patches
 */
class ParticleID
{
public:

  static void SetStartTimestep(int it_start);

  //! set patch number
  void SetPatch(int patch) { _PATCH_BASE = patch*ParticleID_Const::N_PATCH_BASE; };

  //! set number of the new timestep and reset counter
  void NewTimestep(int it);

  //! Timestep ID of the created particle
  int GetIDTS() const { return _Timestep; };
  //! get unique particle ID and advance internal counter
  int GetID()  { return _PATCH_BASE + _Counter++; }

private:
  
  int _PATCH_BASE;

  static int _StartTimestep;
  static int _Timestep;

  static int _Counter;
};

#endif
