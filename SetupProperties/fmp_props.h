#ifndef FMP_PROPS_H
#define FMP_PROPS_H

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

class FMPProps: public SetupProps
{
public:

  void Initialize();

  double W0()    const { return _W0; };
  double W_FMP() const { return _WFMP; };


  void SaveToHDFFile(Save2HDF& hdf) const;
  void ReadFromHDFFile(Save2HDF& hdf);

  std::ostream& Print(std::ostream& s) const;

private:

  static double _W0;    //<! normalized charge of the FMP
  static double _WFMP;  //<! number of real physical particles in one FMP
};


#endif
