#ifndef PHOTONS_CACHE_H
#define PHOTONS_CACHE_H

#include <iostream>

#include "ATbase.h"

#include "particle_cache.h"

#include "../../utils/direction.h"
#include "../../inout/save2hdf.h"

#include "../../PhysicsLib/small_functions.h"


/**
 * \ingroup monte_carlo_grp
 * 
 * \class Photons
 * 
 * This is a container class for photons emitted by Charged particles.
 * It is used by MonteCarlo class for collection of the emission of
 * <B> each </B> particle. It stores: 
 * - position of the emission point (scalar)
 * - statistical weight of the emitted photons (array)
 * - angle of the outgoing photons relative to the direction of the magnetic field (array)
 * - momentum of  emitted photons (array)
 *  
 * The class has maximum members set by Initialize(int n_max). It
 * neither sets arrays to zeros nor checks the correcteness of the
 * array ranges for the sake of speed.
 *
 * For test purpouses it can also dump it content into an HDF file
 * (see void Save2HDFFile(Save2HDF &hdf) ).
 * 
 */

class PhotonCache: public ParticleCache
{
public:
  PhotonCache();

  //! do initialization
  void Initialize(int n_max);

  //! Set emission point
  void   Set_X0( double x0 );
  //! Get emission point
  double Get_X0() const;

  //! Set direction
  void Set_Direction( double p );
  //! Get direction
  Direction Get_Direction() const;

  //! Get direction sign
  int Get_MomentumSign() const;

  //! Set emisstion time
  void   Set_T0( double t0 );
  //! Get emisstion time
  double Get_T0() const;

  //! Set emitting particle IDTS
  void Set_IDTS( int idts );
  //! Get emitting particle IDTS
  int  Get_IDTS() const;

  //! Set emitting particle ID
  void Set_ID( int id );
  //! Get emitting particle ID
  int  Get_ID() const;

  //! Set emission particle parameters common for all photons
  void SetInheritedParams( double x0, double t0, double p, int idts, int id );

  //! Add a photon to the list
  void Add(double weight, double e, double psi0, char origin);

  //! Save photons to an HDF file
  void Save2HDFFile(Save2HDF &hdf);
  //! Print container content
  virtual void Print(ostream& os) const;

public:
  //! Statistical weght of photons
  std::vector<double> Weight;
  //! Emission angle relative to the magnetic field at X0
  std::vector<double> Psi0;
  //! Photon momentum
  std::vector<double> E;
  
  //! Origin of Photons (mechanism by which photon was emitted)
  std::vector<char>   Origin;
  
private:
  //! Emission point
  double _X0;
  //! Time of emission
  double _T0;
  //! Photons direction
  Direction _Dir;

  //! timestep ID of emitting particle
  int _IDTS;
  //! ID of emitting particle
  int _ID;
};


inline void   PhotonCache::Set_X0( double x0 ) {_X0 = x0; }
inline double PhotonCache::Get_X0() const      { return _X0; }

inline void      PhotonCache::Set_Direction( double p ) { _Dir = ( p > 0  ? UP : DOWN ); }
inline Direction PhotonCache::Get_Direction() const     { return _Dir; }

inline int PhotonCache::Get_MomentumSign() const { return ( _Dir == DOWN ? -1 : 1 ); };

inline void   PhotonCache::Set_T0( double t0 ) { _T0 = t0; }
inline double PhotonCache::Get_T0() const      { return _T0; }

inline void PhotonCache::Set_IDTS( int idts ) { _IDTS = idts; }
inline int  PhotonCache::Get_IDTS() const     { return _IDTS; }

inline void PhotonCache::Set_ID( int id ) { _ID = id; }
inline int  PhotonCache::Get_ID() const   { return _ID; }


inline void PhotonCache::SetInheritedParams( double x0, double t0, double p, int idts, int id )
{
  _X0 = x0;
  _T0 = t0;
  Set_Direction( p );

  _IDTS = idts;
  _ID = id;
}

inline void PhotonCache::Add(double weight, double e, double psi0, char origin)
{
  // check size
  ParticleCache::CheckSize();

  Weight[_i] = weight;
  Psi0[_i]   = psi0;
  E[_i]      = e;

  Origin[_i] = origin;

  // increment index
  ParticleCache::Add();
}

#endif
