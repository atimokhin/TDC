/**
 * \file   io_filenames.h
 * \author  <atim@sai.msu.ru>
 * 
 * \brief  Contain the name of the directory with results of calculations
 */
#ifndef IO_FILENAMES_H
#define IO_FILENAMES_H

#include <string>


/**
 * \name InOut
 * 
 * This namespace contains names of various files usen in input/output
 * process
 */
namespace InOut {

  using namespace std;

  //! Directory where calculation results will be stored
  //! relative to the <B> solve </B> direcory
  const string ResultsDir = "../RESULTS/";

  //! Read configuration of equation from this file
  const string SetupFile  = "cascade.input";

  //! Read configuration of equation from this file
  const string CompilerOptsFile = "COMPILER_OPTS";

  //! Calculation logs are written to this file
  const string LogFile  = "cascade.log";


  //! Write error map to this file (if required)
  const string ErrorFile = "error.h5";



  //! Write properties (mesh, init pars) to this file
  const string MeshFile = "mesh.h5";

  //! Write properties (mesh, init pars) to this file
  const string SetupPropertiesFile = "setup_properties.h5";

  //! Electromagnetic Field is written to this file
  const string FieldsFile  = "fields.h5";

  //! photons emited by particles is written to this file
  const string PhotonsFile = "pair_photons.h5";
}


/**
 * \name Lock
 * 
 * This namespace contains names of files usen in controlling process
 * (stop/start) calculations
 */
namespace Lock { 

  //! Name of the lock file for stopping calculations
  const std::string StopSolveFile="cascade.STOP";

}

#endif


