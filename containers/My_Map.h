#ifndef MY_MAP_H
#define MY_MAP_H

#include <map>
#include <string>
#include <algorithm>



/**
 * \ingroup containers_grp
 * 
 * \class My_Map
 * 
 * \brief Map(string,object) 
 *
 * My_Map stores a map of names and classes pointers, that are used to
 * store a homogenous collection of type T. This is simple wrapper of STL map class.
 *
 */
template<class T>
class My_Map
{
public:
  // Typedefs ----------------------------------------------
  typedef T           Content_t;
  typedef My_Map<T>   This_t;

  typedef std::pair< const std::string, T* > Pair_t;
  typedef std::map< const std::string, T* >  Map_t;

  typedef typename Map_t::size_type       Size_t;
  typedef typename Map_t::iterator        Iter_t;
  typedef typename Map_t::const_iterator  CIter_t;
  // Typedefs ----------------------------------------------


  //! Default constructor; map is empty now
  My_Map(){};
  //! Destructor - empty
  ~My_Map(){};


  //! Return the number of attributes
  Size_t Size() const { return map_m.size(); };
  //! Return pointer to the item with the name "name"
  Content_t *Item(std::string name) const;


  //! Add a new Attribute to our list
  Size_t Add(std::string name, T &item);
  //! Remove (and delete) the attrib with name "name" from our list.  
  bool Remove(std::string);


  //! execute functor f for each attribute
  template<class UnaryFunction>
  void DoForEach( UnaryFunction f);


protected:

  //! add pair to internal map
  Size_t AddPairToMap(const Pair_t &p);

protected:

  //! The list of Attribute pointers
  mutable Map_t map_m;

private:

  // Make copy constructor and operator= private and undefined since
  // they should not be used
  My_Map(const My_Map &) {};
  const This_t &operator=(const This_t &){};

};




/**
 *  If inetm with the name "name" exests return pointer to it.
 *  If not returns NULL
 * 
 */
template<class T>
T* My_Map<T>::Item(std::string name) const
{
  if ( map_m.count(name) )
    {
      return map_m[name];
    }
  else
    return NULL;
};



/**
 * Add a new Attribute to our list. 
 * Return the index of the newly added item
 *
 * @param name name of the item
 * @param item item
 */
template<class T>
typename My_Map<T>::Size_t  My_Map<T>::Add(std::string name, T &item)
{
  Pair_t p(name, &item);
  return AddPairToMap(p);
}


/**
 * Remove item with the name "name" from our list.  
 * Return success.
 */
template<class T>
bool My_Map<T>::Remove(std::string name)
{
  if ( map_m.count(name) )
    {
      map_m.erase(name);
      return true;
    }
    
  return false;
};


/**
 * Execute functor f for each attribute.
 */
template<class T>
template<class UnaryFunction>
void My_Map<T>::DoForEach( UnaryFunction f)
{
  for_each(map_m.begin(), map_m.end(), f);
};



/**
 * Add pair to internal Map
 */
template<class T> 
typename My_Map<T>::Size_t  My_Map<T>::AddPairToMap(const Pair_t &p)
{
  std::pair<Iter_t,bool> ptest = map_m.insert(p);
  if ( ptest.second )
    {
	  return (this->Size() - 1);
    }
  else
    {
      std::cout<<"Error: My_Map::addPairToMap :";
      std::cout<<"Container \""<<p.first<<"\" cannot be inserted in My_Map\n!";
      exit(1);	  
    }
};


#endif 
