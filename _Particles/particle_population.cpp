#include "particle_population.h"



#define MACRO__CODING_TABLE_ENTRIES   \
  MACRO__CODING_TABLE_ENTRY('P')      \
  MACRO__CODING_TABLE_ENTRY('I')      \
  MACRO__CODING_TABLE_ENTRY('C')      \
  MACRO__CODING_TABLE_ENTRY('S')      \
  MACRO__CODING_TABLE_ENTRY('R')      \
  MACRO__CODING_TABLE_ENTRY('N')  


namespace PP{
  using namespace std;
  const string PP_GRP_NAME     = "PP";
  const string PP_NUMBERS_NAME = "Numbers";
  const string PP_WEIGHTS_NAME = "Weights";
  const string PP_MEAN_WEIGHT_NAME  = "MeanWeight";
  const string PP_TABLE_NAME   = "CodingTable";
}


/**
 * \ingroup particles_grp
 * \def  MACRO__CODING_TABLE_ENTRY
 * 
 * \brief this macro is associated with the class ParticlePopulation
 *
 * this macro generates code for 
 *
 * - insertion of the letter code of particle origin and corresponding
 * position in the particle population array into the coding table
 */
#define MACRO__CODING_TABLE_ENTRY(LETTER) _CodingTable[LETTER] = table_index++;	    


ParticlePopulation::ParticlePopulation()
{
  int table_index = 1;

  MACRO__CODING_TABLE_ENTRIES
    
  _TableSize = _CodingTable.size()+1;
  _ParticleNumbers.assign(_TableSize, 0);
  _ParticleWeights.assign(_TableSize, 0);

  _MeanWeight = 0;
}


ParticlePopulation::~ParticlePopulation()
{
}


//! Save each element into an HDF file as "name/id"
bool ParticlePopulation::SaveToHDFFile(std::string id, Save2HDF &hdf)
{
  bool status = true;

  hid_t file_id     = hdf.GetFileId();
  hid_t pp_group_id = H5Gopen(file_id, PP::PP_GRP_NAME.c_str());
  // if this is the first dump to file 
  // - create grroup PP_GRP_NAME, PP_NUMBERS_NAME, PP::PP_WEIGHT_NAME
  if ( pp_group_id < 0 )
    {
      // create groups
      pp_group_id = H5Gcreate(file_id, PP::PP_GRP_NAME.c_str(), 0);

      hid_t group_particle_numbers_id = H5Gcreate(pp_group_id,  PP::PP_NUMBERS_NAME.c_str(), 0);
      H5Gclose(group_particle_numbers_id);

      hid_t group_weights_id = H5Gcreate(pp_group_id,  PP::PP_WEIGHTS_NAME.c_str(), 0);
      H5Gclose(group_weights_id);

      hid_t group_mean_weight_id = H5Gcreate(pp_group_id,  PP::PP_MEAN_WEIGHT_NAME.c_str(), 0);
      H5Gclose(group_mean_weight_id);
    }
  H5Gclose(pp_group_id);

  // write particle statistics
  std::string numbers_dataset_name = PP::PP_GRP_NAME + "/" + PP::PP_NUMBERS_NAME + "/" + id;
  status = hdf.SaveSTLContainer( numbers_dataset_name,_ParticleNumbers ) && status;

  // write particle statistics
  std::string weights_dataset_name = PP::PP_GRP_NAME + "/" + PP::PP_WEIGHTS_NAME + "/" + id;
  status = hdf.SaveSTLContainer( weights_dataset_name,_ParticleWeights ) && status;

  std::string mean_weight_dataset_name = PP::PP_GRP_NAME + "/" + PP::PP_MEAN_WEIGHT_NAME + "/" + id;
  status = hdf.writeScalar( mean_weight_dataset_name.c_str(), _MeanWeight ) && status;

  return status;
}


bool ParticlePopulation::ReadFromHDFFile(std::string id, Save2HDF &hdf)
{
  bool status = true;

  // read particle statistics
  std::string numbers_dataset_name = PP::PP_GRP_NAME + "/" + PP::PP_NUMBERS_NAME + "/" + id;
  status = hdf.ReadSTLContainer( numbers_dataset_name , _ParticleNumbers ) && status;

  // write particle statistics
  std::string weights_dataset_name = PP::PP_GRP_NAME + "/" + PP::PP_WEIGHTS_NAME + "/" + id;
  status = hdf.ReadSTLContainer( weights_dataset_name, _ParticleWeights ) && status;

  std::string mean_weight_dataset_name = PP::PP_GRP_NAME + "/" + PP::PP_MEAN_WEIGHT_NAME + "/" + id;
  status = hdf.readScalar( mean_weight_dataset_name.c_str(), _MeanWeight ) && status;

  return status;
}
