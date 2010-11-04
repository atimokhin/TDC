#ifndef INPUT_CONTROL_H
#define INPUT_CONTROL_H

#include <iostream>
#include <iomanip>
#include <string>

#include "ATbase.h"

#include "control.h"
#include "directories.h"



/**
 *  \ingroup control_grp 
 *
 *  \class InputControl
 *
 */
class InputControl: public Control, private Directories
{
public:

  //! Setup control parameters from command line
  void SetupFromCommandLine(int argc, char *argv[]);
  //! Setup control parameters from config file
  void SetupFromConfig(FileInput &in);

  //! full name of the <b> config file </b> to be read
  std::string ConfigFileName() const;
  //! full name of the directory with data to be read
  std::string InputDirName() const;
  

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

private:

  //! diplay help on program usage
  void DisplayHelp(const char* prog_name, void* argtable[]);

private:

  static std::string _ID;             //!< Calculation ID
  static std::string _ConfigFileName; //! FULL(!) name of the config file 
};



//! get results dirertory name
inline string InputControl::InputDirName() const 
{ return ResultsDir() + "/" + _ID + "/"; };

//! get results dirertory name
inline string InputControl::ConfigFileName() const 
{ return _ConfigFileName; };


#endif


