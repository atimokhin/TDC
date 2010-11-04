#include "cascade.h"

#include <sstream>

#include "utils/formats.h"
#include "stop_and_save/stop_and_save.h"

#include "SetupParameters/time_params.h"
#include "SetupParameters/grid_params.h"
#include "SetupParameters/particles_params.h"
#include "SetupParameters/geometry_params.h"

#include "_Mesh/MeshAndLayouts.h"

#include "_Environment/MagneticField/magnetic_field_maker.h"

#include "_Environment/RhoGJ/rho_gj_maker.h"
#include "_Fields/em_fields_maker.h"

#include "_Particles/BC/tdc_particles_bc_maker.h"
#include "_Particles/ID/particle_id.h"


/**
 * Constructor. (empty)
 */
Cascade::Cascade()
{
};


/**
 * Destructor. delete all particles 
 */
Cascade::~Cascade()
{
  delete _pEM;
  delete _pPairs;
  _PList.FreeMemory();
};


/**
 * \fn void Cascade::SetupFromConfig(FileInput& in)
 * 
 * This function sets up all ingredients of calculations. 
 * It defines all relevant layouts and sets up
 * mesh, electromagnetic field and particle list objects.
 *
 * Order of initialization:
 *
 * - Mesh, Field- and Particles- Layouts via instance of MeshAndLayout class
 * 
 * - Fields:
 *
 * - ParticleLists: 
 *   - reads NumberOfChargedParticleTypes
 *   - adds  reqested number of Charged classes instances to the 
 *        internal particle list Plist
 *   - initializes Particles by calling Plist.SetupFromConfigGroup(in)
 *   .
 *   configuration file example
     \verbatim
       Group "PARTICLES" { 
          ..... 
          NumberOfChargedParticleTypes = 1;
          ChargedParticles_#0 = 'Electrons';
          Group "Electrons" {
          .....
          }
      }
     \endverbatim
 * .
 *
 * @param in FileInput object
 *
 */
void Cascade::SetupFromConfig(FileInput& in)
{
  // ********************************************
  // Setup start time and particle numbering class
  // ********************************************
  int start_timestep_number;
  if ( __start.RestartCalculations() )
    {
      // HDF file object
      string tt_filename = __start.InputDirName() + Pairs_namespace::PairsFileName +".h5";
      Save2HDF hdf;
      hdf.open( tt_filename.c_str() );
      int start_timeshot = __start.StartFromTimeshot();			      

      Timetable tt;
      _T0 = tt.GetTimeOfTheTimeshot(hdf, start_timeshot );
      start_timestep_number = tt.GetTimestepOfTheTimeshot(hdf, start_timeshot );
      hdf.close();
     }
   else
     {
        _T0 = 0;
        start_timestep_number = 0;
     }

  // set start timestep number for all timetables
  Timetable::SetStartTimestep(start_timestep_number);

  // set start timestep number for particle identification
  _ID.SetStartTimestep(start_timestep_number);
  _ID.NewTimestep(0);
  // ********************************************


  // ********************************************
  // Setup Mesh *********************************
  // ********************************************
  MeshAndLayouts<Mesh_t,Field_t,PTraits_t> ml;
  ml.SetupFromConfig(in);
  ml.SaveToHDFFile();




  // ********************************************
  // Setup Particles ****************************
  // ********************************************
  in.ChangeGroup("PARTICLES");
  
  // create and fill particle list --------------
  int n_part = static_cast<int>(in.get_param("NumberOfChargedParticleTypes"));
  for ( int i=0; i<n_part; i++)
    {
      string name = in.get_name("ChargedParticles",i);
      _PList.Add(name, new CParticles_t(ml.GetParticlesLayout()));
    };
  _PList.SetupOutputFiles();

  // setup individual particles------------------
  if ( __start.RestartCalculations() )
    // Reads particle parameters from HDF file
    _PList.SetupFromHDFFilesAndConfigGroups(in);
  else
    // Setup particles parameters from config file
    _PList.SetupFromConfigGroups(in,_ID);

  _PList.Swap();
	  
  // Setup Particles Boundary Conditions ========
  in.ChangeGroup("BoundaryConditions");
  string bc_name = in.get_name("Type");
  _pBC= TDC_ParticlesBC_Maker<EM_t,CParticles_t>(in).pMake(bc_name);

  _pBC->SetupFromConfigGroup(in);
  _pBC->SetupGeneralBC(_PList);

  in.ChangeGroup();  
  // ********************************************


  // ********************************************
  // Setup Pairs ********************************
  // ********************************************
  in.ChangeGroup("PAIRS");

  _pPairs = new Pairs_t(ml.GetParticlesLayout());
  _pPairs->SetupOutputFile();

  if ( __start.RestartCalculations() )
    // Reads particle parameters from HDF file
    _pPairs->SetupFromHDFFileAndConfigGroup(in);
  else
    // Setup particles parameters from config file
    _pPairs->SetupFromConfigGroup(in,_ID);

  in.ChangeGroup();
  // ********************************************



  // ********************************************
  // Setup Particle Fluxes **********************
  // ********************************************
  in.ChangeGroup("NS_PARTICLE_FLUX");
  if ( __start.RestartCalculations() )
    _NSFlux.SetupFromHDFFileAndConfigGroup(in);
  else
    _NSFlux.SetupFromConfigGroup(in);   
  in.ChangeGroup();
  _NSFlux.SetupOutputFile();
  

  in.ChangeGroup("LC_PARTICLE_FLUX");
  if ( __start.RestartCalculations() )
    _LCFlux.SetupFromHDFFileAndConfigGroup(in);
  else
    _LCFlux.SetupFromConfigGroup(in);
  in.ChangeGroup();
  _LCFlux.SetupOutputFile();
  // ********************************************


  // ********************************************
  // Setup GJ charge density ********************
  // ********************************************
  in.ChangeGroup("RHO_GJ");

  string rhogj_functor_name = in.get_name("Type");
  RhoGJFunctor* p_rhogj_functor = RhoGJFunctorMaker(in).pMake(rhogj_functor_name); 
  p_rhogj_functor->SetupFromConfigGroup(in);

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
  // ********************************************


  // ********************************************
  // Setup Electromagnetic fields ***************
  // ********************************************
  in.ChangeGroup("ELECTRODYNAMICS");

  string em_name = in.get_name("Type");
  _pEM = EMFieldsMaker<Field_t>().pMake(em_name);
  _pEM->SetRhoGJFunctor(p_rhogj_functor);
  _pEM->Initialize(ml.GetFieldLayout(), ml.GetMesh());
  _pEM->SetupOutputFile();

  if ( __start.RestartCalculations() )
    // Reads fields  from HDF file
    _pEM->SetupFromHDFFilesAndConfigGroup(in);
  else
    // Setup fields from config file
    _pEM->SetupFromConfigGroup(in);

  in.ChangeGroup();
  // ********************************************

  // ********************************************
  // Setup PIC  *********************************
  // ********************************************
  _PIC.SetupFromConfig(in);

  // ********************************************
  // ********************************************
  // Setup Monte Carlo **************************
  // ********************************************
  if ( __code.DoMonteCarlo() ) 
    {
      _MC.SetupFromConfig(in);
      _MC.SetMagneticField(p_magnetic_field);
    }

  // ********************************************
  // Setup Large Particles Tools ****************
  // ********************************************
  if ( __code.DoLPT() ) _LPT.SetupFromConfig(in);

};



/** 
 * Main routine - performs simulations
 * 
 */
void Cascade::RunSimulations()
{
  StopAndSave       stop;

  TimeParams        time;

  double t  = _T0;
  double dt = time.dt();

 
  // ************************************
  // <<Start>> initial setup (if this is a fresh start)
  // ************************************
  if ( !__start.RestartCalculations() )
    {
      // save initial particle distribution
      _PList.SaveToHDFFile(t, 0);
      _pPairs->SaveToHDFFile(t, 0);

      // Setup firts step of the leapfrog schema
      Leapfrog_HalfStepBack(dt);

      // save initial electromagnetic fields 
      _pEM->SaveToHDFFile(t, 0);
    }
  // ************************************




  // Main timestep loop ++++++++++++++++++++
  for (int it=1; it <= time.NumberOfSteps(); ++it)
    {
      // ************************************
      // <Control> time advance
      // ************************************
      // advance time
      t+=dt;
      // set timestep for id
      _ID.NewTimestep(it);


      // ************************************
      // <Log> print info to stdout and log file
      // ************************************
      // current status info -> log file
      if ( __output.DoLogFileOutput(it) || it == time.NumberOfSteps() ) 
	{
	  __output.LogStream()<<CurrentStateInfo(it, t)<<std::flush;
	}
      // current status info -> stdout
      if ( __output.DoStdoutOutput(it) ) 
	{
	  std::cout<<CurrentStateInfo(it, t)<<std::flush;
	}
      // ************************************


      // ************************************
      // <<PIC>> 
      // ************************************
      // Move particles
      if ( __code.DoMoveParticles() ) 
	{
	  _PIC.MoveAndScatterParticles(_PList,*_pEM, _NSFlux,_LCFlux, t,dt);
	}
      // Solve field eqiations
      if ( __code.DoMaxwell() ) 
	{
	  _PIC.SolveFieldEquations(*_pEM, t,dt);
	}
      // ************************************


      // ************************************
      // <MC> Inject pairs 
      // ************************************
      if ( __code.DoInjectPairs() ) 
	{
	  _MC.Pairs2Particles(*_pPairs,_PList, t,dt);
	}

      // ************************************
      // <BC> Apply time-dependent boundary conditions
      // ************************************
      _pBC->ApplyTimeDependentBC(*_pEM,_PList, t,dt);
      

      // ************************************
      // <LPT> Adjust number of particles 
      // ************************************
      if ( __code.DoLPT(t) ) 
	{
	  if ( _LPT.AdjustNumberOfParticles(_PList) )
	    {
	      _PIC.EnforceGaussLaw(_PList,*_pEM, t,dt);
	      LogEvent(it,"Particle number adjusted");
	    }
	}

      // ************************************
      // <<MC>>  Create pairs 
      // ************************************
      if ( __code.DoCreatePairs() ) 
	{
	  _MC.Particles2Pairs(_PList,*_pPairs, t,dt,_ID);
	}


      // ************************************
      // <LPT> Adjust number of pairs and emitting particles
      // ************************************
      if ( __code.DoLPT(t) ) 
	{
	  if ( _LPT.AdjustNumberOfParticles(*_pPairs) )  
	    {
	      LogEvent(it,"Pairs and Leptons numbers adjusted");
	      _LPT.AdjustNumberOfParticlesForPairOverproduction(_PList);
	      _PIC.EnforceGaussLaw(_PList,*_pEM, t,dt);
	    }
	}


      // ************************************
      // <Output> Save results to HDF file 
      // ************************************
      if ( __output.DoSaveToHDFFiles(it) ) this->SaveToHDFFile(t, it);


      // ************************************
      // <Control> Pause program to see results?
      // ************************************
      // Watch for stop requests
      stop.Watch();
       if ( stop.Asked_Pause() )
         {
	   LogEvent(it," Computations Paused! <<<<<<<<<<<");
           // flush data in HDF file
           FlushHDFFile();
	   LogEvent(it," HDF Files are ready to be seen <========");
           // wait untill RESUME request is received
           while ( stop.Asked_Pause() ) stop.Watch(); 
	   LogEvent(it," Computations Resumed! <<<<<<<<<<");
         } 

      
      // ************************************
      // <Control> Interrupt calculations? 
      // ************************************
       if ( stop.Asked_Stop() ) 
	 {
           if ( stop.Asked_SaveCurrentState() )
             {
               // if results are not saved -- save them
               if ( !__output.DoSaveToHDFFiles(it) ) this->SaveToHDFFile(t, it);
               // write notice about interupted calculations
               LogEvent(it," Results at this step are saved! <-------");
             }
	   // write notice about interupted calculations
	   LogEvent(it," Computations Interupted! <<<<<<<");
	   // itrerrupt the loop
	   break;
	 }
    }
  // +++++++++++++++++++++++++++++++++++++++
};



/** 
 * Setups quantity for the first step -- calculates initial
 * electromagnetic fields and momentum at \f$ t =- dt/2 \f$
 * 
 * @param dt first timestep
 */
void Cascade::Leapfrog_HalfStepBack(double dt)
{
  if ( __code.DoGatherRhoAndJ() ) 
    _PIC.ScatterRho(*_pEM,_PList);

  // solve Maxwell equations - calculate **initial** electric field!
  if ( __code.DoMaxwell() ) 
    _PIC.CalculateInitialField(*_pEM, dt);
  
  // Setup initial Momentum
  if ( __code.DoMoveParticles() ) 
    _PIC.CalculateInitialMomentum(_PList,*_pEM,dt);

  // Invoke general particles boundary conditions and update particle distribution
  _PList.Sync();
};




/** 
 * Save particles and field into HDF file at the moment t
 * 
 * @param t time moment
 */
void Cascade::SaveToHDFFile(double t, int it)
{
  // save particles
  _PList.SaveToHDFFile(t, it);
  // save fields 
  _pEM->SaveToHDFFile(t, it);
  // save pairs
  _pPairs->SaveToHDFFile(t, it);

  // save fluxes
  _NSFlux.SaveToHDFFile();
  _LCFlux.SaveToHDFFile();
};


/** 
 * Flush particles and field HDF files
 * 
 */
void Cascade::FlushHDFFile()
{
  // flush particles
  _PList.FlushHDFFile();
  // flush fields
  _pEM->FlushHDFFile();
  // flush pairs
  _pPairs->FlushHDFFile();

  // flush fluxes
  _NSFlux.FlushHDFFile();
  _LCFlux.FlushHDFFile();
}



/** 
 * Print event info to stdout and to the logfile
 * 
 * @param iteration iteration number
 * @param message   message to be printed
 */
void Cascade::LogEvent(int iteration, string message)
{
  std::ostringstream ostr_message;

  ostr_message<<"--> Step# "<<iteration<<" "<<message<<"\n";
  __output.LogStream()<<ostr_message.str()<<std::flush;
  std::cout<<ostr_message.str()<<std::flush;
}


/** 
 * Form a string with information about current calculation status
 * 
 * @param t time moment
 */
string Cascade::CurrentStateInfo(int iteration, double t)
{  
  GeometryParams geom;

  std::ostringstream ostr_message;
  ostr_message<<" Step# "<<setw(5)<< iteration;
  ostr_message<<" T<fb>="<<setw(9)<< geom.FlybyTime(t) <<"  ";
  ostr_message<<" **Particles: "<<fmt::short_format<<_PList;
  ostr_message<<" **Pairs "<<fmt::short_format<<*_pPairs;
  ostr_message<<std::endl<<std::flush;

  return ostr_message.str();
};

