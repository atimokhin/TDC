#include <iostream>

#include "../_Particles/charged.h"
#include "../_Particles/pairs.h"

#include "../_Particles/ParticleCaches/pair_cache.h"
#include "../_Particles/ParticleCaches/charged_particle_cache.h"

#include <list>



// Typedefs -----------------------------------

// Mesh type and Point type **************
const   int DIM = 1;
typedef MeshTraits<DIM,
		   double,
		   RectilinearTag,CartesianTag,
		   DIM>::Mesh_t  Mesh_t; 
typedef Mesh_t::PointType_t                         PointType_t; 

// Field type and Field layout ***********
typedef Field<Mesh_t,double,MultiPatch<GridTag,Brick> >  Field_t;
typedef Field_t::Engine_t::Layout_t                      FLayout_t;

// Particle traits and Particles Types
typedef MPDynamicSpatial<Mesh_t,FLayout_t>  PTraits_t;
typedef Charged<PTraits_t>                  CParticles_t;
typedef Pairs<PTraits_t>                    Pairs_t;

typedef Mesh_t::SpacingsType_t  SpacingsType_t;
typedef PTraits_t::ParticleLayout_t   PLayout_t;

// ---------------------------------------------

const int nx =10;

int main(int argc, char *argv[])
{
  using namespace std;

  Pooma::initialize(argc, argv);

  // Create field layout ****************************
  Interval<1>    meshDomain(1,nx+1);
  GuardLayers<1> gl(1);
  Loc<1>         blocks(1);
  // Field layout
  FLayout_t flayout(meshDomain,blocks,gl,DistributedTag());
  // *************************************************

  // Create Mesh ************************************
  Interval<1> i_s(0,nx+2);
  Array<1,double,Brick> ss(i_s);
  SpacingsType_t spacings = {ss};
  // set up spacings from GridParams class
  for (int i=1; i<=nx; i++) spacings[0](i) = static_cast<double>(i)/nx;
  // the guard cells are equal to the first and last cells
  spacings[0](0)    =  spacings[0](1);
  spacings[0](nx+1) =  spacings[0](nx);
  spacings[0](nx+2) =  spacings[0](nx+1);
  // Mesh
  Mesh_t mesh(flayout, Vector<1>(0), spacings);
  // *************************************************

  // Create particles layout *************************
  PLayout_t playout(mesh,flayout);
  // *************************************************

  CParticles_t P(playout);
  P.create(2);
  P.Sync();

  cout<<P;
 
  const int n_cache = 100;
  ChargedParticleCache cpc("TestParticles");
  cpc.Initialize(n_cache);

  std::cout<<"\n\nCreate particles ========>\n\n";

  int nn1 = 6;
  for ( int i=0; i<nn1; i++)
    {
      cpc.Add(1, i, i*i, 0, 'c', i, 2*i);
      std::cout<<" size "<<cpc.Size()<<"\n";
    }
  std::cout<<cpc<<"\n\n";

  int i_start = P.size();
  P.create(cpc.Size());
  P.Sync();
  for (int i=0; i<cpc.Size(); i++)
    {
      P.SetParticle( i_start + i, 
                     cpc.Weight[i],
                     cpc.X[i],
                     cpc.P_par[i],
                     cpc.P_perp[i],
                     cpc.Origin[i],
                     cpc.IDTS[i],
                     cpc.ID[i] );
    }
  cpc.Clear();

  cout<<P;
  std::cout<<"\n\nCreate particles ========<\n\n";

  
  std::cout<<"\n\nDestroy particles ========>\n\n";
  for ( int i=0; i< P.size(); i++ )
    {
      if ( P.P_par(i) == 9e0 ||  P.P_par(i) == 25e0 )
	P.deferredDestroy(Loc<1>(i));
    }
  P.performDestroy();
  P.Sync();
  cout<<P;
  std::cout<<"\n\nDestroy particles ========<\n\n";


  Pooma::finalize();
  return 0;
}

