#ifndef MY_ATTRIBUTE_WRAPPER_H
#define MY_ATTRIBUTE_WRAPPER_H

#include "My_ContainerWrapper.h"
#include "../inout/save2hdf.h"


/** 
 * \ingroup containers_grp
 *
 * \class My_AttributeWrapper 
 *
 * This is a subclass of My_ContainerWrapper<T> that implements the
 * basic My_Container interface by passing on the operations in the
 * interface to an object of type T that My_AttributeWrapper wraps.
 *
 * <B> It implementins method SaveToHDFFile for POOMA Arrays </B>, as
 * it uses HDF5File class capability to save Arrays
 *
 */
template<class T>
class My_AttributeWrapper : public My_ContainerWrapper<T>
{
  typedef T   Wrapped_t;

public:

  /**
   * My_AttributeWrapper should be initialized with an object of type
   * that it will wrap.  It will store a reference to this object.
   */
  My_AttributeWrapper(Wrapped_t &array): 
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
    return hdf.writeArray(name.c_str(), this->Array());
  };


  bool ReadFromHDFFile(std::string name, Save2HDF &hdf) 
  {
    return hdf.readArray(name.c_str(), this->Array());
  };
};

#endif 
