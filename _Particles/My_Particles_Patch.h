#ifndef MY_PARTICLES_PATCH_H
#define MY_PARTICLES_PATCH_H

#include "Pooma/Particles.h"

template <class P>
class My_Particles_Patch
{
protected:

  typedef typename P::Patch_Point_t::Type_t  Patch_Point_t;
  typedef typename P::Patch_Scalar_t::Type_t Patch_Scalar_t;
  typedef typename P::Patch_CHAR_t::Type_t   Patch_CHAR_t;
  typedef typename P::Patch_INT_t::Type_t    Patch_INT_t;

public:

  My_Particles_Patch(P& p, int patch):
    _Patch(patch),
    Weight(p.Weight.patchLocal(patch)),
    Origin(p.Origin.patchLocal(patch)),
    IDTS(p.IDTS.patchLocal(patch)),
    ID(p.ID.patchLocal(patch)),
    N_MAX(p.N_MAX),N_OPT(p.N_OPT),N_BASE(p.N_BASE)
  {};

  // size of the patch
  int size() { return Weight.size(); }

public:

  const int N_MAX;
  const int N_OPT;
  const int N_BASE;

  Patch_Scalar_t Weight;
  Patch_CHAR_t   Origin;
  Patch_INT_t    IDTS;
  Patch_INT_t    ID;

private:

  const int _Patch; // number of the local patch
};

#endif

