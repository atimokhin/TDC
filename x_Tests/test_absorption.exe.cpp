#include <iostream>
#include <string>
#include <vector>

#include "ATbase.h"

#include "../SetupDimensionalConstants/dimensional_consts_setup.h"
#include "../SetupParameters/grid_params.h"
#include "../SetupParameters/geometry_params.h"

#include "../_Control/code_control.h"
#include "../_Control/output_control.h"

#include "../_Environment/MagneticField/magnetic_field_maker.h"

#include "../_Particles/ParticleCaches/photon_cache.h"
#include "../_Particles/ParticleCaches/pair_cache.h"

#include "../_MC/PairProduction/gamma2pair.h"
#include "../_MC/monte_carlo.h"


//! config fielname
const string config_file ="test_absorption.input";
const string tests_dir="x_Tests/";


using namespace std;
using namespace Gamma2Pair_namespace;

int main()
{
  FileInput in;
  in.ReadFile(tests_dir+config_file);

  DimensionalConstsSetup consts;
  consts.SetupFromConfig(in);

  GeometryParams geom;
  geom.SetupFromConfig(in);

  CodeControl test;
  test.SetupFromConfig(in);

  OutputControl out;
  out.SetupFromConfig(in);
  out.CreateResultsDir();
  ATbase::filesys_utils::copyfile(tests_dir+config_file, out.OutputDirName()+config_file);


  // ********************************************
  // Setup photons ******************************
  // ********************************************
  PhotonCache ph;
  in.ChangeGroup("PHOTONS");
  int n_ph = static_cast<int>(in.get_param("PhotonCacheSize"));
  ph.Initialize(n_ph);
  double X0 = in.get_param("X0");
  ph.Set_X0( X0 );
  ph.Set_T0(         in.get_param("T0") );
  ph.Set_Direction(  in.get_param("Direction") );
  ph.Set_IDTS( static_cast<int>( in.get_param("IDTS") ) );
  ph.Set_ID(   static_cast<int>( in.get_param("ID")   ) );

  double E    = in.get_param("E");
  double Psi0 = in.get_param("Psi0");
  for ( int i=0; i<n_ph; i++)
    ph.Add(1, E, Psi0, 'T');

  in.ChangeGroup();
  cout<<"direction "<<ph.Get_Direction()<<"\n";
  // ********************************************


  // ********************************************
  // Setup ID ***********************************
  // ********************************************
  ParticleID particle_id;
  particle_id.SetStartTimestep(0);
  particle_id.SetPatch(0);

  
  // ********************************************
  // Setup Magnetic field ***********************
  // ********************************************
  in.ChangeGroup("MAGNETIC_FIELD");
  string magnetic_field_name = in.get_name("Type");
  MagneticField* p_magnetic_field = MagneticFieldMaker(in).pMake(magnetic_field_name); 
  p_magnetic_field->SetupFromConfigGroup(in);
  in.ChangeGroup();
  std::cout<<*p_magnetic_field;
  // ********************************************

  // ********************************************
  // Setup Monte Carlo **************************
  // ********************************************
  MonteCarlo MC;
  MC.SetupFromConfig(in);
  MC.SetMagneticField(p_magnetic_field);
  MC.Set_PhotonCache(ph);


  // ********************************************
  // Setup Integrated quantities ****************
  // ********************************************
  // class with gamma-B cross-section
  GammaB gb;
  gb.SetMagneticField(p_magnetic_field);
  // functor for optical depth integration
  CS cs;
  cs.SetMagneticField(p_magnetic_field);
  cs.SetupPhoton(E, X0);

  // read parameters from input file
  in.ChangeGroup("IntegratedQuantities");
  // divide interval functor
  int n_mesh   = static_cast<int>(in.get_param("NumberOfPoints"));
  double x_end = ph.Get_Direction()==UP ? geom.L() : 0;
  DivideInterval dvd(X0,x_end, n_mesh);
  // integration accuracy
  double eps_tau=in.get_param("Eps_Itgr");
  in.ChangeGroup();

  // array with axis points
  vector<double> X;
  dvd(X);

  vector<double> Sigma(X.size());  // cross-section
  vector<double> Tau(X.size());    // optical depth
  vector<double> dNdX(X.size());   // dNdX(x)
  // ********************************************

  
  // ********************************************
  // GK integrator ******************************
  // ********************************************
  ATbase::quad::QGK15 GK;

  if ( in.get_answer("Test_GammaB") )
    {
      // HDF files setup ------------------------
      // HDF file with photon cache
      Save2HDF hdf_photons;
      string hdf_photons_filename = out.OutputDirName() + "cache_photons.h5";
      hdf_photons.create(hdf_photons_filename.c_str());
      // HDF file with results of integration
      Save2HDF hdf_itgr;
      string hdf_itgr_filename = out.OutputDirName() + "dndx_itgr.h5";
      hdf_itgr.create(hdf_itgr_filename.c_str());
      // HDF file with pair cache
      Save2HDF hdf_pairs;
      string hdf_pairs_filename = out.OutputDirName() + "cache_pairs.h5";
      hdf_pairs.create(hdf_pairs_filename.c_str());
      // ----------------------------------------

      // save photon cache into HDF file ***
      ph.Save2HDFFile(hdf_photons); 

      // dNdX: Numerical integration ------------
      double a=X0;
      double x;
      for ( int i=1; i<X.size(); i++)
	{
	  Sigma[i] = gb.Sigma_x0_l(E, X0, X[i]-X0 );
	  Tau[i]   = Tau[i-1] + abs( GK(a,X[i], cs, eps_tau) );
	  dNdX[i]  = Sigma[i]*exp(-Tau[i]);
	  a=X[i];
	}
      // save dNdX into HDF file ***
      hdf_itgr.SaveSTLContainer( "X",    X );
      hdf_itgr.SaveSTLContainer( "dNdX", dNdX );
      hdf_itgr.SaveSTLContainer( "Tau",  Tau );
      hdf_itgr.SaveSTLContainer( "Sigma",Sigma );
      // ----------------------------------------


      // dNdX: Monte Carlo ----------------------
      particle_id.NewTimestep(1); 
      // Monte Carlo <<<<<<<<<<<<<<<<<<<<<<<<<<<<
      MC.CreatePairs(particle_id);
      // save pairs cache into HDF file ***
      PairCache &pairs = MC.Get_PairCache();
      pairs.Save2HDFFile(hdf_pairs); 
      // ----------------------------------------      
    }

  return 0;
}
