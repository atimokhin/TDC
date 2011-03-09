#ifndef TAU_PL_CONSTS_H
#define TAU_PL_CONSTS_H


#include "consts_initializer.h"
#include "norm_consts.h"
#include "magnetic_field_consts.h"


/**
 * \ingroup setup_consts_grp
 *
 * \class TauPl_ConstsInitializer
 * \brief Constants Initializer: initialized all constants using priod of
 *        GJ plasma oscillations
 *
 * Set normalization of the the time to the period of GJ plasma oscillations
 * 
 * configuration file example:
 \verbatim

   Group "DIMENSIONAL_CONSTANTS" {
      InitializerType='TauPl_ConstsInitializer';
   
      Group "TauPl_ConstsInitializer" {
         ! T0 -- period of GJ plasma oscillations in seconds
         T0 = 1;

         ! Magnetic field at the pole  [10^12 G]
         B_12 = -1;
         ! Radius of curvature of magnetic field lines [ 10^6 cm]
         Rcur_6 = 1;
      }
   }

 \ednverbatim
 *
 */
class TauPl_ConstsInitializer: public ConstsInitializer,
                               public NormConsts, 
                               public MagneticFieldConsts
{
public:
  //! Setup from config file group
  void SetupFromConfigGroup(FileInput &in);
};

#endif
