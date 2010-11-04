#ifndef MY_CONTAINER_H
#define MY_CONTAINER_H

#include "../inout/save2hdf.h"


/**
 * \defgroup containers_grp containers
 * 
 */


/**
 * \ingroup containers_grp
 *
 * \class My_Container
 * 
 * \brief an abstract base class for my containers
 *
 * This is an abstract base class used to implement an external
 * polymorphism interface to Arrays and Fields objects.  The
 * My_ContainerWrapper subclass is templated on the type of Array
 * (really, DynamicArray) of Fields that the user wants to provide an
 * abstract interface to.  This is used to let users create
 * heterogenous collections of DynamicArray's, and to perform common
 * tasks on all of them such as print. <B> It is obtained by modifying
 * POOMA Attribute class </B>
 *
 */
class My_Container
{
public:

  //! Deafult Constructor
  My_Container() {};

  //! Copy constructor.
  My_Container(const My_Container &){};


  /**
   * Destructor                               
   * My_Container needs a virtual destructor, 
   * My_Container's from a base class pointer.
   */
  virtual ~My_Container() {};


  //! Print the contents of the Array to the given stream.
  virtual void Print(std::ostream &) const = 0;


  /** 
   * Interface for saving wrapped object into HDF file  given in hdf object
   * by creating a datased with name "name".
   * 
   * Implementation will be different for each kind of wrapped
   * objects, e.g. Arrays, Fields, Scalars because it will use
   * different methods of Save2HDF class.
   *
   * @param name name of the dataset
   * @param hdf  HDF5File object performing saving
   */
  virtual bool SaveToHDFFile(std::string name, Save2HDF &hdf) const = 0;


  virtual bool ReadFromHDFFile(std::string name, Save2HDF &hdf) = 0;

  
  /** 
   * Interface for saving of complimentary information about wrapped
   * object into HDF file given in hdf object
   * 
   * Currently it is used only for Fields in order to save positions
   * in the file, therefore the default implementation is empty
   *
   * @param name name of the object
   * @param hdf  HDF5File object performing saving
   */
  virtual bool SaveComplimentaryInfoToHDFFile(std::string name, Save2HDF &hdf) const {};

};



/**
 * A specialization of the Inform traits used to say that My_Container
 * has a print method.
 * 
 */
inline std::ostream& operator<<(std::ostream& o, const My_Container& cont)
{
  cont.Print(o);
  return o;
}

#endif
