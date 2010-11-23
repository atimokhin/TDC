#include <iostream>
#include <string>
#include <vector>

#include "ATbase.h"

#include "../SetupDimensionalConstants/dimensional_consts_setup.h"
#include "../SetupParameters/grid_params.h"
#include "../SetupParameters/geometry_params.h"

#include "../_Control/code_control.h"
#include "../_Control/output_control.h"

#include "../_Mesh/MeshAndLayouts.h"

#include "../_Environment/MagneticField/magnetic_field_maker.h"

#include "../_Particles/pairs.h"
#include "../_Particles/ParticleCaches/photon_cache.h"

#include "../_MC/PairProduction/gamma2pair.h"
#include "../_MC/monte_carlo.h"




//! config fielname
const string config_file ="@tests/test_absorption.input";

//! output_directory
const string output_directory ="../RESULTS/TESTS/";


// Typedefs -----------------------------------
// Mesh type and Point type **************
typedef MeshTraits<1,
		   double,
		   UniformRectilinearTag,CartesianTag,
		   1>::Mesh_t  Mesh_t; 

// Field type and Field layout ***********
typedef Field<Mesh_t,double,MultiPatch<GridTag,Brick> >  Field_t;
typedef Field_t::Engine_t::Layout_t                      FLayout_t;

// Particle traits and Particles Types
typedef MPDynamicSpatial<Mesh_t,FLayout_t>  PTraits_t;
typedef Pairs<PTraits_t>                    Pairs_t;
// ---------------------------------------------

using namespace std;
using namespace Gamma2Pair_namespace;

int main()
{
  FileInput in;
  in.ReadFile( config_file );

  DimensionalConstsSetup consts;
  consts.SetupFromConfig(in);

  GridParams grid;
  grid.SetupFromConfig(in);
  GeometryParams geom;
  geom.SetupFromConfig(in);

  CodeControl test;
  test.SetupFromConfig(in);
  OutputControl out;
  out.SetupFromConfig(in);
  out.Initialize();

  // setup photons ++++++++++++++++++++++++++++++
  PhotonCache ph;
  in.ChangeGroup("PHOTONS");

  int n_ph = static_cast<int>(in.get_param("NumberOfPhotons"));
  ph.Initialize(n_ph);

  double X0 = in.get_param("X0");
  ph.Set_X0( X0 );

  ph.Set_T0( in.get_param("T0") );
  ph.Set_Direction(  in.get_param("Direction") );

  int idts = static_cast<int>( in.get_param("IDTS") );
  ph.Set_IDTS( idts );
  
  int id = static_cast<int>( in.get_param("ID") );
  ph.Set_ID( id );
  
  cout<<"direction "<<ph.Get_Direction()<<"\n";


  double E    = in.get_param("E");
  double Psi0 = in.get_param("Psi0");

  for ( int i=0; i<n_ph; i++)
    ph.Add(1, E, Psi0, 'T');

  in.ChangeGroup();

  Save2HDF hdf_photons;
  string hdf_photons_filename = out.OutputDirName() + "photons.h5";
  hdf_photons.create(hdf_photons_filename.c_str());
  ph.Save2HDFFile(hdf_photons); 
  // ++++++++++++++++++++++++++++++++++++++++++++

  // ********************************************
  // Setup ID ***********************************
  // ********************************************
  ParticleID particle_id;
  particle_id.SetStartTimestep(0);
  particle_id.SetPatch(0);


  // ********************************************
  // Setup Mesh *********************************
  // ********************************************
  MeshAndLayouts<Mesh_t,Field_t,PTraits_t> ml;
  ml.SetupFromConfig(in);
  ml.SaveToHDFFile();


  // ********************************************
  // Setup Pairs ********************************
  // ********************************************
  Pairs_t pairs(ml.GetParticlesLayout());
  pairs.SetupOutputFile();
  // Setup particles parameters from config file
  in.ChangeGroup("PAIRS");
  pairs.SetupFromConfigGroup(in,particle_id);
  in.ChangeGroup();
  // ********************************************
  
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
  MC.SetPhotons(ph);


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
  
  // file with results of integration
  Save2HDF hdf_itgr;
  string hdf_itgr_filename = out.OutputDirName() + "dndx_itgr.h5";
  hdf_itgr.create(hdf_itgr_filename.c_str());
  // ********************************************

  
  // ********************************************
  // GK integrator ******************************
  // ********************************************
  ATbase::quad::QGK15 GK;

  if ( in.get_answer("Test_GammaB") )
    {
      // dNdX obtained by numerical integration
      double a=X0;
      double x;
      for ( int i=1; i<X.size(); i++)
	{
	  Sigma[i] = gb.Sigma_x0_l(E, X0, X[i]-X0 );
	  Tau[i]   = Tau[i-1] + abs( GK(a,X[i], cs, eps_tau) );
	  dNdX[i]  = Sigma[i]*exp(-Tau[i]);
	  a=X[i];
	}
      hdf_itgr.SaveSTLContainer( "X",    X );
      hdf_itgr.SaveSTLContainer( "dNdX", dNdX );
      hdf_itgr.SaveSTLContainer( "Tau",  Tau );
      hdf_itgr.SaveSTLContainer( "Sigma",Sigma );

      particle_id.NewTimestep(1); 

      // Monte Carlo
      MC.CreatePairs(pairs, particle_id);
      // save pairs to file
      pairs.SaveToHDFFile(0,1);
    }

  return 0;
}
