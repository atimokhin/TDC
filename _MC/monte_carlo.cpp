#include "monte_carlo.h"


//! constructor
MonteCarlo::MonteCarlo() 
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

  int n_ph = static_cast<int>( in.get_param("MaxNumberOfPhotons") );

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


  // initialize photons container so it can store all photons
  // produced by any single particle
  int n_ph_max = _CR.MaxNumberOfPhotons();
  if ( n_ph < n_ph_max )
    {
      std::cout<<"Number of Photons n_ph="<<n_ph<<" is less than Maximum number of photons";
      std::cout<<" in emission processes! n_ph_max="<<n_ph_max<<endl;
      exit(1);
    }
  _Ph.Initialize(n_ph);
}


void MonteCarlo::SetMagneticField(MagneticField* p_mf)
{
  _G2P.SetMagneticField(p_mf);
  _CR.SetMagneticField(p_mf);
}


