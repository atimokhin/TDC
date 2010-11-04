#ifndef MY_FIELDS_PATCH_H
#define MY_FIELDS_PATCH_H

template<class F>
struct My_Fields_Patch
{
  typedef typename F::Patch_Field_t::Type_t    Patch_Field_t;
  typedef typename F::Patch_FieldAux_t::Type_t Patch_FieldAux_t;
};

#endif
