#include "monte_carlo.h"


//! constructor
MonteCarlo::MonteCarlo():
  _ElectronCache(ChargedParticleCache("Electrons")),
  _PositronCache(ChargedParticleCache("Positrons"))
{
  _DoOnTheSpotPairCreation_Flag   = false;
  _DoPairInjectionSmoothing_Flag  = false;
  _DoReducePairInjectionRate_Flag = false;
};


void MonteCarlo::SetupFromConfig(FileInput& in)
{
  My_Grid grid;

  // read config ---------------
  in.ChangeGroup("MONTE_CARLO");

  int n_ph   = static_cast<int>( in.get_param("PhotonCacheSize") );
  int n_pair = static_cast<int>( in.get_param("PairCacheSize") );
  int n_p    = static_cast<int>( in.get_param("PositronCacheSize") );
  int n_e    = static_cast<int>( in.get_param("ElectronCacheSize") );

  if ( in.answer_is_set("DoOnTheSpotPairCreation") )
    _DoOnTheSpotPairCreation_Flag = in.get_answer("DoOnTheSpotPairCreation");

  if ( in.answer_is_set("DoPairInjectionSmoothing") )
    _DoPairInjectionSmoothing_Flag = in.get_answer("DoPairInjectionSmoothing");
  // if requested do pair creation position smoothing setup
  if ( _DoPairInjectionSmoothing_Flag )
    {
      int n_smooth_cells = static_cast<int>( in.get_param("NSmoothCells") );
      _dX_Smooth = n_smooth_cells * grid.dX();
    }

  if ( in.answer_is_set("DoReducePairInjectionRate") )
    _DoReducePairInjectionRate_Flag = in.get_answer("DoReducePairInjectionRate");
  // if requested reduce rate of pair injection
  if ( _DoReducePairInjectionRate_Flag )
    {
      _PairInjectionRate_Factor = in.get_param("PairInjectionRate_Factor");
    }

  if ( ( _DoReducePairInjectionRate_Flag ||
         _DoPairInjectionSmoothing_Flag     )  &&
       in.get_answer("SetSeedForRandomNumberGenerator")
       )
    {
      int test_seed = static_cast<int>(in.get_param("Seed"));
      _Rand.Reseed(test_seed);
    }
  

  // radiation processes setup
  _CR.SetupFromConfigGroup(in);

  // absorption processes setup
  _G2P.SetupFromConfigGroup(in);

  in.ChangeGroup("..");
  // ---------------------------

  // Initialize Caches *********
  // check photon cache size
  int n_ph_rad_processes = _CR.MaxNumberOfPhotons();
  if ( n_ph < n_ph_rad_processes )
    {
      std::cout<<"Number of Photons n_ph="<<n_ph<<" is less than Maximum number of photons";
      std::cout<<" in emission processes! <n_ph_rad_processes="<<n_ph_rad_processes<<endl;
      exit(1);
    }
  _Ph.Initialize(n_ph);

  _PairCache.Initialize(n_pair);
  _ElectronCache.Initialize(n_e);
  _PositronCache.Initialize(n_p);
  // ****************************
}


void MonteCarlo::SetMagneticField(MagneticField* p_mf)
{
  _G2P.SetMagneticField(p_mf);
  _CR.SetMagneticField(p_mf);
}


/** 
 * This fuction iterates over internal Photons list _Ph and for each photon
 *
 * - calls _G2P.IsAbsorbed(x_cr, psi_cr) for doing Monte Carlo sampling 
 *    of photon absorption
 *
 * - if photon is absorbed it calculates injection time and creates 
 *    a new virtual pair in _PairCache
 * 
 */
bool MonteCarlo::CreatePairs(ParticleID& id)
{
  bool pairs_created = false;
  double x_cr, psi_cr;

  int e_sign = _Ph.Get_MomentumSign();

  // iterate over all photons *********
  for (int i=0; i<_Ph.Size(); i++)
    {
#ifdef TEST_GAMMA2PAIR
      bool test_photon_absorbed = false;
#endif

      // setup photons parameters in Gamma2Pair class
      _G2P.SetupPhoton(_Ph.E[i],_Ph.Get_X0(),_Ph.Get_Direction() );
      // if absorbed - create pair
      if ( _G2P.IsAbsorbed(x_cr, psi_cr) ) 
	{
	  if ( !pairs_created ) pairs_created = true;
	  // pair injection time
	  double t_cr = _Ph.Get_T0() + fabs( x_cr - _Ph.Get_X0() );

	  // if requested use on the spot approximation <<============= (!!!)
	  if ( _DoOnTheSpotPairCreation_Flag )
	    {
	      t_cr = _Ph.Get_T0();
	      x_cr = _Ph.Get_X0();
	    }	  
          // ----------------------------------------------------------
	  // if requested do pair creation position smoothing <<====== (!!!)
	  if ( _DoPairInjectionSmoothing_Flag )
	    {
	      t_cr += _dX_Smooth * _Rand.Fixed();
	    }	  
          // ----------------------------------------------------------

	  // add newly created pair
	  _PairCache.Add(_Ph.Weight[i], 
                         t_cr, x_cr, _Ph.Get_X0(), 
                         e_sign*_Ph.E[i], psi_cr, 
                         _Ph.Origin[i],
                         _Ph.Get_IDTS(), _Ph.Get_ID(), id.GetIDTS(), id.GetID() );

#ifdef TEST_GAMMA2PAIR
	  test_photon_absorbed = true;
	  std::cout<<"travelled l="<<x_cr - _Ph.Get_X0()<<"\n";
	  std::cout<<">>>>>>>[ABSORBED]<<<<<<<\n\n";
#endif
	}

#ifdef TEST_GAMMA2PAIR
      if (!test_photon_absorbed)  std::cout<<".......[ESCAPED]........\n\n";
#endif
    }

  return pairs_created;
}

