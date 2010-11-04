#ifndef PARTICLE_LIST_H
#define PARTICLE_LIST_H

#include <iostream>
#include <vector>

#include "ATbase.h"

#include "../utils/io_filenames.h"
#include "../utils/formats.h"
#include "../inout/save2hdf.h"

#include "../_Control/output_control.h"

#include "My_Particles.h"


/**
 * \ingroup particles_grp
 * 
 * \class ParticleList
 *
 * Collection of particles. Can set up particles from config files and
 * save them into separate HDF files
 *
 * for saving to HDF file following SetupParametrs are requied:
 * - OutputControl
 */
template<class P>
class ParticleList
{
public:

  //! Type of particles in the list 
  typedef P Particles_t;

public:

  //! Destructor
  ~ParticleList();

  //! return pointer to particle object of sort i
  P* operator[] (int i) {return pParticles[i];};

  //! number of particle sorts
  int Size() const { return pParticles.size(); };

  //! add particles to the list
  void Add(std::string name, P& particles);
  //! add particles to the list
  void Add(std::string name, P* p_particles);


  //! get reference to the particles "name"
  P& GetParticles(std::string name);
  //! get name of i'th particles 
  std::string GetParticlesName(int i) const;

  //! delete all particles objects
  void FreeMemory();

  //! Setup all particles from config file
  void SetupFromConfigGroups(FileInput& in, ParticleID& p_id);
  //! Setup all particles from their own HDF files
  void SetupFromHDFFilesAndConfigGroups(FileInput& in);

  //! Setup output HDF files
  void SetupOutputFiles();
  //! Save all Particles to corresponding HDF files
  void SaveToHDFFile(double t, int it);
  //! Flush all Particles HDF files 
  void FlushHDFFile();

  //! performe sync on particles
  void Swap();
  //! performe sync on particles
  void Sync();

  //! print content of the list
  void Print(std::ostream& o = std::cout) const;
  //! print attribute of all particles types
  void PrintAttribute(std::string attr_name, std::ostream& o = std::cout) const;

public:

  //! array with particle object pointers
  std::vector<P*> pParticles;

private:

  std::vector<std::string>  mNameList;  //!< array with particle names
  std::vector<Save2HDF*>    mpHDFList;  //!< array with pointers to HDF fiel objects
};



/**
 * Destructor.
 * deletes HDF file objects
 * 
 */
template<class P> ParticleList<P>::~ParticleList()
{
  for (int i=0; i<pParticles.size(); i++) delete mpHDFList[i];
};


/** 
 * add particles to the list
 * 
 * @param name name of the particles
 * @param particles particles object
 */
template<class P>
void ParticleList<P>::Add(std::string name, P& particles) 
{ 
  pParticles.push_back(&particles);
  mNameList.push_back(name);
  mpHDFList.push_back(new Save2HDF());
};

/** 
 * add particles to the list using pointer
 * 
 * @param name name of the particles
 * @param p_particles piointer to a particles object
 */
template<class P>
void ParticleList<P>::Add(std::string name, P* p_particles) 
{ 
  pParticles.push_back(p_particles);
  mNameList.push_back(name);
  mpHDFList.push_back(new Save2HDF());
};


/** 
 * get reference to Particles
 * 
 * @param name name of the particles
 */
template<class P>
P& ParticleList<P>::GetParticles(std::string name)
{
  int i;

  //! get index of particles object
  vector<string>::iterator iter = find(mNameList.begin(),mNameList.end(),name);
 
  if ( iter != mNameList.end() )
    {
      i = iter - mNameList.begin();
    }
  else
    {
      cout<<"No particles \""<<name<<"\" in ParticleList!\n";
      exit(1);
    }

  return *pParticles[i];
}


/** 
 * Name of the i'th specie
 * 
 * @param i index of the specie
 */
template<class P>
std::string ParticleList<P>::GetParticlesName(int i) const
{
  return mNameList[i];
}


/** 
 * delete all particles object in the list.  
 *
 * <I> Call this function if particles objects exists only in the
 * particle list !</I>
 * 
 */
template<class P>
void ParticleList<P>::FreeMemory()
{ 
  for (int i=0; i<pParticles.size(); i++) delete pParticles[i];
};



/** 
 * \fn void ParticleList<P>::SetupFromConfigGroup(FileInput& in)
 * Sets up all particles from config file
 *
 * Iterates over all Particle classes in the list and:
 * - for each particle type goes into the corresponding group 
 *       in the config file
 * - reads NumberOfParticles 
 * - creates requested number of particles
 * - calls SetupFromConfigGroup(in) for each particle type in the list
 * - leaves the group
 * - creates new entry in HDF file list for particle type
 *
 * example of config file:
     \verbatim
     Group "Electrons" {
        .....
        NumberOfParticles_Initial = 2;
        .....
     }
     \endverbatim
 *
 * 
 * @param in FileInput object associated with config file
 *
 */
template<class P>
void ParticleList<P>::SetupFromConfigGroups(FileInput& in, ParticleID& p_id)
{
  // iterate over the particle kinds
  for (int i=0; i<pParticles.size(); i++)
    {
      in.ChangeGroup(mNameList[i]);

      // setup from config file 
      pParticles[i]->SetupFromConfigGroup(in, p_id);
	
      in.ChangeGroup("..");
    }
};


/** 
 * Setup Particles by reading particle data from corresponding HDF files
 */
template<class P>
void ParticleList<P>::SetupFromHDFFilesAndConfigGroups(FileInput& in)
{
  // iterate over the particle kinds
  for (int i=0; i<pParticles.size(); i++)
    {
      in.ChangeGroup(mNameList[i]);

      // setup particles from HDF file 
      pParticles[i]->SetupFromHDFFileAndConfigGroup(mNameList[i],in);

      in.ChangeGroup("..");
    }
};


/** 
 * Setup output HDF files for each sort of particles
 * 
 */
template<class P>
void ParticleList<P>::SetupOutputFiles()
{
  OutputControl out;

  // iterate over the particle kinds
  for (int i=0; i<pParticles.size(); i++)
    {
      // create new HDF file for this sort of particles
      std::string filename = out.OutputDirName()+mNameList[i]+".h5";
      mpHDFList[i]->create(filename.c_str());
    }
};



/** 
 * Save all Particles to corresponding HDF files at the moment t
 * 
 * @param t current time
 */
template<class P>
void ParticleList<P>::SaveToHDFFile(double t, int it)
{
  for (int i=0; i<pParticles.size(); i++)
    {
      pParticles[i]->SaveToHDFFile(t,it,*mpHDFList[i]);
    }
};


/** 
 * Flush Particles' HDF files
 */
template<class P>
void ParticleList<P>::FlushHDFFile()
{
  for (int i=0; i<pParticles.size(); i++)
    {
      pParticles[i]->FlushHDFFile( *mpHDFList[i] );
    }
};



/** 
 *  Swap all particles 
 */
template<class P>
void ParticleList<P>::Swap()
{
  for (int i=0; i<pParticles.size(); i++)
      pParticles[i]->Swap();
};


/** 
 *  Sync all particles 
 */
template<class P>
void ParticleList<P>::Sync()
{
  for (int i=0; i<pParticles.size(); i++)
      pParticles[i]->Sync();
};




/** 
 * Print attribute of all Particles to cout
 * 
 * @param attr_name attribute name
 * @param o output stream
 */
template<class P>
void ParticleList<P>::PrintAttribute(string attr_name, ostream& o) const
{
  for (int i=0; i<pParticles.size(); i++)
    {
      o<<"\nSpecie: \""<<mNameList[i]<<"\" ->";
      pParticles[i]->PrintAttribute(attr_name, o);
    } 
}

/** 
 * Print number of particles in the list
 * 
 * @param o output stream
 */
template<class P>
void ParticleList<P>::Print(ostream& o) const
{
  switch( fmt::Format::GetFormat() )
    {
    case fmt::SHORT :
      for (int i=0; i<pParticles.size(); i++)
	o<<"\""<<mNameList[i]<<"\": "<<pParticles[i]->size()<<" ";
      break;

    case fmt::LONG :
      for (int i=0; i<pParticles.size(); i++)
	{
	  o<<"\""<<mNameList[i]<<"\":\n";
	  o<<*pParticles[i];
	}
      break;
    }
  fmt::Format::SetDefaultFormat();
}



template <class P>
inline std::ostream& operator<< ( std::ostream& s, const ParticleList<P>& p )
{
  p.Print(s);
  return s;
};

#endif
