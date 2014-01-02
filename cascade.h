#ifndef CASCADE_H
#define CASCADE_H


#include <iostream>

#include "ATbase.h"

#include "utils/io_filenames.h"


#include "_Particles/charged.h"
#include "_Particles/pairs.h"
#include "_Particles/particle_list.h"
#include "_Particles/BC/tdc_particles_bc.h"
#include "_Particles/LPT/large_particles_tools.h"
#include "_Particles/ID/particle_id.h"

#include "_Fields/em_fields.h"
#include "_Environment/RhoGJ/rho_gj.h"
#include "_Environment/MagneticField/magnetic_field.h"

#include "_SolutionProperties/particle_flux.h"

#include "_PIC/pic.h"

#include "_MC/monte_carlo.h"

#include "_Control/code_control.h"
#include "_Control/output_control.h"
#include "_Control/start_control.h"




/**
 * \defgroup main_grp Global routines
 * 
 */


/**
 * \ingroup main_grp
 * 
 * \class Cascade
 *
 * Class responsible for doing cascade simulations.
 * All Compiler-time settings are here.
 * such as:
 *
 * - POOMA type setups
 * - PIC interpolator 
 * - Backgroung magnetic field
 *
 */
class Cascade
{
  // Typedefs -----------------------------------

  // Mesh type and Point type **************
  #define DIM 1
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
  typedef EMFields<Field_t>           EM_t;
  typedef ParticleList<CParticles_t>  PL_t;

  // ---------------------------------------------

public:

  //! Constructor
  Cascade();
  //! Destructor
  ~Cascade();

  //! main routine
  void RunSimulations();

  //! Setup computation problem from comfig file
  void SetupFromConfig(FileInput& in);


private:

  //! setups quantities for the first step (at t=-dt/2)
  void Leapfrog_HalfStepBack(double dt);

  //! save calculation data to HDF files at time t
  void SaveToHDFFile(double t, int it);
  //! flush all HDF files with calculation data 
  void FlushHDFFile();
  //! save calculation status to logfile
  string CurrentStateInfo(int iteration, double t);

  //! print event information to sdout and logfile
  void LogEvent(int iteration, string message);

private:

  //! Physical ingredients classes @{


  //! Particle list
  PL_t     _PList;
  //! Virtual Pairs
  Pairs_t* _pPairs;

  //! Electromagnetic fields
  EM_t*  _pEM;

  RhoGJFunctor*  __p_rhogj_functor;
  MagneticField* __p_magnetic_field;
  //!@}


  //! Action classes @{ 
  
  //! PIC class
  ParticleInCell<EM_t, PL_t::Particles_t> _PIC; 
  //! Monte Carlo
  MonteCarlo _MC;

  //! Particle Boundary Conditions class
  TDC_ParticlesBC<EM_t,CParticles_t>* _pBC; 

  //! Particle tools
  LargeParticlesTools _LPT;

  //!@}

  ParticleID _ID;

  //! flux of particles hitting NS
  ParticleFlux _NSFlux;
  //! flux of particles leaving cascade zone
  ParticleFlux _LCFlux;

  //! claculation start time
  double _T0;

  //! output control
  OutputControl __output;
  //! Calculation start control
  StartControl  __start;
  //! Code control
  CodeControl   __code;
};


#endif
