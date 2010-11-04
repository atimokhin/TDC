#ifndef EMFIELDS_PATCH_H
#define EMFIELDS_PATCH_H


#include "My_Fields_Patch.h"

template<class F>
struct EMFields_Patch: public My_Fields_Patch<F>
{
  typedef My_Fields_Patch<F> Base_t;

  EMFields_Patch(F& f, int patch):
    E(f.E.patchLocal(patch)),
    Rho(f.Rho.patchLocal(patch)),
    J(f.J.patchLocal(patch)),
    X(f.X.patchLocal(patch)),
    RhoGJ(f.RhoGJ.patchLocal(patch))
  {};
    
  typename Base_t::Patch_Field_t E;
  typename Base_t::Patch_Field_t Rho;
  typename Base_t::Patch_Field_t J;

  typename Base_t::Patch_Field_t X;
  typename Base_t::Patch_Field_t RhoGJ;
};

#endif
