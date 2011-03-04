#include <iostream>
#include <list>
#include "argtable2.h"

#include "ATbase.h"

#include "_Control/input_control.h"
#include "_Control/output_control.h"

#include "SetupParameters/params_list.h"

#include "SetupProperties/setup_props_list.h"

#include "_Mesh/MeshAndLayouts.h"

#include "_Particles/charged.h"
#include "_Particles/pairs.h"

#include "_Fields/em_fields.h"
#include "_Fields/em_fields_maker.h"

#include "_PIC/cic_interpolators.h"

#include "_SolutionProperties/energy_vs_time.h"
#include "_SolutionProperties/particles_prop_fields.h"


using namespace std;

// ----------------------------------------------
// Typedefs 
// ----------------------------------------------

// Mesh type and Point type **************
const   int DIM=1;
typedef MeshTraits<DIM,
                   double,
                   UniformRectilinearTag,CartesianTag,
                   DIM>::Mesh_t  Mesh_t; 
typedef Mesh_t::PointType_t                         PointType_t; 

// Field type and Field layout ***********
typedef Field<Mesh_t,double,MultiPatch<GridTag,Brick> >  Field_t;
typedef Field_t::Engine_t::Layout_t                      FLayout_t;

// Particle traits and Particles Types
typedef MPDynamicSpatial<Mesh_t,FLayout_t>  PTraits_t;
typedef Charged<PTraits_t>                  CParticles_t;
typedef Pairs<PTraits_t>                    Pairs_t;

// Types of Electromagnetic fields and Particle list
typedef EMFields<Field_t>  EM_t;
// ----------------------------------------------







//! Main function for properties calculation
int main(int argc, char *argv[])
{
  FileInput in_energy;
  in_energy.ReadFile( "prop.input" );

  // setup input <<<<<<<<
  // calc_id
  InputControl ic;
  ic.SetupFromCommandLine(argc, argv);
  // open config file and setup input directory
  FileInput in;
  in.ReadFile( ic.ConfigFileName() );
  ic.SetupFromConfig(in);

  // setup output <<<<<<<
  OutputControl out;
  out.SetupFromConfig(in);
 
  if ( ic.AskedForHelp() || out.AskedForHelp() )
    exit(0);

  // setup parameters <<<
  ParameterList params;
  params.SetupFromConfig(in);

  // setup properties <<<
  SetupPropsList props;
  props.Initialize(in);


  // --------------------------------
  // Initialize POOMA 
  // --------------------------------
  Pooma::initialize(argc, argv);

  // ********************************************
  // Setup Mesh *********************************
  // ********************************************
  MeshAndLayouts<Mesh_t,Field_t,PTraits_t> ml;
  ml.SetupFromConfig(in);

  // Timetable **********************************
  Timetable tt;
  // should set StartTimestep to zero, as timesteps 
  // are set already in the main files
  Timetable::SetStartTimestep( 0 );
  int            n_timeshots; 
  vector<double> time;
  vector<int>    timesteps;

  // ********************************************
  // Particles properties
  // ********************************************
  // setup Particle List ------------------------
  typedef std::list<std::string> NameList_t;
  NameList_t particle_names;

  in.ChangeGroup("PARTICLES");
  
  int n_part = static_cast<int>(in.get_param("NumberOfChargedParticleTypes"));
  for ( int i=0; i<n_part; i++)
    particle_names.push_back( in.get_name("ChargedParticles",i) );

  in.ChangeGroup();
  // --------------------------------------------

  // --------------------------------------------
  // iterate over individual particles in the particles list
  // --------------------------------------------
  for ( NameList_t::iterator iter=particle_names.begin(); 
        iter != particle_names.end(); iter++ )
    {
      // Setup Fields with particle properties
      ParticlesPropFields<Field_t> ParticlesProps;
      ParticlesProps.SetupFromConfigGroup(in_energy);
      ParticlesProps.Initialize(ml.GetFieldLayout(), ml.GetMesh());
      string particles_prop_filename = "prop_"+ *iter + ".h5";
      ParticlesProps.SetupOutputFile( particles_prop_filename );

      // Setup Particles
      CParticles_t particles(ml.GetParticlesLayout()); 
      // name of current particles HDF5 file
      string particles_filename =  ic.InputDirName() + *iter + ".h5";
      cout<<"Working with file \""<<particles_filename<<"\"...\n";
      // open Particles file
      Save2HDF hdf_particles;
      hdf_particles.open( particles_filename.c_str() );

      // n_timeshots, time, timesteps ------------
      n_timeshots = tt.GetNumbeOfTimeshots(hdf_particles);
      time        = tt.GetTimeArray(hdf_particles);
      timesteps   = tt.GetTimestepArray(hdf_particles);

      // ----------------------------------------
      // iterate over timeshots
      // ----------------------------------------
      cout<<"Number of Timeshots: "<<n_timeshots<<"\n";
      cout<<"Reading: ";
      for ( int it=1; it<=n_timeshots; it++)
        {
          cout<<it<<" "<<flush;
          // read particles 
          particles.ReadFromHDFFile(it, hdf_particles);
          // calculate particle number density n(x)
          ParticlesProps.Scatter_N_Charged( particles );
          // save dataset with n(x)
          ParticlesProps.SaveToHDFFile(time[it-1], timesteps[it-1]);
        }
      cout<<"\n\n";
      // ----------------------------------------

      hdf_particles.FlushHDFFile();
      hdf_particles.close();
    }
  // --------------------------------------------
  // ********************************************



  // ********************************************
  // Pairs properties
  // ********************************************
  // Setup Fields with particle properties
  ParticlesPropFields<Field_t> PairsProps;
  PairsProps.SetupFromConfigGroup(in_energy);
  PairsProps.Initialize(ml.GetFieldLayout(), ml.GetMesh());
  string pairs_prop_filename = "prop_"+ Pairs_namespace::PairsFileName + ".h5";
  PairsProps.SetupOutputFile( pairs_prop_filename );

  // setup Pairs --------------------------------
  Pairs_t pairs(ml.GetParticlesLayout());
  // name of current pairs HDF5 file
  string pairs_filename = ic.InputDirName() + Pairs_namespace::PairsFileName + ".h5";
  cout<<"Working with file \""<<pairs_filename<<"\"...\n";
  // open Particles file
  Save2HDF hdf_pairs;
  hdf_pairs.open( pairs_filename.c_str() );

  // n_timeshots, time, timesteps ------------
  n_timeshots = tt.GetNumbeOfTimeshots(hdf_pairs);
  time        = tt.GetTimeArray(hdf_pairs);
  timesteps   = tt.GetTimestepArray(hdf_pairs);
 
  // ----------------------------------------
  // iterate over timeshots
  // ----------------------------------------
  cout<<"Number of Timeshots: "<<n_timeshots<<"\n";
  cout<<"Reading: ";
  for ( int it=1; it<=n_timeshots; it++)
    {
      cout<<it<<" "<<flush;
      // read particles 
      pairs.ReadFromHDFFile(it, hdf_pairs);
      // calculate particle number density n(x)
      PairsProps.Scatter_N_Pairs( pairs, time[it-1]);
      // save dataset with n(x)
      PairsProps.SaveToHDFFile(time[it-1], timesteps[it-1]);
    }
  cout<<"\n\n";
  // ----------------------------------------
  hdf_pairs.FlushHDFFile();
  hdf_pairs.close();
  // ********************************************



  // --------------------------------
  // Finalize POOMA 
  // --------------------------------
  Pooma::finalize();

  return 0;
}









