#ifndef PSR_CONSTS_H
#define PSR_CONSTS_H


#include "consts_initializer.h"
#include "norm_consts.h"
#include "magnetic_field_consts.h"


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
         ! Magnetic field at the pole  [10^12 G]
         B_12 = -1;
         ! Period of pulsar [seconds]
         P_SEC = .2;
         ! Radius of curvature of magnetic field lines [ 10^6 cm]
         Rcur_6 = 1;
   
         ! Inclination angle
         CHI = 0;
         ! NS radius [10^6 cm]
         RNS_6 = 1;
      }
   }

 \ednverbatim
 *
 */
class PSR_ConstsInitializer: public ConstsInitializer,
                             public NormConsts, 
                             public MagneticFieldConsts
{
public:
  //! Setup from config file group
  void SetupFromConfigGroup(FileInput &in);

  //! Puslar period \f$ P \f$ in seconds
  double P() const { return _P; };
  //! inclination angle
  double Chi() const { return _Chi; };
  //! Radius of the NS \f$ R_{6} \f$ [\f$ 10^{6} \f$ cm ]
  double RNS_6() const { return _RNS_6; };  

private:
  //! Pulsar period \f$ P \f$ in seconds
  static double _P;
  //! inclination angle - between \f$ \vec{\Omega} \f$ and \f$ \mu \f$
  static double _Chi;
  //! Radius of the NS \f$ R_{6} \f$ in [\f$ 10^{6} \f$ cm ]   
  static double _RNS_6; 
};

#endif
