#ifndef RS_CASCADE_PROPS_H
#define RS_CASCADE_PROPS_H

#include <iostream>
#include <string>

#include "ATbase.h"

#include "setup_props.h"


/**
 * \ingroup setup_properties_grp 
 *
 * \class RSCascadeProps
 * \brief Various parameters of Ruderman-Sutherland model
 *
 * It needs:
 * - MagneticField           (Environment::MagneticField_t)
 * - Pulsar parameters       (PSR_ConstsInitializer)
 * - Normalization Constants (NormConsts)
 */
class RSCascadeProps: public SetupProps
{
public:

  void Initialize(FileInput &in);

  //! gap height in cm
  double H_cm() const { return _H_cm; };
  //! gap height dimensionless
  double H() const { return _H; };

  //! potential drop in Volts
  double V_volts() const { return _V_volts; };
  //! potential drop dimensionless
  double V() const { return _V; };

  //! dimensionless Debye length
  double GammaMax() const { return _GammaMax; };
  

  void SaveToHDFFile(Save2HDF& hdf) const;
  void ReadFromHDFFile(Save2HDF& hdf);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

protected:

  static double _H;         //!< Ruderman Sutherland gap height dimensionless
  static double _H_cm;      //!< Ruderman Sutherland gap height in cm
  static double _V;         //!< Potential drop in Ruderman Sutherland gap dimensionless
  static double _V_volts;   //!< Potential drop in Ruderman Sutherland gap in Volts
  static double _GammaMax;  //!< Maximum Lorenz factor in Ruderman Sutherland gap
};



inline std::ostream& operator<< (std::ostream& s, const RSCascadeProps& pp)
{
  return pp.Print(s);
};

#endif
