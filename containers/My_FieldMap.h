#ifndef MY_FIELD_MAP_H
#define MY_FIELD_MAP_H


#include "My_FieldWrapper.h"
#include "My_ContainerMap.h"


/**
 * \ingroup containers_grp
 * 
 * \class My_FieldMap
 * 
 * \brief Implements addition of Field to the map My_ContainerMap
 * by instantiating My_FieldWrapper objects which will be added to
 * the My_ContainerMap
 *
 */
class My_FieldMap: public My_ContainerMap
{
public:
  // Typedefs
  typedef  My_ContainerMap Base_t;
  typedef  Base_t::Size_t  Size_t;
  typedef  Base_t::Pair_t  Pair_t;


  /**
   * Add a new Field to our list.  This will make an FieldWrapper
   * instance that will wrap the item, and add it to our list.
   * Return the index of the newly added Field
   *
   * @param name name of the Field
   * @param item Field
   */
  template<class T>
  Size_t Add(std::string name, T &item)
    {
      Pair_t p(name, new My_FieldWrapper<T>(item));
      return My_ContainerMap::AddPairToContainerMap(p);
    };
};


#endif

