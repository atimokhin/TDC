#ifndef SETUP_PROPS_H
#define SETUP_PROPS_H

#include <iostream>
#include <string>

#include "ATbase.h"

#include "../inout/save2hdf.h"


/**
 *  \defgroup setup_properties_grp Setup Properties
 */


/**
 *  \ingroup setup_properties_grp
 *
 *  \class SetupProps
 *  \brief Base Class for classes with setup properties
 *
 *  All subclasses should store parameters as static memebrs in order
 *  they can be used everywhwre withoud additional initialization
 *
 *  Contains virtual functions for
 *  - Read parameters from config file
 *  - Saving aparmeters into HDF file 
 *  - Print parameters into an output stream
 */
class SetupProps
{
public:

  //! Initialize static memebers
  virtual void Initialize() = 0;

  //! save properties into an hdf file
  virtual void SaveToHDFFile(Save2HDF& hdf) const = 0;
  //! save properties into an hdf file
  virtual void ReadFromHDFFile(Save2HDF& hdf) = 0;

  //! Print physical paramneters
  virtual std::ostream& Print(std::ostream& s) const { return s; };

};



inline std::ostream& operator<< ( std::ostream& s, const SetupProps& init )
{
  return init.Print(s);
};

#endif
