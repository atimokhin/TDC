#ifndef MY_PROPERTY_MAP_H
#define MY_PROPERTY_MAP_H

#include "My_PropertyWrapper.h"
#include "My_ContainerMap.h"


/**
 * \ingroup containers_grp
 * 
 * \class My_PropertyMap
 * 
 * \brief Implements addition of Property to the map My_ContainerMap
 * by instantiating My_PropertyWrapper objects which will be added to
 * the My_ContainerMap
 *
 */
class My_PropertyMap: public My_ContainerMap
{
public:

  typedef  My_ContainerMap::Size_t Size_t;
  typedef  My_ContainerMap::Pair_t Pair_t;

  /**
   * Add a new Property to our list.  This will make an PropertyWrapper
   * instance that will wrap the item, and add it to our list.
   * Return the index of the newly added Property
   *
   * @param name name of the Property
   * @param item Property
   */
  template<class T>
  Size_t Add(std::string name, T &item)
    {
      Pair_t p(name, new My_PropertyWrapper<T>(item));
      return My_ContainerMap::AddPairToContainerMap(p);
    }
};


#endif


