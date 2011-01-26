#ifndef MAGNETIC_FIELD_CONSTS_H
#define MAGNETIC_FIELD_CONSTS_H

#include "ATbase.h"

/**
 * \ingroup setup_consts_grp
 *
 * \class  MagneticFieldConsts
 * \brief  Contains constants related to the background magnetci field
 *
 * Contains constants related to the background magnetic field:
 * - \f$ B_{12} \f$   --  magnetic field strength in \f$ 10^{12} \f$ Gauss
 * - \f$ sign(B) \f$  --  sign of the magnetic %field (+1|0|-1)
 * - \f$ \rho_{\mbox{cur 6}} \f$  --  radius of curvature of the magnetic field lines in  \f$ 10^{6} \f$ cm  
 *
 *
 * configuration file example:
 \verbatim

   Group "DIMENSIONAL_CONSTANTS" {
      InitializerType='Some_ConstsInitializer';
   
      Group "Some_ConstsInitializer" {

         ...

         ! Magnetic field at the pole  [10^12 G]
         B_12 = -1;
         ! Radius of curvature of magnetic field lines [ 10^6 cm]
         Rcur_6 = 1;

         ...
      }
   }

 \ednverbatim
 */
class MagneticFieldConsts
{
public:
  //! Setup from config file group
  void SetupFromConfigGroup(FileInput &in);

  //! magnetic %field strength in \f$ 10^{12} \f$ Gauss
  double B_12()  const { return _B_12; };
  //! Sign of the magnetic %field (+1|0|-1)
  double SignB() const { return _SignB; };

  //! radius of curvature of the magnetic field lines in  \f$ 10^{6} \f$ cm  
  double Rcur_6() const { return _Rcur_6; };

protected:
  static double _B_12;
  static double _SignB; 
  static double _Rcur_6;
};

#endif
