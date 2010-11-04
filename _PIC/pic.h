#ifndef PIC_H
#define PIC_H

#include <cmath>

#include "Pooma/Fields.h"
#include "Pooma/Particles.h"

#include "../PhysicsLib/small_functions.h"

#include "../SetupDimensionalConstants/norm_consts.h"

#include "../SetupParameters/geometry_params.h"
#include "../SetupProperties/fmp_props.h"

#include "../_SolutionProperties/particle_flux.h"

#include "../_Particles/particle_list.h"
#include "../_Particles/charged_patch.h"

#include "../_Fields/em_fields.h"
#include "../_Fields/em_fields_patch.h"

#include "../_Control/code_control.h"

#include "cic_interpolators.h"


/**
 * \defgroup pic_grp {PIC} Particle In Cell
 * 
 */

/**
 * \ingroup pic_grp
 *
 * \class ParticleInCell<>
 *
 * ParticleInCell<Field_t,Particles_t,InterpolatorTag_t>. This class
 * contains all PIC related routines. It is parametrized on field
 * type and particle type
 * 
 * <B> Boundary conditions description is in
 * SetupFromConfig(FileInput& in) function </B>
 *
 * configuration file example:
 \verbatim

    Group "PIC" {
       Group "BoundaryConditions" {
             Type = 'FreeFlowBothEnds';
          }
    }
 
 \endverbatim
 *
 * following SetupParametrs are requied:
 * - CodeControl
 * - GeometryParams
 *
 */
template<class EM, class P>
class ParticleInCell
{
  // BC types ----------------------------------
  enum BC_Type { BC_NONE,
                 BC_FREE_FLOW__BOTH_ENDS, 
                 BC_FREE_FLOW__TOP, BC_FREE_FLOW__BOTTOM };
  // -------------------------------------------

public:
  //! constructor
  ParticleInCell();

  //! setup from config file
  void SetupFromConfig(FileInput& in);

  //! Move particles, scatter Rho & J, and collect particle flux (top level function)
  void MoveAndScatterParticles(ParticleList<P>& plist, EM& em,  
			       ParticleFlux& ns_flux, ParticleFlux& lc_flux,
			       double t, double dt);

  //! Solve field equations
  void SolveFieldEquations(EM& em, double t, double dt);
  //! Enforce Gauss law after merging particles
  void EnforceGaussLaw(ParticleList<P>& pl, EM& em, double t, double dt );


  //! Initial Electromagnetic field configuration
  void CalculateInitialField(EM& em, double dt);
  //! Calculate velocity for the first timestep (top level fuction)
  void CalculateInitialMomentum( ParticleList<P>& plist, EM& em, double dt );

  //! scatter charge density to mesh points (top level fuction)
  void ScatterRho( EM& em, ParticleList<P>& plist);


private:

  //! move and scatter Rho and J for each particle specie
  void MoveAndScatterParticleSpecie(P& p, EM& em,  double dt);
  //! iterate over particles move it and scatter Rho and J
  template<class InterpolatorFunctor>
  void MoveAndScatter_Itr( P& p, EM& em,  double dt);

  //! check whether paricle is outside of calculation domain
  bool DoChangeMomentum(double x);

  //! apply general BC to particle specie, collect flux, and perform destroys
  void CollectParticleSpecieFlux(P& p, 
				 ParticleFlux& ns_flux, ParticleFlux& lc_flux,
				 double t);

  //! Calculate velocity for the first timestep for each specie individually
  void CalculateInitialMomentum_Itr(P& p, EM& em, double dt);

  //! this function aterates over particles and scatter charge
  template<class InterpolatorFunctor>
  void ScatterRho_Itr( EM& em, P& p);


private:

  //! coefficient on the equation of motion
  double _P_CF;
  //! length of the domain
  double _L;

  //! Type of boundary conditions
  BC_Type _BC_Type;
};


/**
 * Constructor
 * 
 */
template<class EM, class P>
ParticleInCell<EM,P>::ParticleInCell()
{
  _P_CF = NormConsts().Pcf();
  _L = GeometryParams().L();
};


/** 
 * <B> Sets up boundary conditions for particle motion at the ends of
 * computational domain </B>
 *
 * Types of boundary conditions:
 *  
 * - "FreeFlowTop"      -- particles flow freely in the top boundary cell
 *
 * - "FreeFlowBottom"   -- particles flow freely in the bottom boundary cell
 *
 * - "FreeFlowBothEnds" -- particles flow freely in both boundary cells
 *
 * - "None"             -- particles feel the boundary electric field
 * 
 * @param in  -- FileInput object associated with config file
 *
 */
template<class EM, class P>
void ParticleInCell<EM,P>::SetupFromConfig(FileInput& in)
{
  std::string bc_name;

  // **********************************************
  // Boundary conditions 
  // **********************************************
  in.ChangeGroup("PIC");
  in.ChangeGroup("BoundaryConditions");
  bc_name = in.get_name("Type");

  if ( bc_name == "FreeFlowTop" )
    {
      _BC_Type = BC_FREE_FLOW__TOP;
    }
  else if ( bc_name == "FreeFlowBottom" ) 
    {
      _BC_Type = BC_FREE_FLOW__BOTTOM;
    }
  else if ( bc_name == "FreeFlowBothEnds" ) 
    {
      _BC_Type = BC_FREE_FLOW__BOTH_ENDS;
    }
  else if ( bc_name == "None" )
    {
      _BC_Type = BC_NONE;
    }
  else 
    {
      cout<<"Wrong boundary condition type for PIC :"<<bc_name<<"!\n";
      exit(1);
    }
  in.ChangeGroup();
}



template<class EM, class P>
void ParticleInCell<EM,P>::MoveAndScatterParticles( ParticleList<P>& plist, EM& em, 
						    ParticleFlux& ns_flux, ParticleFlux& lc_flux,
						    double t, double dt)
{
  FMPProps fmp;

  CodeControl cc;

  em.ClearRho();
  em.ClearJ();

  // Move each particle specie ********************************
  for (int i=0; i<plist.Size(); i++)
    MoveAndScatterParticleSpecie(*plist[i], em, dt);


  // Collect particle flux trough the boundaries **************
  // particles leaving computation domain are destroyed here !!
  for (int i=0; i<plist.Size(); i++)
    CollectParticleSpecieFlux(*plist[i], ns_flux,lc_flux, t);

  // swap
  plist.Swap();

  if ( cc.DoGatherRhoAndJ() ) 
    {
      em.ApplyBoundaryConditionsToRhoAndJ();  
      em.ApplyDigitalFilter();
      // normalization of current and charge density
      em.Rho(em.Rho.totalDomain()) *= fmp.W0()/Pooma::cellVolumes(em.Rho).comp(0).read(0);
      em.J(em.J.totalDomain())     *= fmp.W0()/dt;
    }
  else
    {
      em.ClearRho();
      em.ClearJ();
    }
}


/**
 * For each particle specie call 
 * MoveAndScatter_Itr( P& p, EM& em, double dt ) 
 * specialized on the specific version of InterpolatorFunctor
 * 
 */
template<class EM, class P>
inline void ParticleInCell<EM,P>::MoveAndScatterParticleSpecie( P& p, EM& em, double dt )
{
  if ( p.Q == -1 )
    MoveAndScatter_Itr< CIC_ScatterUnitNegativeChargeFunctor >( p, em, dt);
  else if ( p.Q == 1 )
    MoveAndScatter_Itr< CIC_ScatterUnitPositiveChargeFunctor >( p, em, dt);
  else 
    MoveAndScatter_Itr< CIC_ScatterFunctor >( p, em, dt);
}


/**
 * Iterates over individual particles:
 * - gather electric field to the particle's old location
 * - move the particle and scatter Rho and J
 * 
 */
template<class EM, class P>
template<class InterpolatorFunctor>
void ParticleInCell<EM,P>::MoveAndScatter_Itr( P& particles, EM& em_fields, double dt )
{
  Pooma::blockAndEvaluate();

  // patch number
  int i_patch = 0;
  // local patch of particles
  Charged_Patch<P>   p(particles,i_patch);
  // local patch of fields
  EMFields_Patch<EM> em(em_fields,i_patch);


  double h =  em.X.mesh().spacings()(0); 
  double p_coeff = dt * _P_CF * p.Q/p.M ;

  int j_start = 0; 
  int j_end   = p.size()-1;

  //
  // j       - particle index
  // l       - cell index
  // i,i_old - vertex index
  //
  for ( int j=j_start; j<=j_end; j++)
    {
      int l;         // charge density indexies
      int i, i_old;  // curret density indexies

      double delta;  // relative distance from cell center
      double E;      // electric field at particle's position
      
      double q_old_l, q_old_lp1, q_l, q_lp1; // charges at cell centers
      

      // .......................................
      // @ old particle position ***************
      l = LeftCellCenterIndex( p.X(j)(0), h );
      i_old = l+1;
      delta = ( p.X(j)(0) - em.X(l) )/h;

      // scatter charge : old step -------------
      InterpolatorFunctor()(p.Q, p.Weight(j), delta, q_old_l, q_old_lp1 );
      // ***************************************


      // Move particle *************************      
      if ( DoChangeMomentum( p.X(j)(0) ) )
        {
          // gather electric field 
          CIC_GatherFunctor()( delta, em.E(i_old-1), em.E(i_old), em.E(i_old+1), E);
          // advance particle momentum
          p.P_par(j) += p_coeff * E;
        }

      // move particle
      p.X(j) += dt * SF::P2Beta(p.P_par(j));
      // ***************************************
 

      // @ new particle position ***************
      l = LeftCellCenterIndex( p.X(j)(0), h );
      i = l+1;
      delta = ( p.X(j)(0) - em.X(l) )/h;
 
      // scatter charge : new step -------------
      InterpolatorFunctor()(p.Q, p.Weight(j), delta, q_l, q_lp1 );
      // .......................................


      // charge density
      // CIC *******************
      em.Rho(l)   += q_l;
      em.Rho(l+1) += q_lp1;
      // ***********************

      // current density
      if ( i == i_old+1 )      // particle moved to the right cell
	{	      
	  em.J(i_old)   += q_old_l;
	  em.J(i_old+1) += q_lp1;
	}
      else if ( i == i_old-1 ) // particle moved to the left cell
	{
	  em.J(i_old)   -= q_old_lp1;
	  em.J(i_old-1) -= q_l;
	}
      else if ( i == i_old )   // particle stays in the same cell
	{
	  em.J(i_old) -= (q_l - q_old_l);
	}
      else
	{
	  cout<<"ParticleInCell<EM,P>::MoveAndScatter_Itr :";
	  cout<<" Error! a particle moves by more that 1 cell!\n";
	  cout<<" i_old="<<i_old<<" i="<<i<<"\n";
	  exit(1);
	}
      // ***************************************		
     }
}


/** 
 * Returns True if particle momentum is to be changed by the electric
 * field: at the end cells particle can move freely,  not feeeling
 * the boundary electric field
 * 
 * @param x  -- particle position
 * 
 * @return True if particle momentum is to be changed
 */
template<class EM, class P>
inline bool ParticleInCell<EM,P>::DoChangeMomentum(double x)
{
  switch ( _BC_Type ) 
    {
    case BC_FREE_FLOW__TOP:
      return  ( x <= _L );
    case BC_FREE_FLOW__BOTH_ENDS:
      return  ( x <= _L && x >= 0 );
    case BC_FREE_FLOW__BOTTOM:
      return  ( x >= 0 );
    default:
      return true;
    }
}


/**
 * - applies boundary conditions to a particle specie
 * - get destry list
 * - iterates over the destry list and collects indexies of particles leaving
 *    trough lower and upper boundaries separatly
 * - updates flux containers by calling UpdateFlux with the corresponding lists
 * - destroys particles leaving the volume
 * 
 */
template<class EM, class P>
void ParticleInCell<EM,P>::CollectParticleSpecieFlux(P& p, 
						     ParticleFlux& ns_flux, ParticleFlux& lc_flux,
						     double t)
{
  std::list<int> idx_ns_flux;
  std::list<int> idx_lc_flux;

  int i_patch = 0;

  // apply boundaryc conditions and get destroy list
  p.applyBoundaryConditions(i_patch);
  DynamicArray<int> detsroy_list = p.destroyList(i_patch);
  
  // calculate fluxes of current particle specie
  for (int j=0; j<detsroy_list.size(); j++)
    {
      // nc flux --
      if ( p.X(detsroy_list(j))(0) < 0 )
	idx_ns_flux.push_back( detsroy_list(j) );
      // lc flux --
      else if ( p.X(detsroy_list(j))(0) > _L )
	idx_lc_flux.push_back( detsroy_list(j) );
    }
  // store fluxes in corresponding flux containers
  ns_flux.UpdateFlux(idx_ns_flux, p, t);
  lc_flux.UpdateFlux(idx_lc_flux, p, t);
  
  // destroy particles leaving domains
  p.performDestroy();
}





template<class EM, class P>
void ParticleInCell<EM,P>::CalculateInitialMomentum( ParticleList<P>& plist, EM& em, double dt )
{
  // Move each particle specie
  for (int i=0; i<plist.Size(); i++)
    CalculateInitialMomentum_Itr(*plist[i], em, dt);
}


template<class EM, class P>
void ParticleInCell<EM,P>::CalculateInitialMomentum_Itr( P& particles, EM& em_fields, double dt )
{
  // patch number
  int i_patch = 0;
  // local patch of particles
  Charged_Patch<P>   p(particles,i_patch);
  // local patch of fields
  EMFields_Patch<EM> em(em_fields,i_patch);

  double h =  em.X.mesh().spacings()(0); 
  double p_coeff = dt * _P_CF * p.Q/p.M;

  int j_start = 0; 
  int j_end   = p.size()-1;

  for ( int j=j_start; j<=j_end; j++)
    {
      int l = LeftCellCenterIndex( p.X(j)(0), h );
      int i = l+1;
      double delta = ( p.X(j)(0) - em.X(l) )/h;
      
      double E;
      CIC_GatherFunctor()( delta, em.E(i-1), em.E(i), em.E(i+1), E);

      // Advance particle momentum
      p.P_par(j) -= 0.5*p_coeff * E;
    }
}




template<class EM, class P>
void ParticleInCell<EM,P>::ScatterRho( EM& em, ParticleList<P>& plist)
{
  FMPProps fmp;
  em.ClearRho();

  for (int i=0; i<plist.Size(); i++)
    {
      if (  plist[i]->Q == -1 )
	ScatterRho_Itr< CIC_ScatterUnitNegativeChargeFunctor >(em, *plist[i]);
      else if ( plist[i]->Q == 1 )
	ScatterRho_Itr< CIC_ScatterUnitPositiveChargeFunctor >(em, *plist[i]);
      else 
	ScatterRho_Itr< CIC_ScatterFunctor >( em, *plist[i]);
    }

  em.ApplyBoundaryConditionsToRhoAndJ(); 
  em.ApplyDigitalFilter(); 
 
  em.Rho(em.Rho.totalDomain()) *= fmp.W0()/Pooma::cellVolumes(em.Rho).comp(0).read(0);
}



template<class EM, class P>
template<class InterpolatorFunctor>
void ParticleInCell<EM,P>::ScatterRho_Itr( EM& em_fields, P& particles)
{
  // patch number
  int i_patch = 0;
  // local patch of particles
  Charged_Patch<P>   p(particles,i_patch);
  // local patch of fields
  EMFields_Patch<EM> em(em_fields,i_patch);


  double h =  em.X.mesh().spacings()(0); 

  int j_start = 0; 
  int j_end   = p.size()-1;

  for ( int j=j_start; j<=j_end; j++)
    {
      int l = LeftCellCenterIndex( p.X(j)(0), h );
      double delta = ( p.X(j)(0) - em.X(l) )/h;

      // CIC *******************
      double q_l, q_lp1;
      InterpolatorFunctor()(p.Q, p.Weight(j), delta, q_l, q_lp1 );

      em.Rho(l)   += q_l;
      em.Rho(l+1) += q_lp1;
      // ***********************
    }
}




template<class EM, class P>
inline void ParticleInCell<EM,P>::SolveFieldEquations( EM& em, double t, double dt ) 
{
  em.EvolveElectroMagneticField(t,dt);
}

template<class EM, class P>
inline void ParticleInCell<EM,P>::EnforceGaussLaw( ParticleList<P>& pl, 
						   EM& em, 
						   double t, double dt ) 
{
  ScatterRho( em, pl );
  em.EnforceGaussLaw(t,dt);
}


//! Initial Electromagnetic field configuration
template<class EM, class P>
inline void ParticleInCell<EM,P>::CalculateInitialField(EM& em,  double dt)
{
  em.InitialField();
}

#endif





