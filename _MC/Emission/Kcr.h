#ifndef KCR_H
#define KCR_H

#include <cmath>

#include "ATbase.h"


namespace Kcr_Const{

  const double X_SMALL = 0.1;
  const double X_LARGE = 10;

  const string filename = "_MC/Emission/Kcr.h5";
};


class Kcr
{
public:

  //! Constructor
  Kcr(){ _pItpl = new Polint_1D_hdf5(Kcr_Const::filename); };
  //! Destructor
  ~Kcr(){ delete _pItpl; };

  double operator() (double x);

private:

  Polint_1D_hdf5 *_pItpl;
};


inline double Kcr::operator() (double x)
{
  if ( x < Kcr_Const::X_SMALL ) 
    return 2.1495282415344787*pow(x,-2e0/3) - 1.8137993642342176;
  else if ( x < Kcr_Const::X_LARGE ) 
    return (*_pItpl)(x);
  else
    return 1.253 * exp(-x)/sqrt(x);
}

#endif
