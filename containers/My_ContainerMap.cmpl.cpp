#include "My_ContainerMap.h"


/**
 * Destructor.  My_ContainerMap will delete all attributes that it
 * owns when it is deleted.
 */
My_ContainerMap::~My_ContainerMap()
{
  for (Iter_t iter = map_m.begin(); iter != map_m.end(); ++iter)
    map_m.erase(iter);
}



/**
 * Remove the attrib with the name "name" from our list, deleting it
 * in the process.  Return success.
 */

bool My_ContainerMap::Remove(std::string name)
{
  if ( map_m.count(name) )
    {
      delete Attribute(name);
      map_m.erase(name);
      return true;
    }

  return false;
}


/** 
 * Save each element into an HDF file in to the group
 * "name" [as "name/id" dataset]
 *
 * @param id ID of the current data dump
 * @param hdf HDF5File object doing dump 
 */
bool My_ContainerMap::SaveToHDFFile(std::string id, Save2HDF &hdf)
{
  bool status = true;
  for (CIter_t iter = map_m.begin(); iter != map_m.end(); ++iter)    
    {
      // check wheter a group exists and create the group if
      // it is necessary
      hdf.createGroup( iter->first.c_str() );

      // save attribute
      std::string dataset_name = iter->first + "/" + id;
      status = (iter->second)->SaveToHDFFile(dataset_name.c_str(),hdf) && status;
    }  
  return status;
}

/** 
 * Save each element into an HDF file in to the group 'group_name'
 * [as "group_name/name" dataset]
 *
 * @param group_name  HDF groupname
 * @param hdf         HDF5File object doing dump 
 */
bool My_ContainerMap::SaveToHDFFileGroup(std::string group_name, Save2HDF &hdf)
{
  bool status = true;
  for (CIter_t iter = map_m.begin(); iter != map_m.end(); ++iter)    
    {
      // check wheter a group exists and create the group if
      // it is necessary
      hdf.createGroup( group_name.c_str() );
      
      // save attribute
      std::string  dataset_name = group_name + "/" + iter->first;
      status = (iter->second)->SaveToHDFFile(dataset_name.c_str(),hdf) && status;
    }  
  return status;
}

/** 
 * Save complimentary information for each element into an HDF file in
 * to the group "name" [as "name/..." dataset]
 *
 * @param id ID of the current data dump
 * @param hdf HDF5File object doing dump 
 */
bool My_ContainerMap::SaveComplimentaryInfoToHDFFile(Save2HDF &hdf) const
{
  bool status = true;
  for (CIter_t iter = map_m.begin(); iter != map_m.end(); ++iter)    
    {
      // check wheter a group exists and create the group if
      // it is necessary
      hdf.createGroup( iter->first.c_str() );

      // save attribute
      std::string dataset_name = iter->first;
      status = (iter->second)->SaveComplimentaryInfoToHDFFile(dataset_name.c_str(),hdf) && status;
    }  
  return status;
}



/** 
 * Initialize each element by reading from dataset "name/id" in the
 * corresponding HDF file
 *
 * @param id ID of the data dump
 * @param hdf HDF5File object representing fiel to be read
 */
bool My_ContainerMap::ReadFromHDFFile(std::string id, Save2HDF &hdf)
{
  bool status = true;
  for (CIter_t iter = map_m.begin(); iter != map_m.end(); ++iter)    
    {
      // save attribute
      std::string dataset_name = iter->first + "/" + id;
      status = (iter->second)->ReadFromHDFFile(dataset_name.c_str(),hdf) && status;
    }  
  return status;
}

/** 
 * Initialize each element by reading from dataset "group_name/name"
 * in the corresponding HDF file
 *
 * @param id ID of the data dump
 * @param hdf HDF5File object representing fiel to be read
 */
bool My_ContainerMap::ReadFromHDFFileGroup(std::string group_name, Save2HDF &hdf)
{
  bool status = true;
  for (CIter_t iter = map_m.begin(); iter != map_m.end(); ++iter)    
    {
      // save attribute
      std::string  dataset_name = group_name + "/" + iter->first;
      status = (iter->second)->ReadFromHDFFile(dataset_name.c_str(),hdf) && status;
    }  
  return status;
}




/**
 * Print out My_ContainerMap to an ostream.
 * 
 */
void My_ContainerMap::Print(std::ostream& o) const
{
  for (CIter_t iter = map_m.begin(); iter != map_m.end(); ++iter)    
    {
      o<<"Attribute name: \""<<iter->first<<"\"\n";
      (iter->second)->Print(o);
    }
}


/**
 * Add pair to the internal map
 * 
 */
My_ContainerMap::Size_t My_ContainerMap::AddPairToContainerMap(const Pair_t &p)
{
  std::pair<Iter_t,bool> ptest = map_m.insert(p);
  if ( ptest.second )
    {
      return (this->Size() - 1);
    }
  else
    {
      std::cout<<"Error: ContainerMap::attribute :";
      std::cout<<"Container \""<<p.first<<"\" cannot be inserted in Container Map\n!";
      exit(1);	  
    }
};




/**
 * When My_ContainerMap is passed to an ostream, call the print method.
 * 
 */
std::ostream& operator<<(std::ostream& o, const My_ContainerMap& alist)
{
  alist.Print(o);
  return o;
}

