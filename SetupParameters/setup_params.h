#ifndef SETUP_PARAMS_H
#define SETUP_PARAMS_H

#include <iostream>
#include <string>
#include "hdf5.h"
#include "ATbase.h"


/**
 * \defgroup setup_params_grp Setup Parameters  
 *
 * this group contains parameters read from .config file
 */


/**
 *  \ingroup setup_params_grp 
 *
 *  \class SetupParams
 *  \brief Base Class for classes with initialization parameters
 *
 *  All subclasses should store parameters as static memebrs in order
 *  they can be used everywhwre withoud additional initialization
 *
 *  Contains virtual functions for
 *  - Read parameters from config file
 *  - Saving aparmeters into HDF file (not yet)
 *  - Print parameters into an output stream
 */
class SetupParams
{
public:

  //! Read data from config file
  virtual void SetupFromConfig(FileInput &in) = 0;

  //! Print physical paramneters
  virtual std::ostream& Print(std::ostream& s) const { return s; };
};



inline std::ostream& operator<< ( std::ostream& s, const SetupParams& init )
{
  return init.Print(s);
};

#endif
