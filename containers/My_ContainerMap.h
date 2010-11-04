#ifndef MY_CONTAINER_MAP_H
#define MY_CONTAINER_MAP_H


#include <map>
#include <string>
#include <algorithm>
#include <iosfwd>

#include "My_ContainerWrapper.h"
#include "../inout/save2hdf.h"



/**
 * \ingroup containers_grp
 * 
 * \class My_ContainerMap
 * 
 * \brief stores a map of names and Container pointers, that are used
 * to store a heterogenous collection of Containers.
 *
 * When an My_ContainerMap is destroyed, it will delete all the
 * containers it contains.  It provides the same interface as
 * My_Container, but it will loop over all the containers it stores and
 * perform these operations on each one in turn.  You can add new
 * containers to an My_ContainerMap or delete container from a list
 * by referring to the container name.
 *
 */
class My_ContainerMap
{
public:

  // Typedefs and enumerations =================================
  typedef My_ContainerMap                      This_t;
  typedef std::map<std::string,My_Container*>  Map_t;
  typedef Map_t::size_type Size_t;

  typedef Map_t::iterator        Iter_t;
  typedef Map_t::const_iterator  CIter_t;

  typedef std::pair<const std::string, My_Container*> Pair_t;
  //============================================================


  /**
   * My_ContainerMap just has a default constructor; it initially has
   * no attributes at all.
   */
  My_ContainerMap(){};
  //! Destrictor
  ~My_ContainerMap();

  //! Return attribute with the name "name"
  My_Container *Attribute(std::string name);

  //! Return the number of attributes
  Size_t Size() const { return map_m.size(); };
  
  //! Remove (and delete) the attrib with name "name" from our list. 
  bool Remove(std::string);

  //! execute functor f for each attribute
  template<class UnaryFunction> void DoForEach( UnaryFunction f);

  //! Save each element into an HDF file as "name/id"
  bool SaveToHDFFile(std::string id, Save2HDF &hdf);
  //! Save each element into an HDF file as "group_name/name"
  bool SaveToHDFFileGroup(std::string group_name, Save2HDF &hdf);
  //! Save complimentary info for each element into an HDF file as "name/..."
  bool SaveComplimentaryInfoToHDFFile(Save2HDF &hdf) const;

  //! Read each element from the HDF file dataset "name/id"
  bool ReadFromHDFFile(std::string id, Save2HDF &hdf);
  //! Read each element from the HDF file dataset "group_name/name"
  bool ReadFromHDFFileGroup(std::string group_name, Save2HDF &hdf);


  //! Print the contents of each Attribute to the given ostream.
  void Print(std::ostream&) const;

protected:
  Size_t AddPairToContainerMap(const Pair_t &p);

protected:
  //! The list of Attribute pointers
  Map_t map_m;


private:
  // Make copy constructor and operator= private and undefined since
  // they should not be used
  My_ContainerMap(const My_ContainerMap &)
    {
      PInsist(false, "Called My_ContainerMap copy constructor.");
    }

  const This_t &operator=(const This_t &)
    {
      PInsist(false, "Called My_ContainerMap operator=.");
      return *this;
    }
};



/**
 * Return attribute with the name "name"
 * 
 */
inline My_Container* My_ContainerMap::Attribute(std::string name)
{
  if ( map_m.count(name) )
    {
      return map_m[name];
    }
  else
    {
      std::cout<<"Error: Attribute Map::attribute :";
      std::cout<<"There is no attribute \""<<name<<"\" in Attribute Map\n!";
      exit(1);
    }
};


/**
 * execute functor f for each attribute
 * 
 */
template<class UnaryFunction>
void My_ContainerMap::DoForEach( UnaryFunction f)
{
  for_each(map_m.begin(), map_m.end(), f);
};


//! Declaration of operator<< for ostream and My_ContainerMap
std::ostream& operator<<(std::ostream&, const My_ContainerMap&);


#endif

