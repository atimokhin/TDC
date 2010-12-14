#include <iostream>

#include "ATbase.h"

#include "../SetupDimensionalConstants/dimensional_consts_setup.h"
#include "../SetupParameters/geometry_params.h"
#include "../SetupParameters/grid_params.h"

#include "../_Control/output_control.h"

#include "../_Mesh/MeshAndLayouts.h"

#include "../_Environment/MagneticField/magnetic_field_maker.h"

#include "../_Particles/charged.h"
#include "../_Particles/ID/particle_id.h"

#include "../_MC/monte_carlo.h"


//! config fielname
const string config_file ="pairs_from_single_particle.input";
const string config_dir="x_Assist/";



// Mesh type and Point type **************
const int DIM=1;
typedef MeshTraits<DIM,
                   double,
                   UniformRectilinearTag,CartesianTag,
                   DIM>::Mesh_t  Mesh_t; 

// Field type and Field layout ***********
typedef Field<Mesh_t,double,MultiPatch<GridTag,Brick> >  Field_t;
typedef Field_t::Engine_t::Layout_t                      FLayout_t;

// Particle traits and Particles Types
typedef MPDynamicSpatial<Mesh_t,FLayout_t>  PTraits_t;
typedef Charged<PTraits_t>                  CParticles_t;
// ---------------------------------------------



int main(int argc, char *argv[])
{
  FileInput in;
  in.ReadFile(config_dir+config_file);

  DimensionalConstsSetup consts;
  consts.SetupFromConfig(in);

  GeometryParams geom;
  geom.SetupFromConfig(in);
  GridParams grid;
  grid.SetupFromConfig(in);

  OutputControl out;
  out.SetupFromConfig(in);
  out.CreateResultsDir();
  ATbase::filesys_utils::copyfile(config_dir+config_file, out.OutputDirName()+config_file);


  Pooma::initialize(argc, argv);

  // ********************************************
  // Setup Mesh *********************************
  // ********************************************
  MeshAndLayouts<Mesh_t,Field_t,PTraits_t> ml;
  ml.SetupFromConfig(in);
  ml.SaveToHDFFile();

  // ********************************************
  // Setup ID ***********************************
  // ********************************************
  ParticleID particle_id;
  particle_id.SetStartTimestep(0);
  particle_id.SetPatch(0);
  particle_id.NewTimestep(1); 


  // ********************************************
  // Setup Particles ****************************
  // ********************************************
  in.ChangeGroup("Particle");
  
  CParticles_t P(ml.GetParticlesLayout());

  // Setup particles parameters from config file
  P.SetupFromConfigGroup(in,particle_id);
  P.Swap();

  in.ChangeGroup();
  //std::cout<<P;
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


  // ********************************************
  // Emitting Time
  // ********************************************
  in.ChangeGroup("EMITTING_TIME");
  double dt_emission = in.get_param("dT");
  in.ChangeGroup();


  // HDF files setup ------------------------
  // HDF file with photon cache
  Save2HDF hdf_photons;
  string hdf_photons_filename = out.OutputDirName() + "cache_photons.h5";
  hdf_photons.create(hdf_photons_filename.c_str());
  // HDF file with pair cache
  Save2HDF hdf_pairs;
  string hdf_pairs_filename = out.OutputDirName() + "cache_pairs.h5";
  hdf_pairs.create(hdf_pairs_filename.c_str());
  // ----------------------------------------



  // =====================================================
  // Emitting Photons
  // =====================================================
  std::cout<<"\n\n Emitting Photons ================== \n";
  int i_P=0;
  PhotonCache &ph = MC.Get_PhotonCache();
  // set common photons parameters
  ph.SetInheritedParams( P.X(i_P)(0),0, P.P_par(i_P), P.IDTS(i_P),P.ID(i_P) );
  // emit photons
  MC.SingleParticleEmission(dt_emission, i_P, P);
  std::cout<<"  Direction: "<<ph.Get_Direction()<<"\n";
  std::cout<<"  Total number of emitted CR photons = "<<ph.Size()<<endl;
  // save emitted photons
  ph.Save2HDFFile(hdf_photons);
  // hdf_photons.writeScalar("Tau",cr.Get_Tau());

  // =====================================================
  // Creating Pairs
  // =====================================================
  cout<<"\n\n Creating Pairs ==================== \n";
  // Monte Carlo <<<<<<<<<<<<<<<<<<<<<<<<<<<<
  MC.CreatePairs(particle_id);
  // save pairs cache into HDF file ***
  PairCache &pairs = MC.Get_PairCache();
  pairs.Save2HDFFile(hdf_pairs); 
  std::cout<<"  Total number of created pairs = "<<pairs.Size()<<"\n\n";
  // ----------------------------------------      

  delete p_magnetic_field;
  Pooma::finalize();

  return 0;
}
