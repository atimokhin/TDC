#ifndef SAVE2HDF_H
#define SAVE2HDF_H

#include <string>
#include "hdf5.h"
#include "pooma_hdf/my_hdf5file.h"



/**
 *  \defgroup inout_grp input/output
 */


/**
 *  \ingroup inout_grp 
 *
 *  \class Save2HDF
 *  \brief Class for output of calculation results to HDF file
 */
class Save2HDF: public My_HDF5File
{
public:
  //! Constructor
  Save2HDF(bool parallel_io = false, bool debug = false):
    My_HDF5File(parallel_io,debug)
  {};

  //! flush file
  void FlushHDFFile() { H5Fflush(this->file(), H5F_SCOPE_GLOBAL ); };

  //! save smart object
  template<class C> void SaveObject(const C &obj) const;
  //! save object into the group "name"
  template<class C> void SaveObject(std::string name, const C &obj ) const;

  //! Save an STL Container
  template<class Cont> bool SaveSTLContainer( std::string name, Cont &cont ) const;
  //! Read dataset from HDF file into an STL Container
  template<class Cont> bool ReadSTLContainer( std::string name, Cont &cont ) const;

};



/**
 *  Save object which creates an HDF group itself
 *  \param obj   object
 * 
 */
template<class C> 
void Save2HDF::SaveObject(const C &obj) const
{
  obj.Save(GetFileId());
}


/**
 *  Create HDF group and save there an object 
 *  \param obj   object
 *  \param name  name of the HDF group to be created
 */
template<class C> 
void Save2HDF::SaveObject( std::string name, const C &obj) const
{
  obj.Save(GetFileId(), name);
}


//! Save an STL Container
template<class Cont> 
bool Save2HDF::SaveSTLContainer( std::string name, Cont &cont ) const
{
  bool res = false;

  // type of the data
  typedef typename Cont::value_type T;
  hid_t type_id = HDF5TypeTraits<T>::type();

  T* buf = new T[cont.size()];
  hsize_t dims[1],  maxdims[1];
  dims[0]=maxdims[0]=cont.size();

  int i = 0;
  for (typename Cont::iterator iter=cont.begin(); iter != cont.end(); iter++, i++ )
    buf[i] = *iter; 

  // save to file -----------------------------
  hid_t  space_id = H5Screate_simple(1,dims,maxdims);
  // save to file
  hid_t dataset_id = H5Dcreate(loc(), name.c_str(), type_id, space_id, H5P_DEFAULT);
  if (!(dataset_id < 0)) {
    res = !(H5Dwrite(dataset_id, type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf ) < 0);
    H5Dclose(dataset_id);
  }
  H5Sclose(space_id);
  H5Tclose(type_id);

  delete[] buf;
  return res;
}


//! Save an STL Container
template<class Cont> 
bool Save2HDF::ReadSTLContainer( std::string name, Cont &cont ) const
{
  bool res = false;

  // type of the data
  typedef typename Cont::value_type T;
  hid_t type_id = HDF5TypeTraits<T>::type();

  hsize_t dims[1],  maxdims[1];

  // read from file -----------------------------
  hid_t dataset_id = H5Dopen( loc(), name.c_str()  );
  if ( !(dataset_id < 0) ) 
    {
      hid_t dataspace_id = H5Dget_space(dataset_id);
      H5Sget_simple_extent_dims(dataspace_id, dims, maxdims  ); 

      T* buf = new T[ dims[0] ];
      res = !(H5Dread(dataset_id, type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf ) < 0);
            
      H5Dclose(dataset_id);
      H5Sclose(dataspace_id);

      cont.resize( dims[0] );
      int i = 0;
      for (typename Cont::iterator iter=cont.begin(); iter != cont.end(); iter++, i++ )
	*iter = buf[i]; 

      delete[] buf;
    }

  H5Tclose(type_id);

  return res;
}



#endif
