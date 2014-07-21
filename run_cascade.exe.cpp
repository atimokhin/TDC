/**
 *  \file  run_cascade.exe.cpp
 *  \brief Main file for cascade simulation
 */
//#include <mcheck.h>

#include <iostream>
#include "argtable2.h"

#include "ATbase.h"

#include "utils/timer.h"
#include "utils/pid.h"
#include "utils/do_print_usage_info.h"

#include "stop_and_save/stop_and_save.h"
#include "stop_and_save/watcher_file.h"
#include "stop_and_save/watcher_signal.h"

#include "SetupDimensionalConstants/dimensional_consts_setup.h"
#include "SetupParameters/params_list.h"
#include "SetupProperties/setup_props_list.h"

#include "_Control/control_list.h"
#include "_Control/output_control.h"

#include "cascade.h"



//! Main function for cascade simulation
int main(int argc, char *argv[])
{

  // mtrace(); /* Starts the recording of memory allocations and releases */

  // start timer ++++++++++++++++++++
  Timer timer;
  timer.Start();

  // input file <<<<<<<<<<<<<<<<<<<<<
  FileInput in;
  in.ReadFile( InOut::SetupFile );

  // setup controls <<<<<<<<<<<<<<<<<
  ControlList controls;
  controls.SetupFromCommandLine(argc, argv);
  controls.SetupFromConfig(in);
  controls.Initialize();
  // stop if help info if asked +++++++
  if ( controls.AskedForHelp() )
    exit(0);


  // setup dimensional constants <<<<
  DimensionalConstsSetup consts;
  consts.SetupFromConfig(in);

  // setup parameters <<<<<<<<<<<<<<<
  ParameterList params;
  params.SetupFromConfig(in);

  // setup properties <<<<<<<<<<<<<<<
  SetupPropsList props;
  props.Initialize(in);

  // save info to log file ----------
  OutputControl out;
  out.LogStream()<<"\n==> Calculation Started  @ "<<timer;
  out.LogStream()<<controls<<std::flush;
  out.LogStream()<<params<<std::flush;
  out.LogStream()<<props<<std::flush;

  std::cout<<out;
  // --------------------------------

  // save PID to pid file -----------
  PID process_id;
  process_id.SavePIDToFile(out.OutputDirName());

  // save properties to HDF file ----
  props.SaveToHDFFile();

  // setup StopAndSave --------------
  WatcherSignal ws;
  WatcherFile   wf(out.OutputDirName());
  StopAndSave stop;
  stop.AddWatcher( &ws );
  stop.AddWatcher( &wf );
  // --------------------------------


  // --------------------------------
  // Initialize POOMA 
  // --------------------------------
  Pooma::initialize(argc, argv);

  // --------------------------------
  // Cascade Simulation 
  // --------------------------------
  Cascade cascade;
  // setup
  cascade.SetupFromConfig(in);
  // run simulations
  cascade.RunSimulations();

  // --------------------------------
  // Finalize POOMA 
  // --------------------------------
  Pooma::finalize();


  // ++++++++++++++++++++++++++++++++
  // stop timer and save timing info
  // ++++++++++++++++++++++++++++++++
  timer.Stop();
  // log file
  out.LogStream()<<"\n==> Calculation Finished @ "<<timer;
  timer.PrintElapsedTime(out.LogStream());
  // cout
  std::cout<<"\n==> Calculation Finished @ "<<timer;
  timer.PrintElapsedTime(std::cout);

 
  return 0;
} 
