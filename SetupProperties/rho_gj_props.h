#ifndef RHO_GJ_PROPS_H
#define RHO_GJ_PROPS_H

#include "setup_props.h"


/**
 * \ingroup setup_properties_grp 
 *
 * \class FMPProps
 *
 * properties of the Fundamental Macro Particle 
 *
 * following SetupParametrs are requied:
 * - GeometryParams
 * - ParticlesParams
 * it needs:
 * - Normalization Constants (NormConsts)
 */

class RhoGJProps: public SetupProps
{
public:

  void Initialize(FileInput &in);

  double N_Total() const { return _N_Total; };

  void SaveToHDFFile(Save2HDF& hdf) const;
  void ReadFromHDFFile(Save2HDF& hdf);

  std::ostream& Print(std::ostream& s) const;

private:

  static double _N_Total;  //<! Total number of particles GJ charge
};


#endif
