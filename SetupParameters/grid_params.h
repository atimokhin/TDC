#ifndef GRID_PARAMS_H
#define GRID_PARAMS_H

#include <iostream>
#include <string>
#include <vector>

#include "hdf5.h"
#include "ATbase.h"

#include "setup_params.h"


/**
 *  \ingroup setup_params_grp 
 *
 *  \class GridParams
 *  \brief parameters of the numerical grid. 
 *
 *  This class is being used in setting up the Mesh in Cascade class
 *
 */
class GridParams: public SetupParams
{
public:
  //! number of <B> Cells </B>
  int NumberOfCells() const { return _NX; };
  
  //! Setup data from config file
  void SetupFromConfig(FileInput &in);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;


protected:

  //! Number of cells
  static int _NX;    
};

#endif







