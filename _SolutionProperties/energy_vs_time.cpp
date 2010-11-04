#include "energy_vs_time.h"

#include "../utils/stl_utils.h"

#include <iostream>

//! Constructor
EnergyVsTime::EnergyVsTime(int chunksize):
  QuantitiesVsTime(chunksize)
{};

//! Destructor
EnergyVsTime::~EnergyVsTime() {};


void EnergyVsTime::UpdateEnergy(double e, double t)
{
  // strore Energy
  if ( GetCurrentTime() == t && Energy.size() > 0 )
    {
      Energy[GetICurrent()] += e;
    }
  else
    {
      AddNewValue(TimeArray,t);
      SetCurrentTime(t);      

      AddNewValue(Energy, e);
    }
}


/**
 * Saves Energy into HDF file and then calls
 * QuantitiesVsTime::SaveToHDFFile() for saving Time related
 * information 
 *
 */
bool EnergyVsTime::SaveToHDFFile()
{
  using namespace EnergyVsTime_names; 

  SetupIStart();

  Print(std::cout);  

  // save EnergyFlux **********************************
  _DequeInout.Save2HDFFile(ENERGY__DATASET_NAME, 
			   GetIStart(), GetIStartDataset(), GetNWrite(),
			   Energy, 
			   IsHDFFileInitialized(),
			   _HDF);


  //- Saves TimeArray, Tcurrent and IStartDataset to HDF file
  //- updates Tcurrent, IStartDataset 
  QuantitiesVsTime::SaveToHDFFile();

  return true;
};




/**
 * This methos is called <b> before </b> all operations in the child
 * class
 * 
 */
void EnergyVsTime::SetupFromHDFFileAndConfigGroup(FileInput& in)
{
  using namespace EnergyVsTime_names; 

  // read data from config file
  SetupFromConfigGroup(in);

  // read data from HDF file
  Save2HDF input_hdf;
  OpenAndDoSetupFromHDFFile(input_hdf);

  _DequeInout.ReadFromHDFFile(ENERGY__DATASET_NAME,
			      GetSize(),
			      Energy,
			      input_hdf);

}


/**
 * print content 
 * 
 */
void EnergyVsTime::Print(ostream& o) const
{
  QuantitiesVsTime::Print(o);

  o<<"Energy = ";
  PrintSTLContainer(o,Energy);
  o<<"\n";
}
