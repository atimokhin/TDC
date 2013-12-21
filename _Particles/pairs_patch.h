#ifndef PAIRS_PATCH_H
#define PAIRS_PATCH_H

#include "My_Particles_Patch.h"


/**
 * \ingroup particles_grp
 * \class   Pairs_Patch
 *
 * class containing a local patch of Pairs
 * 
 */
template <class CP>
class Pairs_Patch: public My_Particles_Patch<CP>
{
  typedef My_Particles_Patch<CP> Base_t;

public:

  Pairs_Patch(CP& p, int patch):
    My_Particles_Patch<CP>(p,patch),
    IDTS_Parent(p.IDTS_Parent.patchLocal(patch)),
    ID_Parent(p.ID_Parent.patchLocal(patch)),
    T_cr(p.T_cr.patchLocal(patch)),
    X_cr(p.X_cr.patchLocal(patch)),
    X_em(p.X_em.patchLocal(patch)),
    E(p.E.patchLocal(patch)),
    Psi(p.Psi.patchLocal(patch))
  {};

  // size of the patch
  using Base_t::size;

public:

  using Base_t::LptData;
  using Base_t::N_BASE;

  using Base_t::Weight;
  using Base_t::Origin;
  using Base_t::IDTS;
  using Base_t::ID;

  typename Base_t::Patch_INT_t IDTS_Parent;
  typename Base_t::Patch_INT_t ID_Parent;

  typename Base_t::Patch_Scalar_t T_cr;
  typename Base_t::Patch_Point_t  X_cr;
  typename Base_t::Patch_Point_t  X_em;
  typename Base_t::Patch_Scalar_t E;
  typename Base_t::Patch_Scalar_t Psi;
};

#endif
