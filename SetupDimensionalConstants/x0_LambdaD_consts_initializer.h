#ifndef X0_LAMBDA_D_CONSTS_H
#define X0_LAMBDA_D_CONSTS_H


#include "consts_initializer.h"
#include "norm_consts.h"
#include "magnetic_field_consts.h"
#include "ns_consts.h"


/**
 * \ingroup setup_consts_grp
 *
 * \class X0_LambdaD_ConstsInitializer
 * \brief Constants Initializer: initialized all constants setting x0 to Debyey length of
 * GJ plasma (uses  \f$ X_0, B_0 \f$)
 * 
 * configuration file example:
 \verbatim

   Group "DIMENSIONAL_CONSTANTS" {
      InitializerType='X0_LambdaD_ConstsInitializer';
      
      Group "X0_LambdaD_ConstsInitializer" {
         ! X0 -- Debye length in centimeters
         X0 = 1;

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
class X0_LambdaD_ConstsInitializer: public ConstsInitializer,
                                    public NormConsts, 
                                    public MagneticFieldConsts,
                                    public NSConsts
{
public:
  //! Setup from config file group
  void SetupFromConfigGroup(FileInput &in);
};

#endif
