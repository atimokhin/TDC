#ifndef MY_PROPERTY_WRAPPER_H
#define MY_PROPERTY_WRAPPER_H

#include "My_ContainerWrapper.h"
#include "../inout/save2hdf.h"


/** 
 * \ingroup containers_grp
 *
 * \class My_PropertyWrapper 
 *
 * This is a subclass of My_ContainerWrapper<T> that implements the
 * basic My_Container interface by passing on the operations in the
 * interface to an object of type T that My_PropertyWrapper wraps.
 *
 * <B> It implementins method SaveToHDFFile for scalar properties
 * </B>, as it uses Save2HDF class capability to save Scalars
 *
 */
template<class T>
class My_PropertyWrapper : public My_ContainerWrapper<T>
{
  typedef T   Wrapped_t;

public:

  /**
   * My_PropertyWrapper should be initialized with an object of type
   * that it will wrap.  It will store a reference to this object.
   */
  My_PropertyWrapper(Wrapped_t &array): 
    My_ContainerWrapper<Wrapped_t>(array)
  {};


  /** 
   * Saves wrapped object into and HDF file given in hdf object
   * by creating a datased with name "name"
   * 
   * @param name name of the dataset
   * @param hdf  HDF5File object performing saving
   */
  bool SaveToHDFFile(std::string name, Save2HDF &hdf) const
  {
    
    return hdf.writeScalar(name.c_str(),this->Array());
  };


  bool ReadFromHDFFile(std::string name, Save2HDF &hdf) 
  {
    return hdf.readScalar(name.c_str(),this->Array());
  };

};

#endif 
