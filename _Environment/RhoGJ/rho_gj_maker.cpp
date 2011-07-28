#include "rho_gj_maker.h"

#include <iostream>

// ******************************************************
//  Add new rhogj_...
//  no further modifications necessary
// ******************************************************
#include "rho_gj_const.h"
#include "rho_gj_sclf_lin2.h"
#include "rho_gj_sclf_exp2.h"


/**
 * \ingroup rho_gj_grp
 *
 * \def  MACRO__MAKE_P_RHOGJ_FOR_ALL
 * \brief Single macro for all initial distributions
 * 
 * <I> update this list for each new initial distribution! </I>
 */
#define MACRO__MAKE_P_RHOGJ_FUNCTOR_FOR_ALL	\
  MACRO__MAKE_P_RHOGJ_FUNCTOR(RhoGJFunctor_Const); \
  MACRO__MAKE_P_RHOGJ_FUNCTOR(RhoGJFunctor_SCLF_lin2) \
  MACRO__MAKE_P_RHOGJ_FUNCTOR(RhoGJFunctor_SCLF_exp2);

// ******************************************************



/**
 * \ingroup rho_gj_grp
 *
 * \def  MACRO__MAKE_P_RHOGJ
 * 
 * \brief this macro is associated with the function
 * RhoGJMaker::pMake!
 *
 * this macro generates code for:
 * - insertion of RhoGJ name into the list
 * - generation of the corresponding distribution class
 */
#define MACRO__MAKE_P_RHOGJ_FUNCTOR(NAME)			\
  rhogj_functor_name_list.push_back(#NAME);				\
  if ( rhogj_functor_name == #NAME ) {  p_rhogj_functor = new NAME(); };                                   



/** 
 * \brief make RhoGJ
 * 
 * uses macro MACRO__MAKE_P_RHOGJ via MACRO__MAKE_P_RHOGJ_FOR_ALL
 *
 * \return pointer to the RhoGJ class object
 */
RhoGJFunctor* RhoGJFunctorMaker::pMake(string rhogj_functor_name) const
{
  // list of names for known distributions
  std::list<string> rhogj_functor_name_list;
  // set ponter to NULL - need later for check of success
  RhoGJFunctor* p_rhogj_functor = NULL;


  // macros for each known distribution
  MACRO__MAKE_P_RHOGJ_FUNCTOR_FOR_ALL;


  // check success
  if ( !p_rhogj_functor )
    {
      cout<<"Wrong RhoGJ name: "<<rhogj_functor_name<<" !\n";
      cout<<"Valid types are:\n";
      for (std::list<string>::iterator iter = rhogj_functor_name_list.begin();
           iter != rhogj_functor_name_list.end();
           iter++)
        {
          cout<<" --> "<<*iter<<endl;
        }
      cout<<"\n\n\n";
      exit(1);
    }
  
  // setup RhoGJ by reading config file
  p_rhogj_functor->SetupFromConfigGroup(_Input);

  return p_rhogj_functor;
};

