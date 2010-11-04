#ifndef GRID_PROPS
#define GRID_PROPS

#include <iostream>
#include <string>

#include "setup_props.h"


#include "../_Mesh/my_grid.h"

/**
 * \ingroup setup_properties_grp 
 * \class GridProps
 *
 * Properties of the numerical grid
 *
 * following SetupParametrs are requied:
 * - TimeParams
 * - GeometryParams
 */
class GridProps: public SetupProps
{
public:

  void Initialize();

  void SaveToHDFFile(Save2HDF& hdf) const;
  void ReadFromHDFFile(Save2HDF& hdf);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

private:
  static double _dT;
  static int    _N_Timesteps;
  
  static double _L;
  static double _L_cm;
  static double _dX;
  static int    _NCells;
};


inline std::ostream& operator<< (std::ostream& s, const GridProps& gp)
{
  return gp.Print(s);
};

#endif
