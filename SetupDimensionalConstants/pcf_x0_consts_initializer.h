#ifndef PCF_X0_CONSTS_H
#define PCF_X0_CONSTS_H


#include "consts_initializer.h"
#include "norm_consts.h"
#include "magnetic_field_consts.h"


/**
 * \ingroup setup_consts_grp
 *
 * \class PcfX0_Consts
 * \brief Constants Initializer: initialized all constants using Pcf and \f$ x_0 \f$
 * 
 * configuration file example:
 \verbatim

   Group "DIMENSIONAL_CONSTANTS" {
      InitializerType='PcfX0_ConstsInitializer';
   
      Group "PcfX0_ConstsInitializer" {
         ! Pcf -- characteristic Lorentz factor of electrons 
         Pcf = 1e8;
         ! X0  -- characteristic domain length
         X0 = 1e4;

         ! Magnetic field at the pole  [10^12 G]
         B_12 = -1;
         ! Radius of curvature of magnetic field lines [ 10^6 cm]
         Rcur_6 = 1;
      }
   }

 \ednverbatim
 *
 */
class PcfX0_ConstsInitializer: public ConstsInitializer,
                               public NormConsts, 
                               public MagneticFieldConsts
{
public:
  //! Setup from config file group
  void SetupFromConfigGroup(FileInput &in);
};

#endif
