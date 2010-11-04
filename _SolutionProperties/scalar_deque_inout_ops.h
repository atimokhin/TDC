#ifndef SCALAR_DEQUE_INOUT_OPS_H
#define SCALAR_DEQUE_INOUT_OPS_H

#include <deque>
#include <string>

#include "IO/HDF5.h"
#include "../inout/save2hdf.h"


/**
 * \ingroup solution_properties_grp
 *
 */
class ScalarDequeInoutOps
{
public:
  //! constructor
  ScalarDequeInoutOps(int chunk_size);

  //! save deque to HDF file
  template<class T>
  void Save2HDFFile(std::string name, 
		    int i_start,
		    int i_start_dataset,
		    int n_write, 
		    const std::deque<T>& d, 
		    bool is_initialized, 
		    Save2HDF &hdf);

  //! read deque from HDF file
  template<class T>
  void ReadFromHDFFile(std::string name, 
		       int size, 
		       std::deque<T>& d, 
		       Save2HDF &hdf);
  
private:
  //! Size of the chunk in the dataset storage
  int _ChunkSize;
};







template<class T>
void ScalarDequeInoutOps::Save2HDFFile(std::string name, 
				       int i_start,
				       int i_start_dataset,
				       int n_write, 
				       const std::deque<T>& d, 
				       bool is_initialized, 
				       Save2HDF &hdf)
{
  hid_t file_id;
  hid_t dataspace, dataspace_mem;
  hid_t dataset;

  hsize_t dims[1], maxdims[1];
  hsize_t offset[1], count[1];

  herr_t  status;  

  hid_t datatype = HDF5TypeTraits<T>::type();

  file_id = hdf.GetFileId();

  if ( is_initialized ) // there are data already written in HDF file
    {
      dataset   = H5Dopen(file_id, name.c_str());
      dataspace = H5Dget_space(dataset);
      H5Sget_simple_extent_dims(dataspace, dims, maxdims);
    }
  else // create dataset in HDF file
    {
      // Create the data space with unlimited dimensions. 
      dims[0]    = 1;
      maxdims[0] = H5S_UNLIMITED;
      dataspace  = H5Screate_simple (1, dims, maxdims); 

      // Modify dataset creation properties, i.e. enable chunking 
      hsize_t chunk_dims[1] = {_ChunkSize};
      hid_t   cparms = H5Pcreate(H5P_DATASET_CREATE);
      status = H5Pset_chunk(cparms, 1, chunk_dims);

      dataset = H5Dcreate (file_id, name.c_str(), 
			   datatype, dataspace,
			   cparms);
    }
  H5Sclose(dataspace);

  // extend dataset and get dataspace for it
  dims[0] = i_start_dataset+n_write;
  status = H5Dextend(dataset, dims);
  dataspace = H5Dget_space(dataset);

  // select hyperslab in HDF file dataset dataspace:
  // the last count[0] entries, starting at entry save_index 
  offset[0] = i_start_dataset;
  count[0]  = n_write;
  status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, 
			       offset, NULL, count, NULL);

  // buf initialization
  T *buf = new T[ n_write ];
  for ( int i=0; i<n_write; i++ )
    buf[i] = d[i + i_start];
  // memory dataspace -- the entire buf array
  dataspace_mem  = H5Screate_simple (1, count, count); 

  //do writing
  H5Dwrite(dataset, 
	   datatype, 
	   dataspace_mem, dataspace, H5P_DEFAULT, 
	   buf  ); 

  H5Dclose(dataset);
  H5Sclose(dataspace);
  H5Sclose(dataspace_mem);
  delete[] buf; 
}



template<class T>
void ScalarDequeInoutOps::ReadFromHDFFile(std::string name, 
					  int size, 
					  std::deque<T>& d, 
					  Save2HDF &hdf)
{
  hid_t file_id;
  hid_t dataspace, dataspace_mem;
  hid_t dataset;

  hsize_t dims[1], maxdims[1];
  hsize_t offset[1], count[1];

  herr_t  status;  

  hid_t datatype = HDF5TypeTraits<T>::type();

  file_id = hdf.GetFileId();
  
  dataset   = H5Dopen(file_id, name.c_str());
  dataspace = H5Dget_space(dataset);
  H5Sget_simple_extent_dims(dataspace, dims, maxdims);

  if ( dims[0] < size )
    { 
      count[0]  = dims[0];
      offset[0] = 0;
    }
  else
    { 
      count[0]  = size;
      offset[0] = dims[0]-size;
    }
    
  // select hyperslab
  status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, 
			       offset, NULL, count, NULL);

  
  // buf initialization
  T *buf = new T[ count[0] ];
  // memory dataspace -- the entire buf array
  dataspace_mem  = H5Screate_simple (1, count, count); 

  //do writing
  H5Dread(dataset, 
	  datatype, 
	  dataspace_mem, dataspace, H5P_DEFAULT, 
	  buf  ); 

  H5Dclose(dataset);
  H5Sclose(dataspace);
  H5Sclose(dataspace_mem);

  d.resize(count[0]);
  for ( int i=0; i<count[0]; i++ ) d[i] = buf[i];
  delete[] buf; 
}

#endif
