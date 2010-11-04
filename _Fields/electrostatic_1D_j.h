#ifndef ELECTROSTATIC_1D_J_H
#define ELECTROSTATIC_1D_J_H

#include <string>

#include "ATbase.h"

/**
 * \ingroup fields_grp
 *
 * Inductive current.
 */
class Electrostatic_1D_J
{
public:

  void SetupFromConfigGroup(FileInput& in);

  double operator() (double t);

private:

  std::string _Type;

  double _J0;
};

#endif
