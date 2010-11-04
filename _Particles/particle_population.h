#ifndef PARTICLE_POPULATION_H
#define PARTICLE_POPULATION_H

#include <string>
#include <map>
#include <vector>

#include "ATbase.h"

#include "../inout/save2hdf.h"

#include "My_Particles_Patch.h"

/**
 * \ingroup particles_grp
 * \class ParticlePopulation
 *
 * This small class keeps track of particle population. In its
 * internal array it contains number of particles of different classes.
 * These numbers are updated when method Update(P& p) is called.
 * It can save/read this array to/from HDF file
 * 
 * Storage in HDF file:
 * - /PP/Numbers    - numbers of particles (array)
 * - /PP/Weights    - weights of particles (array)
 * - /PP/MeanWeight - mean weight of macroparticle
 */
class ParticlePopulation
{
  typedef std::map<char,int>  Map_t;

public:
  //! Constructor
  ParticlePopulation();
  //! Destructor
  ~ParticlePopulation();

  //! updade particle distribution data
  template<class P>
  void Update(P& p);

  int  GetTotalNumberOfParticles() const { return _ParticleNumbers[0]; };
  vector<int> GetParticleNumbers() const { return _ParticleNumbers; };

  //! Save each element into an HDF file as "name/id"
  bool SaveToHDFFile(std::string id, Save2HDF &hdf);
  //! Read each element from the HDF file dataset "name/id"
  bool ReadFromHDFFile(std::string id, Save2HDF &hdf);


private:
  
  int         _TableSize;
  Map_t       _CodingTable;

  vector<int>    _ParticleNumbers;
  vector<double> _ParticleWeights;

  double         _MeanWeight;
};



/** 
 * Iterates over particles and update statistics of each particle sort
 * according to its origin
 * 
 * @param p reference to a particular particle class 
 */
template<class P>
void ParticlePopulation::Update(P& particles)
{
//   typename Patch<typename P::Attribute_Scalar_t>::Type_t weight = p.Weight.patchLocal(0);
//   typename Patch<typename P::Attribute_CHAR_t>::Type_t   origin = p.Origin.patchLocal(0);

  int i_patch = 0;
  My_Particles_Patch<P> p(particles,i_patch);

  // set all numbers to zero
  for( int i=0; i<_ParticleNumbers.size(); i++ ) 
    {
      _ParticleNumbers[i]=0;
      _ParticleWeights[i]=0;
    }

  // total number of particles
  int n_total = p.size();
  _ParticleNumbers[0] = n_total;

  // iterate over particles
  for ( int i=0; i<n_total; i++ )
    {
      // total weight
      _ParticleWeights[0] += p.Weight(i);

      // iterate over coding table
      for ( Map_t::iterator iter=_CodingTable.begin(); 
	    iter != _CodingTable.end(); 
	    iter++ )
	{
	  if ( p.Origin(i) == iter->first )
	    {
	      _ParticleNumbers[iter->second]++;
	      _ParticleWeights[iter->second] += p.Weight(i);
	    }
	}
    }

  // calculate mean weight
  _MeanWeight = ( n_total > 0 ? _ParticleWeights[0]/n_total : 0 );
}

#endif
