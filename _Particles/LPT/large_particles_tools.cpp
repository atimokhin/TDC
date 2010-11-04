#include "large_particles_tools.h"

/**
 * Setup parameters from config file.  In this realization it controls
 * only seeding of the RNG
 * 
 */
void LargeParticlesTools::SetupFromConfig(FileInput& in)
{
  // read parameters from the config file
  in.ChangeGroup("LPT");

  if ( in.get_answer("SetSeedForRandomNumberGenerator") )
    {
      int test_seed = static_cast<int>(in.get_param("Seed"));
      _Rand.Reseed(test_seed);
    }

  in.ChangeGroup();  
}
