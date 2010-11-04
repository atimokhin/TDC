#ifndef PAIRS_H
#define PAIRS_H

#include "ATbase.h"

#include "../utils/io_filenames.h"
#include "../inout/save2hdf.h"

#include "../SetupParameters/particles_params.h"

#include "../_Control/output_control.h"

#include "My_Particles.h"


namespace Pairs_namespace
{
  const std::string PairsFileName="Pairs";
}


/**
 * \ingroup particles_grp
 * \class Pairs<>
 *
 * Class for storing of virtual electron-positron pairs
 * 
 *
 * for saving to HDF file following SetupParametrs are requied:
 * - OutputControl
 * for reading from HDF file following SetupParametrs are requied (from  My_Particles<PT>):
 * - StartControl
 */ 
template <class PT>
class Pairs: public My_Particles<PT>
{
  // Typedefs
  typedef My_Particles<PT>  Base_t;

public:

  //! Constructor: set up layouts, register attributes
  Pairs(const typename Base_t::ParticleLayout_t &pl);

  //! Setup particles from config file
  void SetupFromConfigGroup(FileInput& in, ParticleID& p_id);
  //! setup and read particle attributes from HDF file
  void SetupFromHDFFileAndConfigGroup(FileInput& in);

  //! setup output file
  void SetupOutputFile();
  //! Save pairs to corresponding HDF files
  void SaveToHDFFile(double t, int it);
  //! Flush HDF file
  void FlushHDFFile();

  //! performe sync on particles
  void Swap() { this->swap(this->X_cr); };
  //! performe sync on particles
  void Sync() { this->sync(this->X_cr); };

  //! create new pair with specified attributes
  void AddPair(double weight, double t_cr, double x_cr, double x_em, double e, double psi, 
	       char origin, int idts_parent, int id_parent, int idts, int id );
 
public:
  // Properties -----------------------------
  using Base_t::N_MAX;
  using Base_t::N_OPT;
  using Base_t::N_BASE;
  // ----------------------------------------

  // Attributes -----------------------------
  using Base_t::Weight;
  using Base_t::Origin;
  using Base_t::IDTS;
  using Base_t::ID; 

  //! Attribute: Parent Particle timestep ID (timestep# when Parent particle was created)
  typename Base_t::Attribute_INT_t  IDTS_Parent;
  //! Attribute: Parent Particle ID (patch# and consequtive# of particle created in that patch)
  typename Base_t::Attribute_INT_t  ID_Parent;

  //! Attribute: Time of pair creation
  typename Base_t::Attribute_Scalar_t T_cr;
  //! Attribute: Position of created pair
  typename Base_t::Attribute_Point_t  X_cr;
  //! Attribute: Emission point of pair-producing photon
  typename Base_t::Attribute_Point_t  X_em;
  //! Attribute: Energy of the pair-creating photon \f$ p=\beta\gamma \f$
  typename Base_t::Attribute_Scalar_t E;
  //! Attribute: Angle between pair-creating photon and magnetic field at the point of pair creation 
  typename Base_t::Attribute_Scalar_t Psi;
  // ----------------------------------------

private:
  Save2HDF _HDF;  //!< HDF file object
};





/**
 * Constructor: set up layouts, register attributes
 * 
 */
template <class PT>
Pairs<PT>::Pairs(const typename Base_t::ParticleLayout_t &pl): 
  My_Particles<PT>(pl)
{
  // add other attributes
  AddAttribute_INT( "IDTS_Parent", IDTS_Parent);
  AddAttribute_INT( "ID_Parent",   ID_Parent);
  AddAttribute_Scalar("T_cr", T_cr);
  AddAttribute_Point( "X_cr", X_cr);
  AddAttribute_Point( "X_em", X_em);
  AddAttribute_Scalar("Energy", E);
  AddAttribute_Scalar("Psi",    Psi);

  // attributes to be saved in HDF file
  AddSavedAttribute("IDTS_Parent", IDTS_Parent);
  AddSavedAttribute("ID_Parent",   ID_Parent);
  AddSavedAttribute("T_cr", T_cr);
  AddSavedAttribute("X_cr", X_cr);
  AddSavedAttribute("X_em", X_em);
  AddSavedAttribute("Energy", E);
  AddSavedAttribute("Psi",    Psi);
}



/**
 * This fucntion does nothing specifically connected with Pairs<PT> it
 * calls Base_t::SetupFromConfigGroup(in)
 * 
 */
template <class PT>
void Pairs<PT>::SetupFromConfigGroup(FileInput& in, ParticleID& p_id)
{
  Base_t::SetupFromConfigGroup(in, p_id);

  // GJ particle number 
  N_BASE=ParticlesParams().NGJ();
};


template <class PT>
void Pairs<PT>::SetupFromHDFFileAndConfigGroup(FileInput& in)
{
  Base_t::SetupFromHDFFileAndConfigGroup(Pairs_namespace::PairsFileName, in);
}


template <class PT>
void Pairs<PT>::SetupOutputFile()
{
  OutputControl out;
  string hdf_filename = out.OutputDirName()+Pairs_namespace::PairsFileName+".h5";
  _HDF.create(hdf_filename.c_str());
}


/** 
 * Save Pairs to corresponding HDF files at the moment t
 * 
 * @param t current time
 */
template <class PT>
void Pairs<PT>::SaveToHDFFile(double t, int it)
{
  Base_t::SaveToHDFFile(t,it,_HDF);
};


/** 
 * Flush HDF file
 */
template <class PT>
void Pairs<PT>::FlushHDFFile()
{
  Base_t::FlushHDFFile(_HDF);
};


/** 
 *  Creates new pair: not save for parallel
 */
template <class PT>
void Pairs<PT>::AddPair(double weight, 
                        double t_cr, double x_cr, double x_em, 
                        double e, double psi, 
			char origin, 
                        int idts_parent, int id_parent, int idts, int id )
{
  // index of the first particle being created
  int i = this->size();
  // create particles in the last patch
  this->create(1);

  Weight(i) = weight;
  Origin(i) = origin;

  T_cr(i)   = t_cr;
  X_cr(i)   = x_cr;
  X_em(i)   = x_em;
  E(i)      = e;
  Psi(i)    = psi;

  IDTS_Parent(i) = idts_parent;
  ID_Parent(i)   = id_parent;

  IDTS(i) = idts;
  ID(i)   = id;
}

#endif
