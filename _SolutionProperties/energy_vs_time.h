#ifndef ENERGY_VS_TIME_H
#define ENERGY_VS_TIME_H

#include <list>

#include "../PhysicsLib/small_functions.h"

#include "QuantitiesVsTime.h"


/**
 * \ingroup solution_properties_grp
 *
 * Container for energy (particles, fields...)
 *
 * - energy
 *
 */
class EnergyVsTime: public QuantitiesVsTime
{
public:

  //! Constructor
  EnergyVsTime(int chunksize = 10);
  //! Destructor
  ~EnergyVsTime();

  //! setup from config file group and read data from pervious calculations
  void SetupFromHDFFileAndConfigGroup(FileInput& in);

  //! Save each element into an HDF file 
  bool SaveToHDFFile();

  void UpdateEnergy(double e, double t);

  //! print content 
  void Print(ostream& o = std::cout) const;

public:

  std::deque<double> Energy;
};


/**
 * \ingroup qvt_group
 *
 * names for HDF datasets associated with QuantitiesVsTime
 * 
 */
namespace EnergyVsTime_names{

  using namespace std;

  const string ENERGY__DATASET_NAME = "Energy";
};




#endif
