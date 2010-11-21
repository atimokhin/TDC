#ifndef CR_H
#define CR_H

#include <cmath>
#include <vector>
#include <algorithm>

#include "ATbase.h"

#include "../../_Environment/MagneticField/magnetic_field.h"

#include "../../LIBS/RandomLib/Random.hpp"

#include "../../PhysicsLib/small_functions.h"

#include "../../SetupDimensionalConstants/norm_consts.h"

#include "Fcr.h"
#include "Kcr.h"
#include "../Samples/Cutpoint.h"
#include "../../_Particles/ParticleCaches/photon_cache.h"



/**
 * \ingroup monte_carlo_grp
 *
 * \defgroup emission_grp Photon Emission
 */


/** 
 * \ingroup emission_grp
 * 
 * Constants with parameters of CR photon energy sampling fucntion Gcr 
 */
namespace CR_Const
{
  //! parameters for CR d.f. @{

  //! \f$ F_{CR}(\eta_1) \f$
  const double F_ETA_1 = 0.738125592902346;

  //! \f$ \eta_1 \f$
  const double ETA_1   = 0.01;
  //! \f$ \eta_{max} \f$
  const double ETA_MAX = 10;

  //! @}

  const char ORIGIN_TAG = 'C';

  //! normalization constant for d.f.
  const double F0 = 5.236e0;

  //! coefficient for  \f$ \epsilon_c \f$
  const double EPSILON_C_0 = 57.924;
}



/**
 * \ingroup emission_grp
 *
 * \class CR
 *
 * \brief This class is responsible for emission of Curvature Radiation Photons
 *
 * This class fills provided instance of Photons with curvature
 * photons.
 *
 * <B> Algorithm </B>
 *
 * - If particle has energy larger than some theshold value 
 *   \f$ E_e > E^{min}_e \f$ procced with emission, else do nothing
 *    -# \f$ E^{min}_e \f$ is set in the config file (CR<>::_E_PARTICLE_MIN)
 *
 * - calculate average number of photons emitted at a given time
 *   intreval \f$ dt \f$ -- aka optical depth \f$ \tau \f$,
 * - if \f$ \tau < n_{se}^{max} \f$, do %Discrete Emission,
 *   otherwise proceed with  Continuous Emission
 *   -# \f$ n_{se}^{max} \f$  is set in config file (CR<>::_N_SINGLE_EMISSION_MAX)
 *
 * <B> %Discrete Emission: </B>
 *
 * - do n_em=ceils(\f$\tau\f$) iterations, 
 *   at each iteration emit a photon with probability \f$\tau\f$/n_em
 * - Sample photons with \f$ \hat{F}(\eta)\equiv F(\eta)/F(0) \f$ distribution 
 *   function, where
 *   \f[
 *      F(\eta) \equiv \int\limits_{\eta}^\infty d\xi
 *                     \int\limits_{\xi}^\infty dx K_{5/3}(x)
 *   \f]
 *  photons are sampled in the range
 *  \f$ [ \epsilon_{min}, \epsilon_c \eta_{max}^{discr} ] \f$,
 *   -# \f$ \epsilon_{min} \f$ is set in config file (CR<>::_E_PHOTONS_MIN),
 *   -# \f$ \eta_{max}^{discr}  \f$ is the constant CR_Const::ETA_MAX
 *   
 *
 * <B> Contonuous Emission: </B> 
 * The spectrum of CR is divided in several bins and for each bin one
 * photon with the weight \f$ W_i \f$ is emitted.
 *
 * \f[
 *    W_i = A_{cont} K(\eta_i) \Delta\eta_i,
 * \f]
 *
 * where \f$ \eta \equiv \epsilon/\epsilon_c \f$,
 * \f[
 *    K(\eta) \equiv \int\limits_{\eta}^\infty K_{5/3}(x) \, dx
 * \f]
 * and
 * \f[
 *    A_{cont} = \Delta{}t A_F \frac{\gamma_6}{\rho_{cur}}
 * \f]
 *
 * The spectrum starts from some threshold photon energy 
 * \f$ \epsilon_{min} \f$ 
 * and extends up to \f$ \epsilon_{max} = \eta_{max}^{cont} \epsilon_c \f$. 
 *
 * Internal array stores values of 
 * \f$ \eta_i \in (\eta_{min},\eta_{max}^{cont}) \f$ and 
 * \f$ \kappa(\eta_i) \Delta\eta_i \f$. 
 * Division of array \f$ \eta_i \f$ is set in config file. So,
 * for each particle of energy \f$ E_e \f$ the spectrum of emitted
 * curvature photons is discretized at points 
 * \f$ \epsilon_c(E_e) * \eta_i \f$
 * 
 *
 *
 * The parameters set in config file includes:
 * - \f$ E^{min}_e \f$      - CR<>::_E_PARTICLE_MIN
 * - \f$ \epsilon_{min} \f$ - CR<>::_E_PHOTONS_MIN
 * - \f$ n_{se}^{max} \f$   - maximum number of single emission attempts
 *                            for larger \f$ \tau \f$ emission is continuous
 *                            (CR<>::_N_SINGLE_EMISSION_MAX)
 * - number of points in table containing discrete value of \f$ F_{CR} \f$
 * - number of <B> cutpoints </B> in table containing discrete value of \f$ F_{CR} \f$
 * - interval for setting table of \f$ \eta^{cont}_i \f$ used in continuous emission
 * - if required, RNG is reseesed to a specified seed (for tests)
 * . 
 * config file example:
 \verbatim

   Group "CR" {
      ! The lowest energy of particles emitting curvature photons (E^{min}_e)
      E_particle_min = 1e6;      
      ! The lowest energy of emitted CR photon ( \epsilon_{min} )
      E_photons_min = 2;      

      ! maximum number of single photon emission attempts n_{se}^{max}
      N_single_emission_max = 50;

      ! Discrete CR emission: number of points in table of Gcr
      N_Gcr_table_points    = 90;
      ! Discrete CR emission: number of cutpoints in table of Gcr
      N_Gcr_table_cutpoints = 60;

      ! Continuous CR emission: spectrum intervals (eta_min,eta_max) 
      Interval "Eta" {
	 Points = 1e-5,10;
	 Number_of_subintervals = 60;
	 Division_method = Log;
      }

      SetSeedForRandomNumberGenerator ? No;
      Seed = 1234;
   }

 \endverbatim
 * 
 *
 */
class CR
{
public:

  //! fill ph with curvature photons -  inline function 
  template<class Particles>
  bool EmitPhotons(double dt, int i, Particles& p, PhotonCache& ph);

  //! setup all parameters from config file group
  void SetupFromConfigGroup(FileInput& in);

  void SetMagneticField(MagneticField* p_mf) { _pMF = p_mf; };

  //! maximim number of photons which can be emitted
  int MaxNumberOfPhotons();

  //! optical depth to CR emission: mainly for test
  double Get_Tau();
  //! fraction of photons above \f$ \epsilon_{min} \f$: mainly for test
  double Get_F_min();
  //! peak energy of CR spectrum [in mc**2]: mainly for test
  double Get_EpsilonC();

 //! fill ph with curvature photons - main, "heavy", emission function
  bool EmitPhotons(double dt, double weight, double x0, double& p_par, PhotonCache& ph);

private:  

  //! "inverse" function to Fcr - used for photon sampling
  double Gcr(double gamma);

private:  
  
  MagneticField* _pMF;       //!< magnetic field functor
  Fcr            _Fcr;       //!< d.f. functor 

  //! Cutpoint functor for sampling phtoton energy with \f$\eta\in(\eta_1,\eta_{max})\f$
  Cutpoint _Gcr_CM; 
  
  //! random number generator for sampling photon energy
  RandomLib::Random _Rand; 


  double _Tau;      //!< optical depth to CR emission 
  double _F_min;    //!< fraction of photons above \f$ \epsilon_{min} \f$
  double _EpsilonC; //!< \f$ \epsilon_c \f$

  //! lower threshold value for CR emission \f$ \epsilon_{min} \f$
  double _E_PHOTONS_MIN;
  //! lower threshold value for Particle emitting curvature photon \f$  E^{min}_e  \f$
  double _E_PARTICLE_MIN;

  //! maximum number of single photon emission attempts \f$ n_{se}^{max} \f$
  int    _N_SINGLE_EMISSION_MAX;

  //! for continuous emission: \f$ \eta_i \f$
  std::vector<double> _Eta;            
  //! for continuous emission: \f$ K(\eta_i) \Delta\eta_i \f$
  std::vector<double> _KappaDeltaEta;  

  //! coefficient for discrete photon emission -- \f$ \hat{A}_{\rm{}F} \f$ 
  double _AF;
  //! coefficient for energy losses calculation
  double _C_losses;

  //! origin flag 
  static char _ORIGIN_TAG;
};



/** 
 * This is small inlined function which check whether emission occurs
 * and if yes it calls the main emission function
 * CR<MagneticField>::EmitPhotons(double weight, double dt, double x0,double& p_par, Photons& ph)
 *
 * <I> low energy particles do not radiate </I>
 * 
 * @param dt  timestep
 * @param i   number of particle
 * @param p   reference to a Charged class
 * @param ph  reference to a Photons class
 * 
 * @return true if emission occurs
 */
template<class Particles>
inline bool CR::EmitPhotons(double dt, int i, Particles& p, PhotonCache& ph)
{
  if ( fabs( p.P_par(i) ) < _E_PARTICLE_MIN ) 
    return false;
  else
    return EmitPhotons(dt, p.Weight(i), p.X(i)(0), p.P_par(i), ph);
}

#endif 
