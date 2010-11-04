#ifndef CONSTS_INITIALIZER_H
#define CONSTS_INITIALIZER_H

#include "ATbase.h"

/**
 * \ingroup setup_consts_grp
 *
 * \class ConstsInitializer
 * \brief Base class for Constants Initializers
 * 
 */
class ConstsInitializer
{
public:
  //! setup from config file group
  virtual void SetupFromConfigGroup(FileInput &in) = 0;
};

#endif
