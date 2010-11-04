#ifndef X0__LD_GJ_CONSTS_H
#define X0__LD_GJ_CONSTS_H


#include "consts_initializer.h"
#include "norm_consts.h"
#include "magnetic_field_consts.h"


/**
 * \ingroup setup_consts_grp
 *
 * \class X0__lD_GJ_Consts
 * \brief Constants Initializer: \f$ x_0 = \lambda_{Debye,GJ}\f$
 * 
 */
class X0__lD_GJ_ConstsInitializer: public ConstsInitializer,
                                   public NormConsts, 
                                   public MagneticFieldConsts
{
public:
  //! Setup from config file group
  void SetupFromConfigGroup(FileInput &in);
};

#endif
