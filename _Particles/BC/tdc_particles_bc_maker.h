#ifndef TDC_PARTICLES_BC_MAKER_H
#define TDC_PARTICLES_BC_MAKER_H

#include <iostream>
#include <string>
#include <list>

#include "ATbase.h"


#include "tdc_particles_bc.h"

// ******************************************************
//  Add new distribution here
//  no further modifications necessary
// ******************************************************
#include "PERIODIC.h"
#include "RS_1.h"
#include "RS_2.h"
#include "SCLF_1.h"
#include "SCLF_2.h"
#include "SCLF_3.h"
#include "INJECT.h"



/**
 * \ingroup particles_bc_grp
 *
 * \def  MACRO__MAKE_P_BC_PARTICLES_FOR_ALL
 * \brief Single macro for all initial distributions
 * 
 * <I> update this list for each new initial distribution! </I>
 */
#define MACRO__MAKE_P_BC_PARTICLES_FOR_ALL	\
  MACRO__MAKE_P_BC_PARTICLES(PERIODIC);		\
  MACRO__MAKE_P_BC_PARTICLES(RS_1);		\
  MACRO__MAKE_P_BC_PARTICLES(RS_2);		\
  MACRO__MAKE_P_BC_PARTICLES(SCLF_1);		\
  MACRO__MAKE_P_BC_PARTICLES(SCLF_2);		\
  MACRO__MAKE_P_BC_PARTICLES(SCLF_3);		\
  MACRO__MAKE_P_BC_PARTICLES(INJECT);		

// ******************************************************




/**
 * \ingroup particles_bc_grp
 *
 * \class TDC_ParticlesBC_Maker
 * \brief TDC_ParticlesBC_Maker class factory
 *
 *  Makes ParticleBC object by reading information 
 *  from configuration file
 */
template<class EM, class P>
class TDC_ParticlesBC_Maker
{
public:
  //! constructor set only input file
  TDC_ParticlesBC_Maker(FileInput& in): mInput(in) {};

  //! make ParticleInitialConditions class object from config file
  TDC_ParticlesBC<EM,P>* pMake( string type ) const;

private:

  FileInput& mInput;     //! pointer to the input class
};






/**
 * \ingroup particles_bc_grp
 *
 * \def  MACRO__MAKE_P_BC_PARTICLES
 * 
 * \brief this macro is associated with the function
 * InitialDistributionMaker<A>::pMake!
 *
 * this macro generates code for:
 * - insertion of distribution name into the list
 * - generation of the corresponding distribution class
 */
#define MACRO__MAKE_P_BC_PARTICLES(NAME)			\
  distr_name_list.push_back(#NAME);				\
  if ( distr_name == #NAME ) {  p_distr = new NAME<EM,P>(); };                                   



/** 
 * \brief make InitialDistribution
 * 
 * uses macro MACRO__MAKE_P_BC_PARTICLES via MACRO__MAKE_P_BC_PARTICLES_FOR_ALL
 *
 * \return pointer to the InitialDistribution<A> class object
 */
template<class EM, class P>
TDC_ParticlesBC<EM,P>* TDC_ParticlesBC_Maker<EM,P>::pMake(string distr_name) const
{
  // list of names for known distributions
  std::list<string> distr_name_list;
  // set ponter to NULL - need later for check of success
  TDC_ParticlesBC<EM,P>* p_distr = NULL;


  // macros for each known distribution
  MACRO__MAKE_P_BC_PARTICLES_FOR_ALL;


  // check success
  if ( !p_distr )
    {
      cout<<"Wrong ParticleBC name: "<<distr_name<<" !\n";
      cout<<"Valid types are:\n";
      for (std::list<string>::iterator iter = distr_name_list.begin();
           iter != distr_name_list.end();
           iter++)
        {
          cout<<" --> "<<*iter<<endl;
        }
      cout<<"\n\n\n";
      exit(1);
    }
  
  // setup distribution by resading config file
  p_distr->SetupFromConfigGroup(mInput);

  return p_distr;
};




#endif

