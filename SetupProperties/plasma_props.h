#ifndef GJ_PLASMA_PROPS_H
#define GJ_PLASMA_PROPS_H

#include <iostream>
#include <string>

#include "ATbase.h"

#include "setup_props.h"


/**
 * \ingroup setup_properties_grp 
 *
 * \class PlasmaProps
 * \brief physical properties of plasma
 *
 * following SetupParametrs are requied:
 * - GeometryParams
 * it needs:
 * - Normalization Constants (NormConsts)
 */
class PlasmaProps: public SetupProps
{
public:

  void Initialize(FileInput &in);

  //! plasma frequency 
  double OmegaPl() const { return _OmegaPl; };
  //! dimensionless period of plasma oscillations 
  double Tau() const { return _Tau; };
  //! dimensionless Debye length
  double LambdaDebye() const { return _LambdaDebye; };
  

  void SaveToHDFFile(Save2HDF& hdf) const;
  void ReadFromHDFFile(Save2HDF& hdf);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

protected:

  static double _OmegaPl;      //!< plasma frequency 
  static double _LambdaDebye;  //!< Debye length
  static double _Tau;          //!< dimensionless period of plasma oscillations 
};



inline std::ostream& operator<< (std::ostream& s, const PlasmaProps& pp)
{
  return pp.Print(s);
};

#endif
