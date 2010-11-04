#ifndef EM_FIELDS_H
#define EM_FIELDS_H

#include "My_Fields.h"

#include "../_Environment/RhoGJ/rho_gj.h"


/**
 * \ingroup fields_grp
 *
 * \class EMFields
 * \brief Base class for electromagnetic fields
 * 
 * Base class for electromagnetic fields.  Contains charge density and
 * accelerating electric field as public members
 *
 */
template<class Field_t>
class EMFields: public My_Fields<Field_t>
{
public:
  // Typedefs --------------------------------------------
  typedef My_Fields<Field_t>          Base_t;
  typedef typename Base_t::Mesh_t     Mesh_t;
  typedef typename Base_t::FLayout_t  FLayout_t;

  typedef typename Base_t::FieldAux_t FieldAux_t;
  // -----------------------------------------------------

public:

  //! Constructor 
  EMFields(); 

  //! setup output hdf file
  void SetupOutputFile(std::string filename);

  //! Maxwell Equation solver
  virtual void EvolveElectroMagneticField(double t, double dt) = 0;
  //! Get initial field configuration (solve Poisson equation)
  virtual void InitialField() = 0;

  virtual void EnforceGaussLaw(double t, double dt) = 0;


  //! Rho and J at the boundaries
  virtual void ApplyBoundaryConditionsToRhoAndJ() = 0;

  //! Digital filter for smoothing of statistical noise
  virtual void ApplyDigitalFilter() = 0;

  virtual double dJ_ToSet_E_i_OnTheNextTimeStep(int i, double e, double t, double dt) = 0;
  

  void ClearRho() { Rho(Rho.totalDomain())=0; }
  void ClearJ()   { J(J.totalDomain())=0; }

  void SetRhoGJFunctor(RhoGJFunctor* p_rhogj_fun) { _pRhoGJFunctor=p_rhogj_fun; };

  //! accelerating electric field
  Field_t E;
  //! charge density
  Field_t Rho;
  //! current density
  Field_t J;

  //! cell centers
  Field_t X;
  
  //! Goldreich-Julian charge density array
  Field_t RhoGJ;

protected:
  //! GJ charge density functor
  RhoGJFunctor* _pRhoGJFunctor;
};



/**
 * Constructor (adds Rho and E_acc to the list of saved fields)
 * 
 */
template<class Field_t>
EMFields<Field_t>::EMFields() 
{
  AddSavedField("Rho",Rho);
  AddSavedField("J",J);
  AddSavedField("E_acc",E);    
    
  AddSavedField("RhoGJ",RhoGJ);
};


/**
 * Setup and open HDF file where fields will be saved
 * <b> by default the filename is InOut::FieldsFile </b>
 */
template<class Field_t>
void EMFields<Field_t>::SetupOutputFile(std::string filename = InOut::FieldsFile)
{
  Base_t::SetupOutputFile(filename);
}

#endif

















