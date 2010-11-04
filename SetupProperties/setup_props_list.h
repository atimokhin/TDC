#ifndef SETUP_PROPS_LIST_H
#define SETUP_PROPS_LIST_H

#include <iostream>
#include <list>

#include "../utils/io_filenames.h"

#include "setup_props.h"


/**
 *  \ingroup setup_properties_grp
 *
 *  \class SetupPropsList
 *  \brief List containing all setup properties 
 * 
 */
class SetupPropsList
{
public:

  SetupPropsList(); 
  ~SetupPropsList(); 

  // initialize all properties classes
  void Initialize();

  // save all properties into HDF file
  void SaveToHDFFile() const;
  // reastore all properties from HDF file
  void ReadFromHDFFile(Save2HDF& hdf);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

private:
  list<SetupProps*> _List;
  
  typedef list<SetupProps*>::const_iterator CI;
};



inline std::ostream& operator<< ( std::ostream& s, const SetupPropsList& pl )
{
  return pl.Print(s);
};

#endif
