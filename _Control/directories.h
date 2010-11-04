#ifndef DIRECTORIES_H
#define DIRECTORIES_H

#include <iostream>
#include <iomanip>
#include <string>

#include "ATbase.h"

#include "control.h"



/**
 *  \ingroup control_grp 
 *
 *  \class Directories
 *  Class wich sets up results dir
 *  - either from command line
 *  - or uses standard value InOut::ResultsDir
 */
class Directories
{
public:

  //! Setup data from command line
  void SetupFromCommandLine(int argc, char *argv[]);

  //! get results dirertory name
  string ResultsDir() const;

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;


private:

  static string _ResultsDir;  //!< Dirertory with calculation results
};



//! get results dirertory name
inline string Directories::ResultsDir() const 
{ return _ResultsDir; };


#endif


