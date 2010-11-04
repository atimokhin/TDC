#ifndef PARTICLE_FLUX_H
#define PARTICLE_FLUX_H

#include <list>

#include "../PhysicsLib/small_functions.h"

#include "QuantitiesVsTime.h"


/**
 * \ingroup solution_properties_grp
 *
 * Container for particle flux
 * stores
 *
 * - particle number density flux
 * - energy flux
 * - current J
 *
 */
class ParticleFlux: public QuantitiesVsTime
{
public:

  //! Constructor
  ParticleFlux(int chunksize = 10);
  //! Destructor
  ~ParticleFlux();

  //! setup from config file group
  void SetupFromConfigGroup(FileInput& in);
  //! setup from config file group and read data from pervious calculations
  void SetupFromHDFFileAndConfigGroup(FileInput& in);

  //! Save each element into an HDF file 
  bool SaveToHDFFile();

  template<class P>
  void UpdateFlux(std::list<int>& idxs, const P& p, double t);

  //! print content 
  void Print(ostream& o = std::cout) const;

public:

  std::deque<double> NFlux;
  std::deque<double> EnergyFlux;
  std::deque<double> J;
};


/**
 * \ingroup qvt_group
 *
 * names for HDF datasets associated with QuantitiesVsTime
 * 
 */
namespace ParticleFlux_names{

  using namespace std;

  const string N_FLUX__DATASET_NAME      = "N";
  const string ENERGY_FLUX__DATASET_NAME = "Energy";
  const string J__DATASET_NAME           = "J";
};



template<class P>
void ParticleFlux::UpdateFlux(std::list<int>& idxs, const P& p, double t)
{
  double fn = 0;
  double fe = 0;
  double j  = 0;

  // calculate current and energy flux due to current set of particles
  while ( !idxs.empty() )
    {
      int i = *( idxs.begin() );
      idxs.pop_front();

      fn += p.Weight(i);
      fe += p.Weight(i) * p.M * SF::P2E( p.P_par(i) );
      j  += p.Weight(i) * p.Q;
    }

  // strore J and Energy flux
  if ( GetCurrentTime() == t )
    {
      NFlux[GetICurrent()]      += fn;
      EnergyFlux[GetICurrent()] += fe;
      J[GetICurrent()]          += j;
    }
  else
    {
      AddNewValue(TimeArray,t);
      SetCurrentTime(t);      

      AddNewValue(NFlux, fn);
      AddNewValue(EnergyFlux, fe);
      AddNewValue(J, j);
    }
}

#endif
