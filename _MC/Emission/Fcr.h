#ifndef FCR_H
#define FCR_H

#include <cmath>

#include "ATbase.h"


namespace Fcr_Const{

  const double X_SMALL = 0.5;
  const double X_LARGE = 10;

  const string filename = "_MC/Emission/Fcr.h5";
};


class Fcr
{
public:

  //! Constructor
  Fcr(){ _pItpl = new Polint_1D_hdf5(Fcr_Const::filename); };
  //! Destructor
  ~Fcr(){ delete _pItpl; };

  double operator() (double x);

private:

  Polint_1D_hdf5 *_pItpl;
};


inline double Fcr::operator() (double x)
{
  if ( x < Fcr_Const::X_SMALL ) 
    return 1 + 0.346410161513775*x - pow(x,.333333333333333)*(1.231588961841031  + 0.032988990049313*x*x);
  else if ( x < Fcr_Const::X_LARGE ) 
    return (*_pItpl)(x);
  else
    return 0;
}

#endif
