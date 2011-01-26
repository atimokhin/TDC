#ifndef PCF_B0_CONSTS_H
#define PCF_B0_CONSTS_H


#include "consts_initializer.h"
#include "norm_consts.h"
#include "magnetic_field_consts.h"
#include "ns_consts.h"


/**
 * \ingroup setup_consts_grp
 *
 * \class PcfB0_Consts
 * \brief Constants Initializer: initialized all constants using Pcf and \f$ B_0 \f$
 * 
 * configuration file example:
 \verbatim

   Group "DIMENSIONAL_CONSTANTS" {
      InitializerType='PcfB0_ConstsInitializer';
   
      Group "PcfB0_ConstsInitializer" {
         ! Pcf -- characteristic Lorentz factor of electrons 
         Pcf = 1e8;

         ! Magnetic field at the pole  [10^12 G]
         B_12 = -1;
         ! Radius of curvature of magnetic field lines [ 10^6 cm]
         Rcur_6 = 1;

         ! Optional:
         ! NS radius [10^6 cm]
         RNS_6 = 1;
      }
   }

 \ednverbatim
 *
 */
class PcfB0_ConstsInitializer: public ConstsInitializer,
                               public NormConsts, 
                               public MagneticFieldConsts,
                               public NSConsts
{
public:
  //! Setup from config file group
  void SetupFromConfigGroup(FileInput &in);
};

#endif
