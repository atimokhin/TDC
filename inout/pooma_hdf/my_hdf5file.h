// -*- C++ -*-

#ifndef RHALK_HDF5FILE_ATIM_H
#define RHALK_HDF5FILE_ATIM_H

#include <string>
#include <iostream>

#include "hdf5file.h"

class My_HDF5File: public HDF5File {
public:
  My_HDF5File(bool parallel_io = false, bool debug = false): 
    HDF5File(parallel_io, debug) 
  {};

  //My_HDF5File(const HDF5File&);       // not implemented, cause link error, if called
  //HDF5File& operator=(const HDF5File&); // not implemented, cause link error, if called

  ~My_HDF5File() {};


  //! name of HDF file associated with this object
  std::string GetFilename() const { return _FileName; };


  //! write scalar quantity to HDF file
  template <class T>
  bool writeScalar(const char *id, T value);
  //! read scalar quantity from HDF file
  template <class T>
  bool readScalar(const char *id, T& value);

  //! write Mesh information into HDF file
  template <class Mesh>
  bool writeMesh(const Mesh& mesh);


  //! create group with name grpName
  bool createGroup(const char *grpName) const;
};


template <class T>
bool My_HDF5File::writeScalar(const char *id, T value)
{
  bool res = false;

  HDF5Object<T> o(value);
  hid_t type_id  = o.type();
  hid_t space_id = o.dataspace();

  hid_t dataset_id = H5Dcreate(loc(), id, type_id, space_id, H5P_DEFAULT);
  if (!(dataset_id < 0)) {
    res = !(H5Dwrite(dataset_id, type_id, space_id, H5S_ALL, H5P_DEFAULT, o.read() ) < 0);
    H5Dclose(dataset_id);
  }
  H5Sclose(space_id);
  H5Tclose(type_id);

  if (!pio())
    return RemoteProxy<bool>(res, ioContext()).value();
  else
    return allOk(res);
}


template <class T>
bool My_HDF5File::readScalar(const char *id, T& value)
{
  bool res = false;

  HDF5Object<T> o(value);
  hid_t type_id  = o.type();
  hid_t space_id = o.dataspace();

  hid_t dataset_id = H5Dopen(loc(), id);


  if (!(dataset_id < 0)) {
    res = !(H5Dread(dataset_id, type_id, H5S_ALL,H5S_ALL, H5P_DEFAULT, &value)< 0);
    H5Dclose(dataset_id);
  }
  H5Sclose(space_id);
  H5Tclose(type_id);

  if (!pio())
    return RemoteProxy<bool>(res, ioContext()).value();
  else
    return allOk(res);
}


/**
 * save mesh vertex positions into HDF file
 * 
 */
template <class Mesh>
bool My_HDF5File::writeMesh(const Mesh& mesh)
{
  std::string name;
  
  Interval<1> VD = mesh.physicalVertexDomain();  
  Array<1,double,Brick> pos(VD);

  for ( int i=VD.first(); i<=VD.last(); i++)
      pos(i) = mesh.vertexPosition(0,i);
  createGroup("Mesh");
  name = "Mesh/CellBoundaries";
  writeArray(name.c_str(), pos(VD));
  
  Interval<1> CD = mesh.physicalCellDomain();  
  for ( int i=CD.first(); i<=CD.last(); i++)
      pos(i) = mesh.cellPosition(0,i);
  name = "Mesh/CellCenters";
  writeArray(name.c_str(), pos(CD));

  return true;
}




#endif /* defined RHALK_HDF5FILE_ATIM_H */
