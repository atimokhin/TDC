#ifndef CHARGED_H
#define CHARGED_H

#include "ATbase.h"

#include "../PhysicsLib/small_functions.h"

#include "../SetupParameters/particles_params.h"

#include "My_Particles.h"


/**
 * \ingroup particles_grp
 * \class   Charged<> 
 * 
 * Class for charged particles
 * 
 *
 * for setup following SetupParametrs are requied:
 * - ParticlesParams
 * for reading from HDF file following SetupParametrs are requied (from  My_Particles<PT>):
 * - StartControl
 *
 */ 
template <class PT>
class Charged : public My_Particles<PT>
{
  // Typedefs
  typedef My_Particles<PT>  Base_t;

public:
  //! Constructor: set up layouts, register attributes
  Charged(const typename Base_t::ParticleLayout_t &pl);

  //! Setup particles from config file
  void SetupFromConfigGroup(FileInput& in, ParticleID& p_id);

  //! performe sync on particles
  void Swap() { this->swap(this->X); };
  //! performe sync on particles
  void Sync() { this->sync(this->X); };

  //! create new particle with specified attributes
  void AddParticle(double weight, double x, double p_par, double p_perp, 
		   char origin, int idts, int id); 

public:
  // Properties -----------------------------
  using Base_t::LptData;
  using Base_t::N_BASE;

  //! Property: particle mass normalized to \f$ m_e \f$
  double M;
  //! Property: particle charge normalized to \f$ e \f$
  double Q;
  // ----------------------------------------

  // Attributes -----------------------------
  using Base_t::Weight;
  using Base_t::Origin;
  using Base_t::IDTS;
  using Base_t::ID;

  //! Attribute: Position 
  typename Base_t::Attribute_Point_t   X;
  //! Attribute: Parallel Momentum \f$ p=\beta\gamma \f$
  typename Base_t::Attribute_Scalar_t  P_par;
  //! Attribute: Perpendicular Momentum \f$ p=\beta\gamma \f$
  typename Base_t::Attribute_Scalar_t  P_perp;
  // ----------------------------------------
};





/**
 * Constructor: set up layouts, register attributes
 * 
 */
template <class PT>
Charged<PT>::Charged(const typename Base_t::ParticleLayout_t &pl): 
  My_Particles<PT>(pl)
{
  // add other attributes
  AddAttribute_Point( "X", X);
  AddSavedAttribute(  "X", X);

  AddAttribute_Scalar("P_par", P_par);
  AddSavedAttribute(  "P_par", P_par);

  AddAttribute_Scalar("P_perp", P_perp);
  AddSavedAttribute(  "P_perp", P_perp);

  Base_t::AddSavedProperty("Mass",M);
  Base_t::AddSavedProperty("Charge",Q);
}



/** 
 * \fn void Charged<PT>::SetupFromConfigGroup(FileInput& in)
 * Sets up charged particles class by reading parameters from config file.
 *
 *
 * This function sets up:
 * - for  <B> InitialConditions </B>  setup it calls My_Particles<PT>::SetupFromConfigGroup(in)
 * - charge related properties <B> QM </B> and <B> q0 </B> 
 * .
 *
 * config file example:
 \verbatim
       Group "Electrons" {
          Mass   = 1;
	  Charge = -1;
          ....
          Group "InitialConditions" {
          ....
          }
       }
 \endverbatim
 * 
 * @param in InputFile object
 *
 */
template <class PT>
void Charged<PT>::SetupFromConfigGroup(FileInput& in, ParticleID& p_id)
{
  // Base class setup and initial conditions
  Base_t::SetupFromConfigGroup(in, p_id);
  
  // Setup properties: ******

  // GJ particle number
  N_BASE=ParticlesParams().NGJ();

  // setup particle mass
  M = in.get_param("Mass");
  if ( M <= 0 )
    {
      cout<<"Charged<PT>::SetupFromConfigGroup() : mass is not positive!\n";
      exit(1);
    } 

  // setup particle charge
  Q   = in.get_param("Charge");
  if ( Q == 0 )
    {
      cout<<"Charged<PT>::SetupFromConfigGroup() : Particle charge iz zero!\n";
      exit(1);
    } 
};




/**
 * Create new particle with specified attributes: not save in parallel
 * 
 */
template <class PT>
void Charged<PT>::AddParticle(double weight, 
			      double x, 
			      double p_par, double p_perp, 
			      char origin,
			      int idts, int id)
{
  // index of the first particle being created
  int i = this->size();

  // create particles in the last patch
  this->create(1);

  Weight(i) = weight;
  Origin(i) = origin;

  X(i)      = x;
  P_par(i)  = p_par;
  P_perp(i) = p_perp;

  IDTS(i) = idts;
  ID(i)   = id;
}


#endif




