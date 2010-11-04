#ifndef MY_ATTRIBUTE_MAP_H
#define MY_ATTRIBUTE_MAP_H

#include "My_AttributeWrapper.h"
#include "My_ContainerMap.h"


/**
 * \ingroup containers_grp
 * 
 * \class My_AttributeMap
 * 
 * \brief Implements addition of Attribute to the map My_ContainerMap
 * by instantiating My_AttributeWrapper objects which will be added to
 * the My_ContainerMap
 *
 */
class My_AttributeMap: public My_ContainerMap
{
public:

  typedef  My_ContainerMap::Size_t Size_t;
  typedef  My_ContainerMap::Pair_t Pair_t;

  /**
   * Add a new Attribute to our list.  This will make an AttributeWrapper
   * instance that will wrap the item, and add it to our list.
   * Return the index of the newly added Attribute
   *
   * @param name name of the Attribute
   * @param item Attribute
   */
  template<class T>
  Size_t Add(std::string name, T &item)
    {
      Pair_t p(name, new My_AttributeWrapper<T>(item));
      return My_ContainerMap::AddPairToContainerMap(p);
    }

};


#endif

