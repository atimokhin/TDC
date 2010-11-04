#ifndef START_CONTROL_H
#define START_CONTROL_H

#include <iostream>
#include <string>
#include "ATbase.h"

#include "control.h"
#include "directories.h"


/**
 *  \ingroup CONTROL_grp 
 *
 *  \class StartControl
 *
 *  Class controlling start of the current calculation.
 *  - whether to restart calculation
 *  - if yes, where are saved results of calculations to be restarted 
 *  - from which timestep calculation should be restarted
 */
class StartControl : public Control, private Directories
{
public:

  //! Setup data from command line
  void SetupFromCommandLine(int argc, char *argv[]);
  //! Setup data from config file
  void SetupFromConfig(FileInput &in);

  //! where are saved results of calculations to be restarted
  std::string InputDirName() const;

  //! whether to restart calculation
  bool RestartCalculations() const;  

  //! from which timestep calculation should be restarted 
  int StartFromTimeshot() const;

  //! Print Calculation paramneters
  std::ostream& Print(std::ostream& s) const;

private:

  static bool        _RestartCalculations;
  static std::string _ID;
  static int         _StartFromStep; 
};



inline  bool StartControl::RestartCalculations() const 
{ return _RestartCalculations; };

inline std::string StartControl::InputDirName() const 
{ return ResultsDir() + "/" + _ID + "/"; };

inline int StartControl::StartFromTimeshot() const 
{ return _StartFromStep; };

#endif


