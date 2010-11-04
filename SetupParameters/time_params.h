#ifndef TIME_PARAMS_H
#define TIME_PARAMS_H

#include <iostream>
#include <string>
#include "hdf5.h"
#include "ATbase.h"

#include "setup_params.h"


/**
 *  \ingroup setup_params_grp 
 *
 *  \class TimeParams
 *  \brief parameters of the numerical scheme rearding timesteps
 *
 */
class TimeParams: public SetupParams
{
public:
  //! Terurn timestep
  double dt() const { return _DT; };
  //! return number of timesteps
  int    NumberOfSteps() const { return _N; };

  
  //! Setup data from config file
  void SetupFromConfig(FileInput &in);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

protected:

  static double _DT;    //! Timestep
  static int    _N;     //! Number of timesteps
};

#endif
