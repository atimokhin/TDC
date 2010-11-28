#ifndef INITIAL_DISTRIBUTION_MAKER_H
#define INITIAL_DISTRIBUTION_MAKER_H

#include <iostream>
#include <string>
#include <list>

#include "ATbase.h"


// ******************************************************
// Add new distribution here
//  no further modifications necessary
// ******************************************************

#include "initial_distribution.h"
#include "single_value.h"
#include "discrete.h"
#include "sinx.h"
#include "cosx.h"
#include "uniform.h"
#include "sinx_perturbed_uniform.h"
#include "random_uniform.h"
#include "random_rho_gj.h"


/**
 * \ingroup initial_distr_grp
 *
 * \def  MACRO__MAKE_P_DISTR_FOR_ALL
 * \brief Single macro for all initial distributions
 * 
 * <I> update this list for each new initial distribution! </I>
 */
#define MACRO__MAKE_P_DISTR_FOR_ALL     \
  MACRO__MAKE_P_DISTR(Additive);	\
  MACRO__MAKE_P_DISTR(SingleValue);	\
  MACRO__MAKE_P_DISTR(Discrete);	\
  MACRO__MAKE_P_DISTR(SinX);            \
  MACRO__MAKE_P_DISTR(CosX);            \
  MACRO__MAKE_P_DISTR(Uniform);         \
  MACRO__MAKE_P_DISTR(SinXPerturbedUniform);  \
  MACRO__MAKE_P_DISTR(RandomUniform);   \
  MACRO__MAKE_P_DISTR(RandomRhoGJ);
// ******************************************************




/**
 *  \ingroup initial_distr_grp
 *
 *  \class InitialDistributionMaker
 *  \brief InitialDistribution<A> class factory
 *
 *  Makes InitialDistribution<A> object by reading information 
 *  from configuration file
 */

template<class A, class A1=A>
class InitialDistributionMaker
{
public:
  //! constructor set only input file
  InitialDistributionMaker(FileInput& in): mInput(in) {};

  //! make ParticleInitialConditions class object from config file
  InitialDistribution<A,A1>* pMake( string type ) const;

private:

  FileInput& mInput;     //! pointer to the input class
};




/**
 * \ingroup initial_distr_grp
 *
 * \class Additive
 *
 * Initial distribution as a sum of several distributions
 *
 * configuration file example:
 \verbatim

   Group "Additive" {		
      NumberOfDistributions=2;
      DistributionName_#0='SingleValue';
      DistributionName_#1='Discrete';
      Group "SingleValue" {		
      ........
      }
      Group "Discrete" {		
      ........
      }
   }

 \endverbatim
 *
 */
template<class A, class A1>
class Additive: public InitialDistribution<A,A1>
{
  typedef std::list<InitialDistribution<A,A1>*> List_t;
  typedef typename List_t::iterator             Iter_t;

public:

  //! Destructor
  ~Additive();

  //! accepts particle attribute attr and initialize it
  void operator() (A& attr);
  //! accepts particle attribute attr, attr_base and initialize attr(attr_base) 
  void operator() (A& attr, A1& attr_base);

  //! setups distribution by reading config file 
  void SetupFromConfigGroup(FileInput& in);

private:

  List_t distr_list;
};






/**
 * \ingroup initial_distr_grp
 *
 * \def  MACRO__MAKE_P_DISTR
 * 
 * \brief this macro is associated with the function
 * InitialDistributionMaker<A>::pMake!
 *
 * this macro generates code for:
 * - insertion of distribution name into the list
 * - generation of the corresponding distribution class
 */
#define MACRO__MAKE_P_DISTR(NAME)	\
  distr_name_list.push_back(#NAME);     \
  if ( distr_name == #NAME ) {  p_distr = new NAME<A,A1>(); };                                   


/** 
 * \brief make InitialDistribution
 * 
 * uses macro MACRO__MAKE_P_DISTR via MACRO__MAKE_P_DISTR_FOR_ALL
 *
 * \return pointer to the InitialDistribution<A> class object
 */
template<class A, class A1>
InitialDistribution<A,A1>* 
InitialDistributionMaker<A,A1>::pMake(string distr_name) const
{
  // list of names for known distributions
  std::list<string> distr_name_list;
  // set ponter to NULL - need later for check of success
  InitialDistribution<A,A1>* p_distr = NULL;


  // macros for each known distribution
  MACRO__MAKE_P_DISTR_FOR_ALL


    // check success
    if ( !p_distr )
      {
	cout<<"Wrong InitialDistribution name: "<<distr_name<<" !\n";
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




/**
 * Destructor
 * delete all distributions in the internal list
 * 
 */
template<class A, class A1>
Additive<A,A1>::~Additive()
{
  for ( Iter_t iter=distr_list.begin(); iter!=distr_list.end(); iter++)
    delete *iter;
}



/**
 * sets value for the attribute
 * iterates over the internal list and adds values
 * 
 */
template<class A, class A1>
void Additive<A,A1>::operator() (A& attr)
{
  attr=0;
  A attr_tmp(attr.size());
  for ( Iter_t iter=distr_list.begin(); iter!=distr_list.end(); iter++)
    {
      (*iter)->operator()(attr_tmp);
      attr += attr_tmp;
    };
};


/**
 * sets value for the attribute
 * iterates over the internal list and adds values
 * 
 */
template<class A, class A1>
void Additive<A,A1>::operator() (A& attr, A1& attr_base)
{
  attr=0;
  A attr_tmp(attr.size());
  for ( Iter_t iter=distr_list.begin(); iter!=distr_list.end(); iter++)
    {
      (*iter)->operator()(attr_tmp,attr_base);
      attr += attr_tmp;
    };
};







/**
 *  setups distributions by adding them into internal list
 * 
 */
template<class A, class A1>
void Additive<A,A1>::SetupFromConfigGroup(FileInput& in)
{
  in.ChangeGroup("Additive");

  int n_vals = static_cast<int>(in.get_param("NumberOfDistributions"));

  for ( int i=0; i<n_vals; i++)
    {
      string distr_name = in.get_name("DistributionName",i);
      distr_list.push_back(InitialDistributionMaker<A,A1>(in).pMake(distr_name));
    }

  in.ChangeGroup("..");
};

#endif

