#ifndef DIMENSIONAL_CONSTS_SETUP_H
#define DIMENSIONAL_CONSTS_SETUP_H

#include <string>

#include "ATbase.h"

#include "consts_initializer.h"

class DimensionalConstsSetup
{
public:
  //! setup from config file group
  void SetupFromConfig(FileInput &in);

private:
  ConstsInitializer* MakeInitializer(std::string type);
};

#endif
