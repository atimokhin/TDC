#ifndef MESH_AND_LAYOUTS_H
#define MESH_AND_LAYOUTS_H


#include "Pooma/Fields.h"
#include "Pooma/Particles.h"

#include "ATbase.h"

#include "../utils/io_filenames.h"
#include "../inout/save2hdf.h"

#include "../_Control/output_control.h"

#include "my_grid.h"

/**
 * \defgroup mesh_grp Mesh 
 */


/**
 * \ingroup mesh_grp
 *
 * \class MeshAndLayouts
 *
 * \brief This class contains Mesh and Field- and Particles- Layouts
 * 
 * This class sets up Mesh and Field- and Particles- Layouts.  It
 * contains them as pointers. The instances of these classes are
 * returned by corresponding accessor methods. This class can
 * initialize Mesh and Layouts from config file and save mesh
 * information to HDF file.
 *
 * MeshAndLayouts<Mesh_t,Field_t,PTraits_t> is parametrized by Mesh
 * type Mesh_t, Fields type Field_t, and ParticlesTraits type
 * PTraits_t. 
 *
 * required SetupParameters:
 *  - GridParams    
 *  - GeometryParams 
 *  - OutputControl   
 *
 */

template<class Mesh_t, class Field_t, class PTraits_t> 
class MeshAndLayouts
{
  typedef typename Mesh_t::SpacingsType_t  SpacingsType_t;

  typedef typename Field_t::Engine_t::Layout_t   FLayout_t;
  typedef typename PTraits_t::ParticleLayout_t   PLayout_t;

public:

  // Constructor (empty)
  MeshAndLayouts() {};
  //! Destructor deletes all content
  ~MeshAndLayouts();

  //! get mesh
  Mesh_t GetMesh() { return *pMesh; };
  //! get field layout
  FLayout_t GetFieldLayout() { return *pFLayout; };
  //! get particles layout
  PLayout_t GetParticlesLayout() { return *pPLayout; };

  
  //! Setup Mesh and layouts from config file
  void SetupFromConfig(FileInput& in);
  //! Save Mesh information to HDF file
  void SaveToHDFFile();

private:

  Mesh_t     *pMesh;    //!< pointer to Mesh
  FLayout_t  *pFLayout; //!< pointer to Field layout
  PLayout_t  *pPLayout; //!< pointer to Particles layout 
};


template<class Mesh_t, class Field_t, class PTraits_t> 
MeshAndLayouts<Mesh_t,Field_t,PTraits_t>::~MeshAndLayouts()
  {
    delete pMesh;
    delete pFLayout;
    delete pPLayout;
  }



template<class Mesh_t, class Field_t, class PTraits_t> 
void MeshAndLayouts<Mesh_t,Field_t,PTraits_t>::SetupFromConfig(FileInput& in)
{
  My_Grid grid;

  // Create field layout ****************************
  Interval<1>    meshDomain(0, grid.NumberOfCells() );
  GuardLayers<1> gl(2);
  Loc<1>         blocks(1);
  // Field layout
  pFLayout = new FLayout_t(meshDomain,blocks,gl,DistributedTag());
  // *************************************************

  // Create Mesh ************************************
  SpacingsType_t spacings( grid.dX() );
  // Mesh
  pMesh = new  Mesh_t(*pFLayout, Vector<1>(0), spacings);
  // *************************************************

  // Create particles layout *************************
  pPLayout =  new PLayout_t(*pMesh,*pFLayout);
  // *************************************************
}



//! save mesh to hdf file
template<class Mesh_t, class Field_t, class PTraits_t> 
void MeshAndLayouts<Mesh_t,Field_t,PTraits_t>::SaveToHDFFile()
{
  OutputControl out;
  std::string filename = out.OutputDirName()+InOut::MeshFile;

  Save2HDF mesh_hdf;
  mesh_hdf.create(filename.c_str());

  mesh_hdf.writeMesh(*pMesh);
}
#endif

