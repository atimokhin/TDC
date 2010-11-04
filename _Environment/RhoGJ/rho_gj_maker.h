#ifndef RHO_GJ_MAKER_H
#define RHO_GJ_MAKER_H

#include <string>
#include <list>

#include "ATbase.h"

#include "rho_gj.h"

/**
 * \ingroup rho_gj_grp
 *
 * \class RhoGJFunctorMaker
 * \brief RhoGJFunctorMaker class factory
 *
 *  Makes RhoGJFunctor object of requested type
 */
class RhoGJFunctorMaker
{
public:
  //! constructor: only sets input file
  RhoGJFunctorMaker(FileInput& in): _Input(in) {};

  //! make RhoGJ class object from config file
  RhoGJFunctor* pMake( string type ) const;

private:
  FileInput& _Input;     //! pointer to the input class
};

#endif
