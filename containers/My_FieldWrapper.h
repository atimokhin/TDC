#ifndef MY_FIELD_WRAPPER_H
#define MY_FIELD_WRAPPER_H


#include "My_ContainerWrapper.h"
#include "../inout/save2hdf.h"


/** 
 * \ingroup containers_grp
 *
 * \class My_FieldWrapper
 *
 * \brief Field object wrapper
 *
 * This is a subclass of My_ContainerWrapper<T> that implements the
 * basic My_Container interface by passing on the operations in the
 * interface to an object of type T that My_FieldWrapper wraps.  
 *
 * <B> It implementins method SaveToHDFFile for POOMA Fields </B>, as
 * *it uses HDF5File class capability to save Fields
 *
 */
template<class T>
class My_FieldWrapper : public My_ContainerWrapper<T>
{
  typedef T   Wrapped_t;

public:

  /**
   * My_FieldWrapper should be initialized with an object of type
   * that it will wrap.  It will store a reference to this object.
   */
  My_FieldWrapper(Wrapped_t &array): 
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
    return hdf.writeField(name.c_str(),this->Array(),(this->Array()).layout().externalGuards() );
  };
  
  bool ReadFromHDFFile(std::string name, Save2HDF &hdf) 
  {
     GuardLayers<1> g = (this->Array()).layout().externalGuards();
     return hdf.readField(name.c_str(), this->Array(), g);
  };

  /** 
   * Saves field point positions for the dataset "name" 
   * 
   * @param name name of the dataset
   * @param hdf  HDF5File object performing saving
   */
  bool SaveComplimentaryInfoToHDFFile(std::string name, Save2HDF &hdf) const 
  {
    Array<1, typename Wrapped_t::Mesh_t::PointType_t> a( (this->Array()).physicalDomain() );
    a = Pooma::positions( this->Array() );
   
    std::string dataset_name = name + "/Positions";
    return hdf.writeArray( dataset_name.c_str(), a );
  };

};

#endif 







