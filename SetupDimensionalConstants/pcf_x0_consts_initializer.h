#ifndef PCF_X0_CONSTS_H
#define PCF_X0_CONSTS_H


#include "consts_initializer.h"
#include "norm_consts.h"
#include "magnetic_field_consts.h"


/**
 * \ingroup setup_consts_grp
 *
 * \class PcfX0_Consts
 * \brief Constants Initializer: initialized all constants using Pcf and \f$ x_0 \f$
 * 
 */
class PcfX0_ConstsInitializer: public ConstsInitializer,
                               public NormConsts, 
                               public MagneticFieldConsts
{
public:
  //! Setup from config file group
  void SetupFromConfigGroup(FileInput &in);
};

#endif
