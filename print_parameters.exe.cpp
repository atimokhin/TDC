/**
 *  \ingroup test_grp
 *
 *  \file print_physical_parameters.exe.cpp 
 *
 *  \brief file for printing out physical parameters accociated with *
 *  the *partuicular calculation
 */
#include <iostream>
#include <cstdio>

#include "ATbase.h"

#include "utils/do_print_usage_info.h"

#include "_Control/code_control.h"
#include "_Control/input_control.h"

#include "SetupDimensionalConstants/dimensional_consts_setup.h"

#include "SetupParameters/params_list.h"

#include "SetupProperties/setup_props_list.h"
#include "SetupProperties/RS_cascade_props.h"



/** 
 * Main function for setup parameter print
 * 
 * @param argc 
 * @param argv 
 * 
 * @return 
 */
int main(int argc, char *argv[])
{
  InputControl ic;
  ic.SetupFromCommandLine(argc, argv);

  if ( ic.AskedForHelp() )
    exit(0);

  FileInput in;
  in.ReadFile( ic.ConfigFileName() );
  ic.SetupFromConfig(in);
    

  // --------------------------------
  // setup parameters from input file
  // --------------------------------
  ParameterList params;
  params.SetupFromConfig(in);

  DimensionalConstsSetup consts;
  consts.SetupFromConfig(in);

  CodeControl  cc;
  cc.SetupFromConfig(in);
  std::cout<<cc;

  SetupPropsList props;
  props.Initialize();
  std::cout<<props;  

  return 0; 
}




