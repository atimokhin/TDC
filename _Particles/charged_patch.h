#ifndef CHARGED_PATCH_H
#define CHARGED_PATCH_H

#include "My_Particles_Patch.h"

/**
 * \ingroup particles_grp
 * \class   Charged_Patch
 *
 * class containing a local patch of charged particles
 * 
 */
template <class CP>
class Charged_Patch: public My_Particles_Patch<CP>
{
  typedef My_Particles_Patch<CP> Base_t;

public:

  Charged_Patch(CP& p, int patch):
    My_Particles_Patch<CP>(p,patch),
    X(p.X.patchLocal(patch)),
    P_par(p.P_par.patchLocal(patch)),
    P_perp(p.P_perp.patchLocal(patch)),
    M(p.M), Q(p.Q)
  {};

  using Base_t::size;

public:

  using Base_t::LptData;
  using Base_t::N_BASE;

  const double M;
  const double Q;

  using Base_t::Weight;
  using Base_t::Origin;
  using Base_t::IDTS;
  using Base_t::ID;

  typename Base_t::Patch_Point_t  X;
  typename Base_t::Patch_Scalar_t P_par;

  typename Base_t::Patch_Scalar_t P_perp;
};

#endif
