#ifndef PSR_CONSTS_H
#define PSR_CONSTS_H


#include "consts_initializer.h"
#include "norm_consts.h"
#include "magnetic_field_consts.h"
#include "ns_consts.h"


/**
 * \ingroup setup_consts_grp
 *
 * \class PSR_Consts
 * \brief Constants Initializer: initialized all constants using pulsar parameters
 * 
 * configuration file example:
 \verbatim

   Group "DIMENSIONAL_CONSTANTS" {
      InitializerType='PSR_ConstsInitializer';
   
      Group "PSR_ConstsInitializer" {
         ! Period of pulsar [seconds]
         P_SEC = .2;
         ! Inclination angle
         CHI = 0;

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
class PSR_ConstsInitializer: public ConstsInitializer,
                             public NormConsts, 
                             public MagneticFieldConsts,
                             public NSConsts
{
public:
  //! Setup from config file group
  void SetupFromConfigGroup(FileInput &in);

  //! Puslar period \f$ P \f$ in seconds
  double P() const { return _P; };
  //! inclination angle
  double Chi() const { return _Chi; };

private:
  //! Pulsar period \f$ P \f$ in seconds
  static double _P;
  //! inclination angle - between \f$ \vec{\Omega} \f$ and \f$ \mu \f$
  static double _Chi;
};

#endif
