#ifndef PULSAR_GAP_PROPS_H
#define PULSAR_GAP_PROPS_H

#include <iostream>
#include <string>

#include "ATbase.h"

#include "setup_props.h"

/**
 * \ingroup  setup_properties_grp 
 *
 * \class PulsarGapProps 
 * \brief physical properties of pulsar gap 
 *
 *  following SetupParametrs are requied:
 * - GeometryParams
 * it needs:
 * - Normalization Constants (NormConsts)
 */
class PulsarGapProps: public SetupProps
{
public:

  void Initialize(FileInput &in);

  //! Vacuum potential drops in statvolts
  double V_vac() const { return _Vvac; };
  //! Vacuum potential drops in Volts
  double V_vac_Volts() const { return _Vvac*299.7; };
  double V_vac_L_Volts() const { return _Vvac_L*299.7; };
  //! Coefficient in the equation of particle motion
  double Pcf() const { return _Pcf; };
  //! GJ number density
  double n_GJ() const { return _n_GJ; };
  

  void SaveToHDFFile(Save2HDF& hdf) const;
  void ReadFromHDFFile(Save2HDF& hdf);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

protected:

  static double _Vvac;     //!< Vacuum potential drops in statvolts
  static double _Pcf;      //!< Coefficient in the equation of particle motion

  static double _Vvac_L;   //!< Vacuum potential drops in statvolts over the whole domain
  static double _Pcf_L;    //!< Vacuum potential drops in mc^2 over the whole domain

  static double _n_GJ;     //!< Goldreich-Julian particle number density
  static double _P;        //!< Pulsar period
  static double _Rpc;      //!< Pulsar polar cap width
  static double _B_12;     //!< Pulsar magnatic field (from electrodynamics)
};



inline std::ostream& operator<< (std::ostream& s, const PulsarGapProps& pg)
{
  return pg.Print(s);
};

#endif
