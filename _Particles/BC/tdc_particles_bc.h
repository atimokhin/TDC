#ifndef TDC_PARTICLES_BC_H
#define TDC_PARTICLES_BC_H

#include "ATbase.h"

#include "../../SetupParameters/geometry_params.h"
#include "../../SetupParameters/grid_params.h"
#include "../../SetupProperties/pulsar_gap_props.h"

#include "../../_Fields/em_fields.h"

#include "../particle_list.h"
#include "../ID/particle_id.h"


/**
 * \ingroup particles_grp
 * 
 * \defgroup particles_bc_grp   {BC}   Particle Boundary Conditions
 */


/**
 * \ingroup particles_bc_grp
 *
 * \class TDC_ParticlesBC
 * \brief Base virtual class for Particles boundary conditions
 * 
 * it requires the following Setup Parameters
 * - GeometryParams
 * - GridParams
 */
template<class EM, class P>
class TDC_ParticlesBC
{
public:

  //! BC using POOMA BC methods
  virtual void SetupGeneralBC(ParticleList<P>& plist);


  //! Setup particle boundary conditions from config file group
  virtual void SetupFromConfigGroup(FileInput& in) = 0 ;


  /**
   * Boundary conditions applied <b> explicitely </b> in course of
   * cascade modelling at each time step. (i.e. this method is called
   * explicitely in the Cascade::RunSimulations() at each time step)
   *
   * All boundary conditions which <b> cannot be implemented by POOMA
   * boundary conditions methods </b> go here, like:
   * - particle inflow
   * - particle injection from the surface
   * 
   * @param   em    reference to a class with electromagnetic fields
   * @param   plist ParticleList with Charged  partciles
   * @param   p_id  ParticleID, neede if particles are injected
   * @return  true  if boundary conditions were applied, i.e. sync on particles is required
   */
  virtual bool ApplyTimeDependentBC( EM& em, ParticleList<P>& plist, ParticleID& p_id, double t, double dt) = 0;


protected:

  //! types of general boundary conditions
  enum GeneralBCType_t { BC_PERIODIC, BC_OUTFLOW, BC_OUTFLOW_LOW_ENERGY_PARTICLES };

  GeneralBCType_t _GeneralBCType;       //!< General BC type
};





/**
 * General boundary conditions on the particles. All boundary
 * conditions that <b> can be implemented by POOMA boundary conditions
 * methods </b> go here.
 *
 * This method is called once during particles setup in
 * Cascade::SetupFromConfig(FileInput& in)
 * 
 * Implements the following conditions:
 * - BC_PERIODIC - periodic BC
 * - BC_OUTFLOW  - particles freely lives claculation domain 
 *                (are killed in POOMA terms at the boundary)
 *
 */
template<class EM, class P>
void TDC_ParticlesBC<EM,P>::SetupGeneralBC(ParticleList<P>& plist)
{
  GeometryParams geom;
  GridParams     grid;

  if ( _GeneralBCType == BC_PERIODIC )
    {
      // region boundaries
      typename P::PointType_t lower(0.0), upper(geom.L());
      // bc class
      PeriodicBC<typename P::PointType_t> bc(lower,upper);
      for (int i=0; i<plist.Size(); i++)
	  plist[i]->addBoundaryCondition(plist[i]->X,bc);
    }
  
  if ( _GeneralBCType == BC_OUTFLOW )
    {
      double dx = geom.L()/grid.NumberOfCells();
      // region boundaries
      typename P::PointType_t lower(-0.5*dx), upper(geom.L()+0.5*dx);
      // bc class
      KillBC<typename P::PointType_t> bc(lower,upper);
      for (int i=0; i<plist.Size(); i++)
	plist[i]->addBoundaryCondition(plist[i]->X,*plist[i],bc);
    }

}

#endif
