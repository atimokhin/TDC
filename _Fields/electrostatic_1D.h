#ifndef ELECTROSTATIC_1D_H
#define ELECTROSTATIC_1D_H

#include "em_fields.h"

#include "electrostatic_1D_j.h"

/**
 * \ingroup fields_grp
 *
 * \class Electrostatic_1D
 * \brief 1D electrostatic field class
 * 
 * Class describing 1D electrostatic problem. Uses Thomas tridiagonal
 * algorithm for solution of Poisson equation. 
 *
 * <B> Boundary conditions description is in
 * SetupFromConfigGroup(FileInput& in) function </B>
 *
 * notes about implementation:
 * - fields uses Cell centerings
 * - finite difference schema uses dx(i)=x(i)-x(i-1)
 *
 */
template<class Field_t>
class Electrostatic_1D: public EMFields<Field_t>
{
  // Typedefs ---------------------------------
  typedef EMFields<Field_t>  Base_t;
  typedef typename Base_t::Mesh_t          Mesh_t;
  typedef typename Base_t::FLayout_t       FLayout_t;

  typedef typename Base_t::FieldAux_t      FieldAux_t;
  // -------------------------------------------

  // BC types ----------------------------------
  enum BC_Type { BC__PERIODIC, BC__DIRICHLET, BC__NEUMANN, 
		 BC__LOOSE_UPPER_END, BC__LOOSE_LOWER_END,
		 BC__FOLLOW_E, 
                 BC__ZERO_AT_LOWER_END };
  // -------------------------------------------


public:

  //! Constructor
  Electrostatic_1D();

  //! Initialization: requires layout, mesh
  void Initialize(const FLayout_t &layout, const Mesh_t &mesh);

  //! Setup from config file. <B> Boundary conditions description </B>
  void SetupFromConfigGroup(FileInput& in);
  //! Read fields from HDF file
  void SetupFromHDFFilesAndConfigGroup(FileInput& in);

  //! Solve for the electromagnetic field at the next time step
  void EvolveElectroMagneticField(double t, double dt);
  //! Electromagnetic field at calculation start
  void InitialField();

  //! solve Poisson Equation fo the current state of the system
  void EnforceGaussLaw(double t, double dt);

  //! currenct mismatch for getting E(i) = e_i_nextstep 
  double dJ_ToSet_E_i_OnTheNextTimeStep(int i, double e_i_nextstep, double t, double dt);

  //! Apply boundary conditions to Rho and J
  void ApplyBoundaryConditionsToRhoAndJ();  
  //! Digital filter for smoothing of statistical noise
  void ApplyDigitalFilter();

  //! Poisson Equation solver
  void SolvePoissonEquation();
  //! calculate Electric field from the Potential
  void CalculateEFromPhi();
  //! calculate Potential from the electric field  
  void CalculatePhiFromE();

  //! claculate E_Gauss from Gauss' Law
  void CalculateGaussField();

public:

  using Base_t::E;
  using Base_t::Rho;
  using Base_t::J;
  using Base_t::RhoGJ;
  using Base_t::X;

  //! electric potential - specific for this class variable
  Field_t Phi;

  //! electric field from the Gauss Law for test purpouses
  Field_t E_Gauss;

private:

  void TiePhiToE();

  void SetupPoissonEquationCoeff();
  void SetupBoundaryConditionsForCoeff();

  void ApplyBoundaryConditionsToE();
  void ApplyBoundaryConditionsToPhi();
  void ApplyBoundaryConditionsToRHS();

private:

  //! inductive current term
  Electrostatic_1D_J _JB;

  //@{
  /** auxilary array for linear equation solution */
  FieldAux_t a,b,c,d;
  FieldAux_t j_aux;
  //@}

  Interval<1> _L; //!< Physical domain for cell   centered quantities
  Interval<1> _L_total; //!< Total domain for cell   centered quantities
  Interval<1> _I; //!< Physical domain for vertex centered quantities
  Interval<1> _I_total; //!< Total domain for vertex centered quantities
  
  double dX; //!< spacings between <B> cell centers </B>

  //! number of the last line in the system of linear equation  
  int _NL; 

  //@{
  //! Variables storing types of Boundary conditions
  BC_Type _Phi_BC1Type,_Phi_BC2Type;
  BC_Type _E_BC1Type,_E_BC2Type;
  double  _E_Damping_Coeff;
  //@}

  //@{
  //! Potential at the boundaries
  double _V0, _V1;
  //@}

  //@{
  //! Electric field at the boundaries
  double _Phi_E0, _Phi_E1; 
  double _E_E0, _E_E1; 
  //@}


  double _B0,_C0,_D0, _An1,_Bn1,_Dn1;

  bool _ApplyDigitalFilter_Flag;
};



/**
 * Constructor. We have only one new variable, so add it to the list
 * of saved fileds
 * 
 */
template<class Field_t>
Electrostatic_1D<Field_t>::Electrostatic_1D()
{
  PAssert( static_cast<int>(Field_t::dimensions) == 1 );
  AddSavedField("Phi",Phi);

  AddSavedField("E_Gauss",E_Gauss);

  // set all boundary quantities to zero
  _V0=_V1=_Phi_E0=_Phi_E1=0;
  _E_E0=_E_E1=0;
  // default values of coefficients
  _B0=_C0=_D0=_An1=_Bn1=_Dn1=0;

  // by default no digital filtering
  _ApplyDigitalFilter_Flag=false;
};




/** 
 * Setup Boundary Copnditions from config file. 
 *
 * <B> Sets up boundary conditions for Potential and electric field</B>
 * - supported types of boundary conditions for \f$ \Phi \f$:
 *
 * -# "Periodic" : \f$ \phi_0=\phi_n; \phi_{n+1}=\phi_1 \f$ 
 *
 * -# "FixedPotentialDrop" :  \f$ \phi(L)-\phi(0)=\Delta V\f$ 
 *
 * -# "SCLF_PHI" : Space Charge Limited Flow \f$ E_{||}(0) = 0; \phi(L)=0 \f$ 
 *
 * -# "RS_PHI" : Ruderman Sutherland \f$ E_{||}(L) = 0; \phi(0)=0 \f$ 
 * 
 *
 * - supported types of boundary conditions for \f$ E_{||} \f$:
 *
 * -# "Periodic" : \f$ E(0) = E(L) \f$ 
 *
 * -# "LooseLowerEnd" : \f$ E_{||}(L) = E2 \f$ 
 *
 * -# "LooseUpperEnd" : \f$ E_{||}(0) = E1 \f$ 
 *
 * -# "LooseEnds" : 
 *
 * -# "FixedEnds" : \f$ E_{||}(0) = E1; E_{||}(L) = E2 \f$ 
 *
 * @param in -- FileInput object associated with config file
 */
template<class Field_t>  
void Electrostatic_1D<Field_t>::SetupFromConfigGroup(FileInput& in)
{
  std::string bc_name;

  in.ChangeGroup("Electrostatic_1D");

  if ( in.answer_is_set("ApplyDigitalFilter") )
    _ApplyDigitalFilter_Flag = in.get_answer("ApplyDigitalFilter");

  // **********************************************
  // Boundary conditions on electrostatic potential
  // **********************************************
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup("BoundaryConditions_Phi");
  bc_name = in.get_name("Type");

  if ( bc_name == "Periodic" )  // Periodic <<<<<<<<<<<<<<<<<<<<<<
    {
      _Phi_BC1Type=_Phi_BC2Type = BC__PERIODIC;
    }
  else if ( bc_name == "FixedPotentialDrop" ) // Fixed dV <<<<<<<<
    {
      _Phi_BC1Type=_Phi_BC2Type = BC__DIRICHLET;
      _V1=in.get_param("dV");      
    }
  else if ( bc_name == "SCLF_PHI" ) // SCLF_PHI <<<<<<<<<<<<<<<<<<<<<<
    {
      _Phi_BC1Type = BC__NEUMANN;
      _Phi_BC2Type = BC__DIRICHLET;
    }
  else if ( bc_name == "RS_PHI" )   // RS_PHI <<<<<<<<<<<<<<<<<<<<<<<<
    {
      _Phi_BC1Type = BC__DIRICHLET;
      _Phi_BC2Type = BC__NEUMANN;
    }
  else
    {
      cout<<"Wrong boundary condition type for Phi :"<<bc_name<<"!\n";
      exit(1);
    }
  in.ChangeGroup(p_current_group);



  // **********************************************
  // Boundary conditions on electric field
  // **********************************************
  in.ChangeGroup("BoundaryConditions_E");
  bc_name = in.get_name("Type");

  if ( bc_name == "Periodic" ) // Periodic <<<<<<<<<<<<<<<<<<<<<<<
    {
      _E_BC1Type = _E_BC2Type = BC__PERIODIC;
    }
  else if ( bc_name == "LooseEnds" ) // Loose Ends <<<<<<<<<<<<<<<
    {
      _E_BC1Type = BC__LOOSE_LOWER_END;      
      _E_BC2Type = BC__LOOSE_UPPER_END;      
      _E_Damping_Coeff = in.get_param("E_Damping_Coeff");
    }
  else if ( bc_name == "LooseUpperEnd" ) // Loose Upper End <<<<<<
    {
      _E_BC1Type = BC__NEUMANN;      
      _E_E0=in.get_param("E0");      

      _E_BC2Type = BC__LOOSE_UPPER_END;      
    }
  else if ( bc_name == "LooseLowerEnd" ) // Loose Lower End <<<<<<
    {
      _E_BC1Type = BC__LOOSE_LOWER_END;      

      _E_BC2Type = BC__NEUMANN;      
      _E_E1=in.get_param("E1");      
    }
  else if ( bc_name == "FixedEnds" ) // Fixed Ends <<<<<<<<<<<<<<<
    {
      _E_BC1Type = BC__NEUMANN;      
      _E_BC2Type = BC__NEUMANN;      
      _E_E0=in.get_param("E1");      
      _E_E1=in.get_param("E2");      
    }
  else
    {
      cout<<"Wrong boundary condition type for E :"<<bc_name<<"!\n";
      exit(1);
    }
  in.ChangeGroup(p_current_group);
  
  in.ChangeGroup("Electrostatic_1D_J");
  _JB.SetupFromConfigGroup(in);
  in.ChangeGroup(p_current_group);
}


/**
 *  call SetupFromConfigGroup(FileInput& in) and then read fields from
 *  HDF file
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::SetupFromHDFFilesAndConfigGroup(FileInput& in)
{
  // usual setup (BC etc)
  SetupFromConfigGroup(in);
  // read data from HDF file
  Base_t::SetupFromHDFFilesAndConfigGroup(in);
}



/**
 * Initialize fileds and coefficients for linear system solution
 * -  uses <B> central <B> centering for Phi, J   fields 
 * -  uses <B> vertex </B> centering for E,   Rho fields 
 *
 * - all fields
 * - all coefficients for linear system solution and electruc field calculation
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::Initialize(const FLayout_t &layout, 
                                           const Mesh_t &mesh )
{
  // vertex centered quantities ------------------
  Centering<1> vertex = canonicalCentering<1>(VertexType, Continuous);  

  //electric field
  E.initialize(vertex,layout,mesh);
  E(E.totalDomain())=0;

  // current density
  J.initialize(vertex,layout,mesh);
  J(J.totalDomain())=0;
  
  // Electric field from the Gauss law
  E_Gauss.initialize(vertex,layout,mesh);
  E_Gauss(E_Gauss.totalDomain())=0;

  // Physical domain
  _I = this->J.physicalDomain();
  // Total domain
  _I_total = this->J.totalDomain();

  // vertex positions
  X.initialize(vertex,layout,mesh);
  Interval<1> l_total = J.mesh().totalCellDomain();
  for ( int i=l_total.first(); i<=l_total.last(); i++)
    X(i) = J.mesh().cellPosition(0,i);

  // initialize field used for digital filitering
  j_aux.initialize(J.centering(),J.layout());
  // ---------------------------------------------


  // cell centered quantities ------------------
  Centering<1> cell = canonicalCentering<1>(CellType, Continuous);  

  // electric potential
  Phi.initialize(cell,layout,mesh);
  Phi(Phi.totalDomain())=0;

  //charge density
  Rho.initialize(cell,layout,mesh);
  Rho(Rho.totalDomain())=0;

  // Physical domain
  _L = this->Phi.physicalDomain();
  // Total domain
  _L_total = this->Phi.totalDomain();

  // cell-spacing 
  dX = mesh.spacings()(0);

  //GJ charge density array
  RhoGJ.initialize(cell,layout,mesh);
  for ( int i=_L.min(); i<=_L.max(); i++ )
    RhoGJ(i) = (*Base_t::_pRhoGJFunctor)( Pooma::positions(Phi).comp(0).read(i) );

  // initialize coefficients for the LHS of Poisson equation
  a.initialize(Phi.centering(),Phi.layout());
  b.initialize(Phi.centering(),Phi.layout());
  c.initialize(Phi.centering(),Phi.layout());
  d.initialize(Phi.centering(),Phi.layout());
};




/**
 * Provides intial configuration of electromagnetic field.
 * - solves Poisson Equation
 * - calculates electric field from the potential
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::InitialField()
{
  SetupPoissonEquationCoeff();
  SolvePoissonEquation();

  CalculateEFromPhi();
  ApplyBoundaryConditionsToE();
  // Electric field from the Gauss Law
  CalculateGaussField();
}



template<class Field_t>
void Electrostatic_1D<Field_t>::EnforceGaussLaw(double t, double dt)
{
  E(_L+1) =  E(_L) + 4*( Rho(_L) - RhoGJ(_L) )*dX;

  // \b TODO: Poisson equation solution does not work 
  // TiePhiToE();
  // SolvePoissonEquation();
  // CalculateEFromPhi(); 
}




/**
 * Evolve electromagnetic field in time. 
 *
 * This method is called by PIC for solution of Maxwell equations
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::EvolveElectroMagneticField(double t, double dt) 
{ 
  //inductive current
  double j_b = _JB(t);

  // NORMAL MODE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifndef ELECTROSTATIC_1D__SOLVE_Gauss_EQUATION
#ifndef ELECTROSTATIC_1D__TEST_GAUSS_LAW
  E(_I) -= 4*dt*( J(_I) - j_b );

  ApplyBoundaryConditionsToE();
  CalculatePhiFromE();
  // Electric field from the Gauss Law
  CalculateGaussField();
#endif
#endif
  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


  // TEST MODE: also solve Poisson equation ----
#ifndef ELECTROSTATIC_1D__SOLVE_Gauss_EQUATION
#ifdef  ELECTROSTATIC_1D__TEST_GAUSS_LAW
  E(_I) -= 4*dt*( J(_I) - j_b );
  ApplyBoundaryConditionsToE();
  
  //CalculatePhiFromE(); // calculate Phi at ghost points -do not need it (?)
  SolvePoissonEquation();
  E_Gauss(_I) = - ( Phi(_I) - Phi(_I-1) )/ dX;
#endif
#endif
  // -------------------------------------------

  // TEST MODE: solve Poisson equation at each timestep
#ifdef  ELECTROSTATIC_1D__SOLVE_Gauss_EQUATION
#ifndef ELECTROSTATIC_1D__TEST_GAUSS_LAW
  SolvePoissonEquation();
  CalculateEFromPhi(); 
#else
  // contradictory compiler options
#error incompatible directives: ELECTROSTATIC_1D__TEST_GAUSS_LAW and ELECTROSTATIC_1D__SOLVE_Gauss_EQUATION
#endif
#endif
  // --------------------------------------------
};


/**
 * Must be called \b after the electric field E is calculated:
 * it needs E(0)
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::CalculateGaussField()
{
  Pooma::blockAndEvaluate();

  int i_s = _I.min();
  E_Gauss(i_s)  = E(i_s);
  E_Gauss(_L+1) = E_Gauss(_L) + 4*( Rho(_L) - RhoGJ(_L) )*dX;
}


/**
 * Set coefficients in numerical Poisson equation in the way it
 * follows the current state of the electric field
 * 
 * - set _Phi_BC{1,2}Type = BC__NEUMANN
 * - SetupPoissonEquationCoeff
 *
 * \b TODO: does not work!!!
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::TiePhiToE()
{
  _Phi_BC1Type = BC__NEUMANN; 
  _Phi_BC2Type = BC__DIRICHLET;

  int i_s = _I.min();
  _Phi_E0 = E(i_s);

  SetupPoissonEquationCoeff();
}


/** 
 * Returnd currenct mismatch dj for getting E(i) = e_i_nextstep and
 * the next timestep, i.e. adding dj to the current will provide
 * E(i)^{n+1}= e_i_nextstep
 * 
 * @param i            - grid point
 * @param e_i_nextstep - desired value of the lectric field at the next timestep 
 * @param t            - current time
 * @param dt           - timestep
 * 
 * @return dj          - current mismatch
 */
template<class Field_t>
double Electrostatic_1D<Field_t>::dJ_ToSet_E_i_OnTheNextTimeStep(int i, 
                                                                 double e_i_nextstep, 
                                                                 double t, 
                                                                 double dt)
{
  return  _JB(t) - J(i) + ( E(i) - e_i_nextstep )/(4*dt);
}


/** 
 * Sets values of electric current at the end points. It is called
 * each time charge and current densities are scattered
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::ApplyBoundaryConditionsToRhoAndJ()
{
  Pooma::blockAndEvaluate();
  
  int l_s = _L.min();
  int l_e = _L.max();
  
  if ( _E_BC1Type == BC__PERIODIC && _E_BC2Type == BC__PERIODIC )  
    {
      // current density ------
      J(l_s)  += J(l_e+1);
      J(l_e+1) = J(l_s);
     
      J(l_s+1) += J(l_e+2);
      J(l_e)   += J(l_s-1);
      // ghost points
      J(l_e+2) = J(l_s+1);
      J(l_s-1) = J(l_e);

      // charge density -------
      Rho(l_s)   += Rho(l_e+1);
      Rho(l_s+1) += Rho(l_e+2);
      // ghost points
      Rho(l_e+1) = Rho(l_s);
      Rho(l_e+2) = Rho(l_s+1);

      Rho(l_e-1) += Rho(l_s-2);
      Rho(l_e)   += Rho(l_s-1);
      // ghost points
      Rho(l_s-2) = Rho(l_e-1);
      Rho(l_s-1) = Rho(l_s-1);
    }
  else
    {
      int i_s = _I.min();
      int i_e = _I.max();

      J(i_s-1) = J(i_s);
      J(i_e+1) = J(i_e);
      
      Rho(l_s-1) = Rho(l_s);
      Rho(l_e+1) = Rho(l_e);      
    }
}

  //! Digital filter for smoothing of statistical noise
template<class Field_t>
void Electrostatic_1D<Field_t>::ApplyDigitalFilter()
{
  if ( _ApplyDigitalFilter_Flag )
    {
      j_aux(_I) = 0.25*( J(_I-1)   + 2*J(_I)   + J(_I+1) );
      J(_I)     = j_aux(_I) ;
      j_aux(_L) = 0.25*( Rho(_L-1) + 2*Rho(_L) + Rho(_L+1) );
      Rho(_L)   = j_aux(_L);
    }
}


/** 
 * Solve Poisson equation using Thomas tridiagonal algorithm
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::SolvePoissonEquation()
{
  // initialize RHS 
  d(_L) = - 2*( Rho(_L) - RhoGJ(_L) );

  // Apply boundary conditions to RHS
  ApplyBoundaryConditionsToRHS();

  // Forward elimination in TDMA (Thomas algorithm)
  Interval<1> K (_L.min()+1,_NL);
  d(K) -= a(K)*d(K-1);

  // Backward substitution in TDMA (Thomas algorithm)
  Pooma::blockAndEvaluate();
  Phi(_NL) = d(_NL)/b(_NL);
  Range<1> J(_NL-1, _L.min(), -1);
  Phi(J) = ( d(J) - c(J)*Phi(J+1) )/b(J);
 
  // Phi at ghost points
  ApplyBoundaryConditionsToPhi();
}


/**
 * Calculate Electric field from the Potential
 * 
 */
template<class Field_t>
inline void Electrostatic_1D<Field_t>::CalculateEFromPhi()
{
  E(_I) = - ( Phi(_I) - Phi(_I-1) )/ dX;
}

/**
 * Calculate Electric Potential  from the Electric field
 * \f$ \phi(x) = \int_0^x E(\xi)\, d\xi \f$
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::CalculatePhiFromE()
{
  Pooma::blockAndEvaluate();

  int l_s = _L.min();
  int l_e = _L.max();

  Phi(l_s-1) = 0;
  Interval<1> I(l_s,l_e+1);
  Phi(I) = Phi(I-1) - dX*E(I);
}





/** 
 * Sets values of Electric field at end points. It is called each time
 * solution for E is obtained in
 * Electrostatic_1D<Field_t>::EvolveElectroMagneticField( double dt )
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::ApplyBoundaryConditionsToE()
{
  int i_s = _I.min();
  int i_e = _I.max();

  Pooma::blockAndEvaluate();

  // Adjust E at boundary points
  if ( _E_BC1Type == BC__PERIODIC )  
    {
      E(i_s)   = E(i_e);
      E(i_s-1) = E(i_e-1);
      E(i_e+1) = E(i_s+1);
    }

  if ( _E_BC1Type == BC__LOOSE_LOWER_END )
    {
      E(i_s-1) = _E_Damping_Coeff*( 3*( E(i_s)-E(i_s+1) ) + E(i_s+2) );
    }
  if ( _E_BC2Type == BC__LOOSE_UPPER_END )
    {
      E(i_e+1) = _E_Damping_Coeff*( 3*( E(i_e)-E(i_e-1) ) + E(i_e-2) );
    }


  if ( _E_BC1Type == BC__NEUMANN )
    {
      E(i_s-1) = E(i_s) = _E_E0;
    }
  if ( _E_BC2Type == BC__NEUMANN )
    {
      E(i_e) = _E_E1;
    }

  if ( _E_BC1Type == BC__ZERO_AT_LOWER_END )
    {
      E(i_s-1) = 0;
    }
}


/** 
 * Changes the RHS of the linear system representing Poisson equation
 * according to the specified boundary conditions. It is called each
 * time before the solution of the system in
 * Electrostatic_1D<Field_t>::SolvePoissonEquation(double dt)
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::ApplyBoundaryConditionsToRHS()
{
  Pooma::blockAndEvaluate(); // Scalar code below

  int l_s = _L.min();
  int l_e = _L.max();

  // adjust RHS
  if ( _Phi_BC1Type == BC__FOLLOW_E && _Phi_BC2Type == BC__FOLLOW_E )
    {
      d(l_s) -= a(l_s)*Phi(l_s-1);
      d(l_e) -= c(l_e)*Phi(l_e+1);      
    }
  else
    {
      d(l_s) -= a(l_s)*_D0;
      d(l_e) -= c(l_e)*_Dn1;
    }
}


/** 
 * Sets values of Potential at ghost points. It is called each time
 * before a solution for Phis is obtained 
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::ApplyBoundaryConditionsToPhi()
{
  Pooma::blockAndEvaluate();

  int l_s = _L.min();
  int l_e = _L.max();

  // Adjust Phi at ghost points
  if ( _Phi_BC1Type == BC__PERIODIC )  
    // periodic BC <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    {
      Phi(l_s-1) = Phi(l_e) = 0;
      Phi(l_e+1) = Phi(l_s);
    }
  else if ( _Phi_BC1Type == BC__FOLLOW_E && _Phi_BC2Type == BC__FOLLOW_E )
    // follow E: do nothing <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    {}
  else
    // all other cases (Solve poisson equations) <<<<<<<<<<<<<<<<
    {
      Phi(l_s-1) = _B0*Phi(l_s)    + _C0*Phi(l_s+1)  + _D0;
      Phi(l_e+1) = _An1*Phi(l_e-1) + _Bn1*Phi(l_e)   + _Dn1;
    }
}




/** 
 * Setup coefficients for poisson equation solver (Thomas algorithm)
 *
 * This function must be called every time after boundary conditions
 * are changed
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::SetupPoissonEquationCoeff()
{
  a = 0.5/(dX*dX);
  b = -1./(dX*dX);
  c = 0.5/(dX*dX);  

  // adjust coefficients for boundary conditions
  SetupBoundaryConditionsForCoeff();

  int l_s = _L.min();
  int l_e = _L.max();

  Pooma::blockAndEvaluate(); // Scalar code below
  b(l_s) += a(l_s)*_B0;
  c(l_s) += a(l_s)*_C0;

  a(l_e) += c(l_e)*_An1;
  b(l_e) += c(l_e)*_Bn1;

  // redefine coefficients for linear system solution 
  Interval<1> K(_L.min()+1,_L.max());
  b(K) -= c(K-1)*a(K)/b(K-1);
  a(K) /= b(K-1);
};


/** 
 * Change coefficients for the linear system solution according to the
 * specified boundary conditions ("Periodic", "Dirichlet",
 * "Neumann"). This function is called only once after changing of the
 * boundary conditions for Phi while initializing linear system
 * coefficients in
 * Electrostatic_1D<Field_t>::SetupPoissonEquationCoeff()
 * 
 */
template<class Field_t>
void Electrostatic_1D<Field_t>::SetupBoundaryConditionsForCoeff()
{
  int l_s = _L.min();
  int l_e = _L.max();

  _NL = l_e;

  if ( _Phi_BC1Type == BC__PERIODIC )  
    {
      _NL = l_e-1;
    }
  
  if ( _Phi_BC1Type == BC__DIRICHLET ) 
    {
      _B0 = -2;
      _C0 = 1e0/3;
      _D0 = _V0*8e0/3;
    }
  if ( _Phi_BC1Type == BC__NEUMANN )
    {
      _B0 = 1;
      _C0 = 0;
      _D0 = - dX*_Phi_E0;
    }

  if ( _Phi_BC2Type == BC__DIRICHLET ) 
    {
      _An1 = 1e0/3;
      _Bn1 = -2;
      _Dn1 = _V1*8e0/3;
    }
  if ( _Phi_BC2Type == BC__NEUMANN )
    {
      _An1 = 0;
      _Bn1 = 1;
      _Dn1 = dX*_Phi_E1;
    }

  if ( _Phi_BC1Type == BC__FOLLOW_E && _Phi_BC2Type == BC__FOLLOW_E )
    {
      _B0=_C0=_D0=_An1=_Bn1=_Dn1=0;
    }
}

#endif
