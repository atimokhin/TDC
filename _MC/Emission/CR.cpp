#include "CR.h"

char CR::_ORIGIN_TAG = CR_Const::ORIGIN_TAG;


void CR::SetupFromConfigGroup(FileInput& in)
{
  NormConsts nc;
  Kcr k_cr;

  // -------------------------------------
  // read parameters from the config file
  // -------------------------------------
  in.ChangeGroup("CR");

  _E_PHOTONS_MIN  = in.get_param("E_photons_min");
  _E_PARTICLE_MIN = in.get_param("E_particle_min");

  _N_SINGLE_EMISSION_MAX = static_cast<int>(in.get_param("N_single_emission_max"));

  int n_gcr_table_points    = static_cast<int>(in.get_param("N_Gcr_table_points"));
  int n_gcr_table_cutpoints = static_cast<int>(in.get_param("N_Gcr_table_cutpoints"));

  DivideInterval eta_interval = in.get_interval("Eta");

  if ( in.get_answer("SetSeedForRandomNumberGenerator") )
    {
      int test_seed = static_cast<int>(in.get_param("Seed"));
      _Rand.Reseed(test_seed);
    }

  in.ChangeGroup(".."); 
  // ------------------------------------


  // -------------------------------------
  // setup arrays for discrete emission (Gcr table)
  // -------------------------------------
  DivideInterval eta_gcr_interval(CR_Const::ETA_MAX,
				  CR_Const::ETA_1, 
				  n_gcr_table_points, 
				  ATbase::LOG);

  vector<double> eta_gcr_table, f_gcr_table;
  eta_gcr_interval(eta_gcr_table);
  f_gcr_table.resize(eta_gcr_table.size());
  for (int i=0; i<eta_gcr_table.size(); i++) 
    f_gcr_table[i]=_Fcr(eta_gcr_table[i]);

  _Gcr_CM.Initialize(eta_gcr_table,f_gcr_table, n_gcr_table_cutpoints);
  // -------------------------------------


  // -------------------------------------
  // setup arrays for continuous emission
  // -------------------------------------
  vector<double> eta_cells;
  eta_interval(eta_cells);
  
  _Eta.resize(eta_cells.size()-1);
  _KappaDeltaEta.resize(_Eta.size());

  for ( int i=0; i<_Eta.size(); i++)
    {
      _Eta[i]           = 0.5*(eta_cells[i+1]+eta_cells[i]);
      _KappaDeltaEta[i] = k_cr(_Eta[i]) * (eta_cells[i+1]-eta_cells[i]);
    }
  // -------------------------------------

  
  // -------------------------------------
  // setup coefficients
  // -------------------------------------
  // coefficient for optical depth calculation
  _AF       = 3.158e8 * nc.T0();
  // coefficient for energy losses
  _C_losses = 5.632e9 * nc.T0();
  // -------------------------------------
};



/** 
 * @return maximim number of photons which can be emitted
 */ 
int CR::MaxNumberOfPhotons() 
{ 
  return std::max(static_cast<int>(_Eta.size()),_N_SINGLE_EMISSION_MAX); 
}






/** 
 * This fuctions fills Photons ph with emitted curvature photons photons
 * 
 * @param dt     timestep
 * @param Weight weight of the emitting particle
 * @param X0     positions of emitting particle
 * @param P_par  [in/out] reference to  Charged.P_par -- \f$ p_{||} \f$
 * @param ph     [out]    reference to a Photons class
 */
bool CR::EmitPhotons(double dt, 
                     double Weight,  double X0, double& P_par,  
                     Photons& ph)
{
  // perpedicular momentum of emitted photon
  const double P_PERP_PH=0;

  bool is_emitted_flag = false;

  double g6              = fabs(P_par)/1e6;      // \gamma_6
  double g6_div_rcurv    = g6/_pMF->Rcur(X0);      // \gamma_6/\rho_{curv,6}
  double g6__3_div_rcurv = g6*g6*g6_div_rcurv;   // \gamma_6^3/\rho_{curv,6}

  // peak energy of CR
  _EpsilonC = CR_Const::EPSILON_C_0 * g6__3_div_rcurv;

  double eta_min = _E_PHOTONS_MIN/_EpsilonC;
  _F_min = _Fcr(eta_min);

  // numerical coefficient for discrete emission
  double A_discr = _AF * g6_div_rcurv * dt;

  // optical depth -- avergae number of emitted photons
  _Tau = _F_min * A_discr; 
  int n_em = static_cast<int>( ceil(_Tau) );

  if ( n_em < _N_SINGLE_EMISSION_MAX )
    // discrete emission **************************
    {
#ifdef TEST_CR
      cout<<"CR: Discrete Emissiom Mode ----------------\n";
      cout<<"average number of emitted photons = "<<n_em<<"\n";
#endif

      for ( int i=0; i<n_em; i++)
	{
	  bool is_emitted_now_flag = _Rand.Prob( _Tau/n_em );
	  if ( is_emitted_now_flag )
	    {
	      double ksi  = _Rand.Fixed()*_F_min;
	      double e_ph = _EpsilonC * Gcr(ksi);
	      ph.Add(Weight, e_ph, P_PERP_PH, _ORIGIN_TAG);
	    }
	  is_emitted_flag = is_emitted_now_flag || is_emitted_flag;
	}
    }
   // ********************************************
  else 
   // continuous emission ************************
    {
#ifdef TEST_CR
      cout<<"CR: Continuous Emissiom Mode --------------\n"; 
#endif

      is_emitted_flag = true;

      double A_cont = Weight * A_discr/CR_Const::F0;
      int i_eta_min = std::lower_bound(_Eta.begin(),_Eta.end(), eta_min) - _Eta.begin();

      // fill photons array
      for ( int i = i_eta_min; i< _Eta.size(); i++)
	{
	  double e_ph = _EpsilonC * _Eta[i];
	  double w_ph = A_cont * _KappaDeltaEta[i] ;    
	  ph.Add(w_ph, e_ph, P_PERP_PH, _ORIGIN_TAG);
	}
    }
   // ********************************************


  // --------------------------------
  // Radiation reaction
  // --------------------------------
  P_par -= SF::Sign(P_par) * _C_losses * g6__3_div_rcurv*g6_div_rcurv * dt;

  return  is_emitted_flag;
};


/** 
 * Inverse fucntion for sampling energy of emitted CR photons. 
 *
 * For low energy part (corresponding to \f$ \eta <\eta_1 \f$) it uses
 * analytic approximation derived from \f$ F(\eta)=1-a\eta^{-1/3} \f$.
 * For high energy part (\f$ \eta >\eta_1 \f$) it uses cutpoint method
 * by calling _Gcr_CM(ksi)
 *
 * @param ksi random uniformly distributed on [0,1) variate  
 * 
 * @return eta 
 */
double CR::Gcr(double ksi)
{
  double const A = 1.231588961841031; 

  if ( ksi >= CR_Const::F_ETA_1 )
    return pow ( (1-ksi)/A, 3);
  else
    return _Gcr_CM(ksi); 
}


/** 
 * optical depth to CR photon emission. It is set up when
 * CR::EmitPhotons() is called
 *
 * This function is used  in tests
 *
 * @return optical depth
 */ 
double CR::Get_Tau() 
{ 
  return _Tau;
}


/** 
 * fraction of photons with energies higher than \f$ \epsilon_{min} \f$
 * of the all curvature photons emitted
 * It is set up when 
 * CR::EmitPhotons() is called * 
 *
 * This function is used in tests
 *
 * @return _F_min
 */
double CR::Get_F_min() 
{ 
  return _F_min;
}


/** 
 * peak energy of CR spectrum [in mc**2]. It is set up when 
 * CR::EmitPhotons() is called
 *
 * This function is used in tests
 *
 * @return optical depth
 */ 
double CR::Get_EpsilonC() 
{ 
  return _EpsilonC;
}

