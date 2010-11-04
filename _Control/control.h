#ifndef CONTROL_H
#define CONTROL_H

#include <iostream>
#include <string>
#include "hdf5.h"

#include "ATbase.h"


/**
 *  \defgroup control_grp Control Classes  
 */


/**
 *  \ingroup control_grp 
 *
 *  \class Control
 *  \brief Base Class for classes with control parameters
 *
 *  All subclasses should store parameters as static members so that
 *  they can be used everywhwre withoud additional initialization
 *
 *  Contains virtual functions for
 *  - Reading parameters from command line
 *  - Reading parameters from config file
 *  - Initialization of control classes
 *  - Saving aparmeters into HDF file (not yet)
 *  - Printing parameters into an output stream
 */
class Control
{
public:

  //! Read data from config file
  virtual void SetupFromConfig(FileInput &in) = 0;

  //! Read data from the command line
  virtual void SetupFromCommandLine(int argc, char *argv[]) {};
  //! Initialize control class
  virtual void Initialize() {};

  bool AskedForHelp() const {return _AskedForHelp; };

  //! Print physical paramneters
  virtual std::ostream& Print(std::ostream& s) const { return s; };

protected:
  static bool _AskedForHelp;
};



inline std::ostream& operator<< ( std::ostream& s, const Control& init )
{
  return init.Print(s);
};

#endif
