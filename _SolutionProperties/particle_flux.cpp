#include "particle_flux.h"

#include "../utils/stl_utils.h"

//! Constructor
ParticleFlux::ParticleFlux(int chunksize):
  QuantitiesVsTime(chunksize)
{};

//! Destructor
ParticleFlux::~ParticleFlux() {};



/**
 * Saves EnergyFlux and J into HDF file and then calls
 * QuantitiesVsTime::SaveToHDFFile() for saving Time related
 * information 
 *
 */
bool ParticleFlux::SaveToHDFFile()
{
  using namespace ParticleFlux_names; 

  SetupIStart();

  // save ParticleFlux **********************************
  _DequeInout.Save2HDFFile(N_FLUX__DATASET_NAME, 
			   GetIStart(), GetIStartDataset(), GetNWrite(),
			   NFlux, 
			   IsHDFFileInitialized(),
			   _HDF);

  // save EnergyFlux **********************************
  _DequeInout.Save2HDFFile(ENERGY_FLUX__DATASET_NAME, 
			   GetIStart(), GetIStartDataset(), GetNWrite(),
			   EnergyFlux, 
			   IsHDFFileInitialized(),
			   _HDF);

  // save Current **********************************
  _DequeInout.Save2HDFFile(J__DATASET_NAME, 
			   GetIStart(), GetIStartDataset(), GetNWrite(),
			   J, 
			   IsHDFFileInitialized(),
			   _HDF);


  //- Saves TimeArray, Tcurrent and IStartDataset to HDF file
  //- updates Tcurrent, IStartDataset 
  QuantitiesVsTime::SaveToHDFFile();

  return true;
};



void ParticleFlux::SetupFromConfigGroup(FileInput& in)
{
  QuantitiesVsTime::SetupFromConfigGroup(in);
}


/**
 * This methos is called <b> before </b> all operations in the child
 * class
 * 
 */
void ParticleFlux::SetupFromHDFFileAndConfigGroup(FileInput& in)
{
  using namespace ParticleFlux_names; 

  // read data from config file
  SetupFromConfigGroup(in);

  // read data from HDF file
  Save2HDF input_hdf;
  OpenAndDoSetupFromHDFFile(input_hdf);


  _DequeInout.ReadFromHDFFile(N_FLUX__DATASET_NAME,
			      GetSize(),
			      NFlux,
			      input_hdf);

  _DequeInout.ReadFromHDFFile(ENERGY_FLUX__DATASET_NAME,
			      GetSize(),
			      EnergyFlux,
			      input_hdf);

  _DequeInout.ReadFromHDFFile(J__DATASET_NAME,
			      GetSize(),
			      J,
			      input_hdf);
}


/**
 * print content 
 * 
 */
void ParticleFlux::Print(ostream& o) const
{
  QuantitiesVsTime::Print(o);

  o<<"N = ";
  PrintSTLContainer(o,NFlux);
  o<<"Energy = ";
  PrintSTLContainer(o,EnergyFlux);
  o<<"J = ";
  PrintSTLContainer(o,J);
  o<<"\n";
}
