#ifndef GAMMA2PAIR_H
#define GAMMA2PAIR_H

#include <iostream>

#include "ATbase.h"

#include "../../_Environment/MagneticField/magnetic_field.h"
#include "../../LIBS/RandomLib/Random.hpp"
#include "../../utils/direction.h"

#include "GammaB.h"

/**
 * \ingroup  monte_carlo_grp
 * \defgroup pair_creation_grp Pair creation 
 */


/**
 * \ingroup pair_creation_grp 
 *
 * Auxiliary namespace associated with class Gamma2Pair
 */
namespace Gamma2Pair_namespace
{
  /**
   * Auxiliary class for optical depth evaluation in course of
   * numerical integration
   */
  class CS
  {
  public:
    void   SetMagneticField(MagneticField* p_mf) { _GB.SetMagneticField(p_mf); };

    void   SetupPhoton(double E, double X0 ) { _E  = E; _X0 = X0;};
    double operator() (double x) const       { return _GB.Sigma_x0_l(_E, _X0, x -_X0); };
    
  private:
    double _E;
    double _X0;
    GammaB _GB;
  };
}


/**
 * \ingroup pair_creation_grp 
 *
 * \class Gamma2Pair
 *
 * Class responsible for photon propagation and creation of
 * electron-positron pairs.
 *
 * Calling sequence:
 * - SetupPhoton(double e, double x0, Direction d )
 * - iterate PropagatePhoton() untill photon IsAbsorbed()
 * - call Get_Xcr() and Get_Psi() to get pair parameters
 * 
 * following SetupParametrs are requied:
 * - GeometryParams
 */
class Gamma2Pair
{
public:

  //! Constructor
  Gamma2Pair();

  //! Sample photon absorption - main method of the class
  bool IsAbsorbed(double &X, double &Psi);

  //! Setup photon 
  void SetupPhoton(double e, double x0, Direction d );

  //! setup all parameters from config file group
  void SetupFromConfigGroup(FileInput& in);

  //! Set pointer to the MagneticField functor
  void SetMagneticField(MagneticField* p_mf);

private:

  //! small step method of photon path sampling
  bool SmallSteps_Method(double A, double Tau1, double Tau_absorb, double &X, double &Psi);
  //! maximum cross-section method of photon path sampling
  bool SigmaMax_Method( double Sigma_max, double &X, double &Psi );
  //! large steps method of photon path sampling
  bool LargeSteps_Method(double X_max, double Sigma_max, double Tau_absorb, double &X, double &Psi );
  
  //! move photon
  bool MovePhoton(double L, double &X, double &Psi);
  //! distance to the boundary of calc domain
  double L_Max(double X);

private:

  double    _E;   //!< energy of the the photon under consideration
  double    _X0;  //!< emission point of the the photon under consideration
  Direction _Dir; //!< propagation direction of the the photon under consideration

  double _L; //! upper boundary of the calculation region

  //! minimum step in photon propagation, i.e. optical depth integration
  double _dX_Tau;

  //! relative error of optical depth calculation
  double _EPS_TAU;
  //! smallest optical depth for which photon propagation will be considered
  double _TAU_MIN;

  //! number of LargeSteps
  int _N_LS;
  /**
   * estimated number of cross-section evaluation in LargeSteps_Method
   * is used to decide whether to use LS method or use SS from the beginning
   */
  int _LS_N_SIGMA_EVAL;


  MagneticField*  _pMF;
  GammaB          _GB;

  //! random number generator for sampling photon propagation distance and photon absorption
  RandomLib::Random _Rand; 

  //! integration functor - used in large steps method
  ATbase::quad::QGK15 _GK;
  //! functor for cross-section integration
  Gamma2Pair_namespace::CS _CS;
};

#endif
