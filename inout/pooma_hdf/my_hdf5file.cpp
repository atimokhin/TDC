#include "my_hdf5file.h" 

//! create group with name grpName id it doesnt exist
//! otherwise do nothing
bool My_HDF5File::createGroup(const char *grpName) const
{
  herr_t status;
  hid_t group_id;
 
  if ( group_id = H5Gopen(this->file(), grpName) < 0 )
    {
      group_id = H5Gcreate(this->file(), grpName, 0);
    }

  status = H5Gclose(group_id);
  return ( status >= 0 ? true: false);
}

