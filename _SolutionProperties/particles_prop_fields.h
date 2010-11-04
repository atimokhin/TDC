#ifndef PARTICLES_PROP_FIELDS_H
#define PARTICLES_PROP_FIELDS_H

#include "../_Fields/My_Fields.h"

#include "../_Particles/charged_patch.h"
#include "../_Particles/pairs_patch.h"

#include "../SetupProperties/fmp_props.h"


/**
 * \ingroup solution_properties_grp
 *
 * \class ParticlesPropFields
 * \brief Class with fields of particle properties
 * It contains both field containers and methods for their calculation
 * 
 * Contains:
 * -n Particle number density
 *
 */
template<class Field_t>
class ParticlesPropFields: public My_Fields<Field_t>
{
public:
  // Typedefs --------------------------------------------
  typedef My_Fields<Field_t>          Base_t;
  typedef typename Base_t::Mesh_t     Mesh_t;
  typedef typename Base_t::FLayout_t  FLayout_t;
  // -----------------------------------------------------

public:
  //! Constructor 
  ParticlesPropFields(); 

  void SetupFromConfigGroup(FileInput& in);

  void Initialize(const FLayout_t &layout, const Mesh_t &mesh );

  template<class P>
  void Scatter_N_Charged( P& particles);
  template<class Pairs>
  void Scatter_N_Pairs( Pairs& particles, double t);

public:
  //! Particle number density
  Field_t N;

  //! Differential particle number density
  double _E[3];
  Field_t N1;
  Field_t N2;
  Field_t N3;
  Field_t N4;

  

  //! cell centers
  Field_t X;
  //! cell spacings
  double dX; 
};



/**
 * Constructor (adds Rho and E_acc to the list of saved fields)
 * 
 */
template<class Field_t>
ParticlesPropFields<Field_t>::ParticlesPropFields() 
{
  AddSavedField("N",N);

  AddSavedField("N1",N1);
  AddSavedField("N2",N2);
  AddSavedField("N3",N3);
  AddSavedField("N4",N4);
};

template<class Field_t>
void ParticlesPropFields<Field_t>::SetupFromConfigGroup(FileInput& in)
{  
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup("ParticlesProperties");

  for (int i=0; i<3; i++)
    _E[i] = in.get_param("E",i);
  
  in.ChangeGroup(p_current_group);
  
}


template<class Field_t>
void ParticlesPropFields<Field_t>::Initialize(const FLayout_t &layout, 
                                             const Mesh_t &mesh )
{
  // vertex centered quantities ------------------
  Centering<1> vertex = canonicalCentering<1>(VertexType, Continuous);  
  // vertex positions
  X.initialize(vertex,layout,mesh);

  Interval<1> l_total = X.mesh().totalCellDomain();
  for ( int i=l_total.first(); i<=l_total.last(); i++)
    X(i) = X.mesh().cellPosition(0,i);
  // ---------------------------------------------


  // cell centered quantities ------------------
  Centering<1> cell = canonicalCentering<1>(CellType, Continuous);  

  // particle number density
  N.initialize(cell,layout,mesh);
  N(N.totalDomain())=0;
  // ---------------------------------------------

  // cell-spacing 
  dX = mesh.spacings()(0);

  // differential particle number density
  N1.initialize(cell,layout,mesh);
  N1(N1.totalDomain())=0;
  N2.initialize(cell,layout,mesh);
  N2(N1.totalDomain())=0;
  N3.initialize(cell,layout,mesh);
  N3(N1.totalDomain())=0;
  N4.initialize(cell,layout,mesh);
  N4(N1.totalDomain())=0;
}


template<class Field_t>
template<class P>
void ParticlesPropFields<Field_t>::Scatter_N_Charged( P& particles)
{
  FMPProps fmp;
  
  // set N to zero
  N(N.totalDomain())=0;

  // patch number
  int i_patch = 0;
  // local patch of particles
  Charged_Patch<P>   p(particles,i_patch);
  // local patch of fields
  typename Patch<Field_t>::Type_t N_Patch = N.patchLocal(i_patch);
  typename Patch<Field_t>::Type_t X_Patch = X.patchLocal(i_patch);

  typename Patch<Field_t>::Type_t N1_Patch = N1.patchLocal(i_patch);
  typename Patch<Field_t>::Type_t N2_Patch = N2.patchLocal(i_patch);
  typename Patch<Field_t>::Type_t N3_Patch = N3.patchLocal(i_patch);
  typename Patch<Field_t>::Type_t N4_Patch = N4.patchLocal(i_patch);

  // iterate over particles ---------
  Pooma::blockAndEvaluate();
  int l;
  double delta;
  double n_l, n_lp1;
  double p_par;
  int j_start = 0; 
  int j_end   = p.size()-1;
  for ( int j=j_start; j<=j_end; j++)
    {
      l = LeftCellCenterIndex( p.X(j)(0), dX );
      delta = ( p.X(j)(0) - X_Patch(l) )/dX;
      // CIC
      n_l   =  p.Weight(j)*(1-delta);
      n_lp1 =  p.Weight(j)*delta;

      N_Patch(l)   += n_l;
      N_Patch(l+1) += n_lp1;

      p_par = abs( p.P_par(j) );
      if ( p_par < _E[0] )
        {
          N1_Patch(l)   += n_l;
          N1_Patch(l+1) += n_lp1;
        }
      else if ( p_par < _E[1] )
        {
          N2_Patch(l)   += n_l;
          N2_Patch(l+1) += n_lp1;
        }
      else if ( p_par < _E[2] )
        {
          N3_Patch(l)   += n_l;
          N3_Patch(l+1) += n_lp1;
        }
      else 
        {
          N4_Patch(l)   += n_l;
          N4_Patch(l+1) += n_lp1;
        }        
    }
  // --------------------------------
  Pooma::blockAndEvaluate();

  // normalize n(x) to n_GJ
  N(N.totalDomain()) *= fmp.W0()/dX;

  N1(N1.totalDomain()) *= fmp.W0()/dX;
  N2(N2.totalDomain()) *= fmp.W0()/dX;
  N3(N3.totalDomain()) *= fmp.W0()/dX;
  N4(N4.totalDomain()) *= fmp.W0()/dX;
}

template<class Field_t>
template<class Pairs>
void ParticlesPropFields<Field_t>::Scatter_N_Pairs( Pairs& pairs, double t)
{
  FMPProps fmp;
  
  // set N to zero
  N(N.totalDomain())=0;

  // patch number
  int i_patch = 0;
  // local patch of particles
  Pairs_Patch<Pairs>   p(pairs,i_patch);
  // local patch of fields
  typename Patch<Field_t>::Type_t N_Patch = N.patchLocal(i_patch);
  typename Patch<Field_t>::Type_t X_Patch = X.patchLocal(i_patch);

  typename Patch<Field_t>::Type_t N1_Patch = N1.patchLocal(i_patch);
  typename Patch<Field_t>::Type_t N2_Patch = N2.patchLocal(i_patch);
  typename Patch<Field_t>::Type_t N3_Patch = N3.patchLocal(i_patch);
  typename Patch<Field_t>::Type_t N4_Patch = N4.patchLocal(i_patch);

  // iterate over pairs ---------
  Pooma::blockAndEvaluate();
  double x_ph, dx_ph, v_ph; // photon coordinates, total path, and velocity
  int l;
  double delta;
  double n_l, n_lp1;
  int j_start = 0; 
  int j_end   = p.size()-1;

  std::cout<<" test 0 "<<std::flush;

  for ( int j=j_start; j<=j_end; j++)
    {
      // x = x_em + v_ph*(t-t_em), where:
      // v_ph = sign(dx_ph); dx_ph = x_cr-x_em;  t_em = t_cr - abs( dx_ph )
      dx_ph = p.X_cr(j)(0) - p.X_em(j)(0); 
      v_ph = (dx_ph > 0) ? 1 : -1;
      x_ph = p.X_em(j)(0) + v_ph*( t - (p.T_cr(j) - abs(dx_ph)) );

      l = LeftCellCenterIndex( x_ph, dX );
      delta = ( x_ph - X_Patch(l) )/dX;
      // CIC
      n_l   =  p.Weight(j)*(1-delta);
      n_lp1 =  p.Weight(j)*delta;

      N_Patch(l)   += n_l;
      N_Patch(l+1) += n_lp1;

      if ( p.E(j) < _E[0] )
        {
          N1_Patch(l)   += n_l;
          N1_Patch(l+1) += n_lp1;
        }
      else if ( p.E(j) < _E[1] )
        {
          N2_Patch(l)   += n_l;
          N2_Patch(l+1) += n_lp1;
        }
      else if ( p.E(j) < _E[2] )
        {
          N3_Patch(l)   += n_l;
          N3_Patch(l+1) += n_lp1;
        }
      else 
        {
          N4_Patch(l)   += n_l;
          N4_Patch(l+1) += n_lp1;
        }        
    }
  // --------------------------------
  Pooma::blockAndEvaluate();

  std::cout<<" test 1 "<<std::flush;

  // normalize n(x) to n_GJ
  N(N.totalDomain()) *= fmp.W0()/dX;
  std::cout<<" test 2 "<<std::flush;

  std::cout<<" domain1 "<<N1.totalDomain()<<std::flush;
  N1(N1.totalDomain()) *= fmp.W0()/dX;
  std::cout<<" domain2 "<<N2.totalDomain()<<std::flush;
  N2(N2.totalDomain()) *= fmp.W0()/dX;
  std::cout<<" domain3 "<<N3.totalDomain()<<std::flush;
  std::cout<<" N3 "<<N3<<std::flush;
  N3(N3.totalDomain()) *= fmp.W0()/dX;
  std::cout<<" domain4 "<<N4.totalDomain()<<std::flush;
  N4(N4.totalDomain()) *= fmp.W0()/dX;
  std::cout<<" test 6 "<<std::flush;
}


#endif

















