#ifndef CIC_INTERPOLATORS
#define CIC_INTERPOLATORS


inline int LeftCellCenterIndex(double x, double h)
{
  int l = static_cast<int>( x/h - 0.5 );

  return (x<0.5*h ? l-1 : l );
}



struct CIC_GatherFunctor 
{
  void operator() (double delta, double E_im1, double E_i, double E_ip1, double& E)
  {
    E = 0.5*( E_im1*(1-delta) + E_i + E_ip1*delta );
  }
};




struct CIC_ScatterFunctor 
{
  void operator() (double q, double weight, double delta, double& q_l, double& q_lp1)
  {
    q_l   = q*weight*(1-delta);
    q_lp1 = q*weight*delta;
  }
};


/**
 * Scatter functor for <b> positive </b> charges
 * 
 */
struct CIC_ScatterUnitPositiveChargeFunctor 
{
  void operator() (double q, double weight, double delta, double& q_l, double& q_lp1)
  {
    q_l   = weight*(1-delta);
    q_lp1 = weight*delta;
  }  
};


/**
 * Scatter functor for <b> negative </b> charges
 * 
 */
struct CIC_ScatterUnitNegativeChargeFunctor 
{ 
  void operator() (double q, double weight, double delta, double& q_l, double& q_lp1)
  {
    q_l   =  weight*(delta-1);
    q_lp1 = -weight*delta;
  }
};

#endif
