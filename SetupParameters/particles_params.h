#ifndef PARTICLES_PARAMS_H
#define PARTICLES_PARAMS_H

#include <iostream>
#include <string>
#include "hdf5.h"
#include "ATbase.h"

#include "setup_params.h"


/**
 *  \ingroup setup_params_grp 
 *
 *  \class ParticlesParams
 *  \brief Class  with particles parameters
 */
class ParticlesParams: public SetupParams
{
public:
  //! particle denisty corresponding to \f$ \rho_{GJ} \f$
  int NGJ() const  { return _NGJ; };

  //! Setup data from config file
  void SetupFromConfig(FileInput &in);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

protected:

  //! number of particles corresponding to \f$ \rho_{GJ} \f$
  static int _NGJ;  
};

#endif
