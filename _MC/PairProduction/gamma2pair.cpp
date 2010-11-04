#include "gamma2pair.h"

#include "../../SetupParameters/geometry_params.h"


/**
 * Constructor
 */
Gamma2Pair::Gamma2Pair()
{
  _L = GeometryParams().L();
}

//! setup all parameters from config file group
void Gamma2Pair::SetMagneticField(MagneticField* p_mf)
{
  _pMF = p_mf;
  _CS.SetMagneticField(p_mf);
  _GB.SetMagneticField(p_mf);
}


/**
 *  Main routine in this class. Returns <b> true </b> if photon is
 *  absorbed, <b> false </b> otherwise.  Cordinate of absorption point
 *  and angle to the magnetic field are returned through parameters of
 *  the function.
 *
 *  It chooses the right method for photon propagation among 
 *  - SmallSteps_Method(double A, double Tau1, double Tau_absorb, double &B, double &Psi)
 *  - SigmaMax_Method( double Sigma_max, double &B, double &Psi )
 *  - LargeSteps_Method( double X_max, double Tau_absorb, double &B, double &Psi )
 *  and delegates calculation to them
 *
 * @param X   [out] photon absorption point coordinate
 * @param Psi [out] angle between photon momentum nad magnetic field 
 *                  at absorption point
 *
 */
bool Gamma2Pair::IsAbsorbed(double &X, double &Psi)
{
  // maximum cross-section
  double sigma_max = _GB.SigmaMax(_E, _X0, _Dir);
  // maximum photon path (to the boundary)
  double l_max = L_Max(_X0);
  // "maximum" optical depth
  double tau_ms  = l_max*sigma_max;

  // maximum number os Small Steps
  int n_ss = static_cast<int>( l_max/_dX_Tau );

#ifdef TEST_GAMMA2PAIR
  cout<<"--call to IsAbsorbed:------------------- \n";
  cout<<"maximum: photon path="<<l_max<<" sigma="<<sigma_max<<" tau="<<tau_ms;
  cout<<" predicted #SmallSteps="<<n_ss<<"\n";
#endif

  // ====> not absorbed: tau is too small and photon escapes
  if ( tau_ms < _TAU_MIN ) return false;

  // ====> try absorption
  if ( tau_ms < _LS_N_SIGMA_EVAL && tau_ms < n_ss )
    // *****> SigmaMax method
    return SigmaMax_Method( sigma_max, X,Psi);
  else
    // -----> Steps Methods
    {
      // optical depths
      double tau = -log( _Rand.Fixed() );
      
      if ( _LS_N_SIGMA_EVAL < n_ss )
	// *****> LargeSteps method
	return LargeSteps_Method( _pMF->XMax(_X0, _Dir),sigma_max, tau,  X,Psi );
      else
	// *****> SmallSteps method
	return SmallSteps_Method( _X0, 0, tau, X,Psi );
    }
}


/**
 * setup parameters from config file group
 */
void Gamma2Pair::SetupFromConfigGroup(FileInput& in)
{
  in.ChangeGroup("Gamma2Pair");

  _dX_Tau = in.get_param("dX_Tau");

  _TAU_MIN = in.get_param("TAU_MIN");
  _EPS_TAU = in.get_param("EPS_TAU");

  _N_LS = static_cast<int>(in.get_param("N_LS"));
  _LS_N_SIGMA_EVAL = static_cast<int>(in.get_param("LS_N_SIGMA_EVAL"));


  if ( in.get_answer("SetSeedForRandomNumberGenerator") )
    {
      int test_seed = static_cast<int>(in.get_param("Seed"));
      _Rand.Reseed(test_seed);
    }

  in.ChangeGroup("..");

}


/**
 * Setup internal variables with photons parameters
 */
void Gamma2Pair::SetupPhoton(double e, double x0, Direction d )
{
  _E   = e;
  _X0  = x0;
  _Dir = d;
}




/**
 *  Go along propagation path with step \f$ \delta x_{ph} \f$ and
 *  calculate optical depth \f$\tau=\tau_a+\int_a^b \sigma(x)\,dx\f$
 *  using <i> trapeze </i> method.  Point where
 *  \f$\tau\ge\tau_{absorb}\f$ is then the photon absorption point.
 * 
 * @param A    [in] starting point
 * @param Tau1 [in] optical depth at the starting point
 * @param Tau_absorb [in] optical depth when photon will be absorbed
 * @param X          [out] absorption point coordinate
 * @param Psi        [out] angle at the absorption point
 *
 * @return <b> true </b> if photon was absorbed
 *         <b> false </b> if photon escapes
 */
bool Gamma2Pair::SmallSteps_Method(double A, double Tau1, 
                                   double Tau_absorb, 
                                   double &X, double &Psi)
{
  double sigma_1, sigma_2;

  X = A;
  sigma_1 = _GB.Sigma_x_psi(_E, A, Psi);
  while ( Tau1 < Tau_absorb )
    {
      // move photon: if it escapes, return false
      if ( !MovePhoton(_dX_Tau, X, Psi) ) return false;

      sigma_2 = _GB.Sigma_x_psi(_E, X, Psi);
      Tau1 += 0.5*( sigma_1 + sigma_2 )*_dX_Tau;
      sigma_1 = sigma_2;
    }

#ifdef TEST_GAMMA2PAIR
  cout<<"**SmallSteps_Method: #steps="<<(X-A)/_dX_Tau;
  cout<<" x_start="<<A<<" x_end="<<X;
  cout<<" tau(x_end)="<<Tau1<<" desired tau="<<Tau_absorb<<"\n";
#endif  

  // if here -- photon is absorbed, return true
  return true;
}


/**
 * Sample photon propagation path using maximum cross-section method
 * 
 * @param Sigma_max [in]  maximum cross-section
 * @param X         [out] absorption point coordinate
 * @param Psi       [out] angle at the absorption point
 *
 * @return <b> true </b> if photon was absorbed
 *         <b> false </b> if photon escapes
 *
 */
bool Gamma2Pair::SigmaMax_Method( double Sigma_max, double &X, double &Psi )
{
  X = _X0;
  double l = -log(_Rand.Fixed())/Sigma_max;

#ifdef TEST_GAMMA2PAIR
  cout<<"++SigmaMax_Method: initial l="<<l<<"\n";
#endif  

  // propagate photon until absorption or escape
  while ( MovePhoton(l, X, Psi) )
    {
      // photon absorbed? -- if yes, return true
      if ( _Rand.Prob( _GB.Sigma_x_psi(_E, X, Psi)/Sigma_max ) ) 
	return true;

      l = -log( _Rand.Fixed() )/Sigma_max;    
    }

  // if here -- photon is outside the gap, return false
  return false;
}





bool Gamma2Pair::LargeSteps_Method(double X_max, double Sigma_max, 
						  double Tau_absorb, 
						  double &X, double &Psi )
{
  double d_tau;  // optical depth increment
  double tau1;   // estimated optical depth
  double tau;    // actual optical depth

  double sigma_up; // maximum sigma in large step segments

  double l_max; // maximum distance

  int i_ls, n_ls;  // counter and number of large steps
  double dx_ls; // size of Large Step

  double a, b; // integration limits
  
  double delta_tau = _EPS_TAU*Tau_absorb;
  
  // before maximum ------------------------
  l_max = fabs(X_max - _X0);
  // we may be very close to the maximum, so we do not want
  // to make many unnecessary steps 
  n_ls  = std::min(_N_LS,  static_cast<int>(l_max/_dX_Tau) );
  dx_ls = l_max/n_ls;

  tau1 = 0;
  b = a = _X0;
  i_ls = 0;
  while ( i_ls<n_ls && tau1<Tau_absorb )
    {
      X = b;
      MovePhoton(dx_ls, b, Psi);
      d_tau = _GB.Sigma_x_psi(_E, b, Psi)*dx_ls;

      // neglect path with very small tau
      if ( d_tau < delta_tau ) a = b;

      tau1 += d_tau;
      i_ls++;
    }
  // main part of tau
  _CS.SetupPhoton(_E,_X0);
  tau = fabs( _GK(a,X, _CS, _EPS_TAU) );

#ifdef TEST_GAMMA2PAIR
  cout<<"* LargeSteps_Method: before maximum @X_max="<<X_max<<": #iterations="<<i_ls;
  cout<<" x_{i_ls}="<<X<<" tau_{i_ls}="<<tau<<" desired tau="<<Tau_absorb<<"\n";
#endif

  // tau is within admitted error from the desired value
  if ( abs(tau-Tau_absorb) < delta_tau ) return true;

  // if main part is before maximum, continue with SmallSteps_Method
  if ( i_ls < n_ls-1 ) 
    return SmallSteps_Method(X, tau, Tau_absorb, X, Psi);



  // after maximum ------------------------
  l_max = L_Max(X_max);
  
  // if too many large steps are required, continue with SmallSteps_Method
  if ( _N_LS > l_max/_dX_Tau ) 
    return SmallSteps_Method( X_max, tau, Tau_absorb, X, Psi);

  dx_ls = l_max/_N_LS;

  i_ls = 0;
  tau1 = tau;
  b = a = X_max;
  sigma_up = Sigma_max;
  while ( i_ls<n_ls && tau1<Tau_absorb )
    {
      X = b;
      d_tau = sigma_up*dx_ls;

      MovePhoton(dx_ls, b, Psi);
      sigma_up = _GB.Sigma_x_psi(_E, b, Psi);

      tau1 += d_tau;
      i_ls++;
    }

#ifdef TEST_GAMMA2PAIR
  cout<<"* LargeSteps_Method: after maximum @X_max="<<X_max<<": #iterations"<<i_ls;
  cout<<" x_{i_ls}="<<X<<" tau_{i_ls}="<<tau1<<" desired tau="<<Tau_absorb<<"\n";
#endif

  // photon escapes
  if ( tau1<Tau_absorb ) return false;

  // main part of tau
  tau += fabs( _GK(a,X, _CS, _EPS_TAU) );

  return SmallSteps_Method( X, tau, Tau_absorb, X, Psi);
}




/**
 * Distance to the boundary of calculation domain from the point X
 *
 * @param X point coordinate
 */
double Gamma2Pair::L_Max(double X)
{
  switch (_Dir)
    {
    case UP :
      return fabs(_L-X);
    case DOWN :
      return X;
    }
}


/**
 * calculate new photon position and angle to the magnetic field after
 * propagation of distance L
 *
 * @param L   propagation distance
 * @param X   [inout] initial and final coordinate of the photon
 * @param Psi [out]   angle to the magnetic field at endpoint
 *
 * @return <b>true</b> if photon is inside calculation region,
 *         <b>false</b> othewise
 *
 */
bool Gamma2Pair::MovePhoton(double L, double &X, double &Psi)
{
  switch (_Dir)
    {
    case UP :
      X += L;
      if ( X > _L ) return false;  // outside the gap
      break;

    case DOWN :
      X -= L;
      if ( X < 0 ) return false;   // hit the surface
      break;
    }

  Psi = _pMF->Psi(_X0,X-_X0);
  return true; // photon is inside the gap
}

