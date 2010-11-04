#ifndef MAGNETIC_FIELD_MAKER_H
#define MAGNETIC_FIELD_MAKER_H

#include <string>
#include <list>

#include "ATbase.h"

#include "magnetic_field.h"

/**
 * \ingroup magnetic_field_grp
 *
 * \class MagneticFieldMaker
 * \brief MagneticFieldMaker class factory
 *
 *  Makes MagneticField object of requested type
 */
class MagneticFieldMaker
{
public:
  //! constructor: only sets input file
  MagneticFieldMaker(FileInput& in): _Input(in) {};

  //! make MagneticField class object from config file
  MagneticField* pMake( string type ) const;

private:
  FileInput& _Input;     //! pointer to the input class
};

#endif
