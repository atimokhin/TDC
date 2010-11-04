#ifndef MY_CONTAINER_WRAPPER_H
#define MY_CONTAINER_WRAPPER_H


#include <iostream>

#include "My_Container.h"



/**
 * \ingroup containers_grp
 * 
 * \class My_ContainerWrapper
 * 
 * \brief My_ContainerWrapper<T> is a subclass of the abstract base
 * class My_Container.  It is templated on a class T, where T should
 * be some form of Array or DynamicArray that supports a dynamic data
 * structure interface.
 *
 * My_ContainerWrapper is meant to be used as an external polymorphism
 * derived class.  You create an My_ContainerWrapper and give it an
 * object to wrap around; the abstract base class is used to provide
 * an abstract interface to a heterogenous collection of
 * My_ContainerWrappers from some other user, for example a
 * My_Particles or My_Fields classes.  Such containers contains
 * references to Attributes or Fields that should be saved to HDF file
 * or printed out.
 *
 */
template<class T>
class My_ContainerWrapper : public My_Container
{
public:
  // Typedefs ================================
  typedef My_ContainerWrapper<T>  This_t;
  typedef T                       Wrapped_t;
  //==========================================


  /**
   * Constructor
   * 
   * My_ContainerWrapper should be initialized with an object of t
   * that it will wrap.  It will store a reference to this object.
   */
  My_ContainerWrapper(Wrapped_t &array)
    : wrapped_m(array)
  {};


  /**
   *  Destrictor                                  
   *  My_ContainerWrapper does not need to do anyt
   *  since it just wraps a reference to an object.
   */
  virtual ~My_ContainerWrapper() {};


  //! Return a reference to our wrapped object.
  Wrapped_t& Array() { return wrapped_m; };
  //! Return a const reference to our wrapped object.
  const Wrapped_t& Array() const { return wrapped_m; };



  //! Print the contents to the given Inform stream.
  virtual void Print(std::ostream &o) const
    {
      o << this->Array() << std::endl;
    };


private:
  //! The object that we're wrapping
  Wrapped_t &wrapped_m;

  // Make copy consructor, default constructor, and operator= private
  // and undefined since they should not be used

  My_ContainerWrapper();
  My_ContainerWrapper(const This_t &);
  const This_t &operator=(const This_t &);
};




/**
 * A specialization of the Inform traits used to say that My_ContainerWrapper has
 * a print method.                                                               
 */
template<class T>
std::ostream &operator<<(std::ostream &o, const My_ContainerWrapper<T> &cont)
{
  cont.Print(o);
  return o;
}

#endif
