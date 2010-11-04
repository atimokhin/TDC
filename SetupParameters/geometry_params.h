#ifndef GEOMETRY_PARAMS_H
#define GEOMETRY_PARAMS_H

#include <iostream>
#include <string>
#include "hdf5.h"
#include "ATbase.h"

#include "setup_params.h"


/**
 *  \ingroup setup_params_grp 
 *
 *  \class GeometryParams
 *  \brief parameters of the gap (length, width etc.)
 *
 */
class GeometryParams: public SetupParams
{
public:

  //! Width of the polar cap [ in units of the dipole polar cap width ]
  double W() const { return mW; };
  //! Length of the polar cap [ in units of the dipole polar cap width ]
  double L() const { return mL; };

  //! Normalized colatitude \f$ \xi = \theta/\theta_{pc} \f$
  double Ksi() { return mKsi; };
  
  //! normilize time to the Flyby time
  double FlybyTime(double t) { return t/mL; };

  //! Setup data from config file
  void SetupFromConfig(FileInput &in);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

protected:

  static double mW; //! Width of the polar cap [ in units of the dipole polar cap width ]
  static double mL; //! Length of the polar cap [ in units of the dipole polar cap width ]

  //! normalized colatitude of the magnetic field line \f$ \xi = \theta/\theta_{pc} \f$
  static double mKsi;

};

#endif
