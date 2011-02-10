#include <list>
#include "dimensional_consts_setup.h"

using namespace std;


// ******************************************************
// Add new dimensional constns intializers here
//  no further modifications necessary
// ******************************************************

#include "psr_consts_initializer.h"
#include "pcf_B0_consts_initializer.h"
#include "pcf_x0_consts_initializer.h"
#include "tau_pl_consts_initializer.h"
#include "x0__lD_GJ_consts_initializer.h"

/**
 * \ingroup setup_consts_grp
 *
 * \def  MACRO__MAKE_P_CONSTS_INITIALIZER_FOR_ALL
 * \brief Single macro for all initializers
 * 
 * <I> update this list for each new constants intializers! </I>
 */
#define MACRO__MAKE_P_CONSTS_INITIALIZER_FOR_ALL     \
  MACRO__MAKE_P_CONSTS_INITIALIZER(PSR_ConstsInitializer); \
  MACRO__MAKE_P_CONSTS_INITIALIZER(PcfB0_ConstsInitializer); \
  MACRO__MAKE_P_CONSTS_INITIALIZER(PcfX0_ConstsInitializer); \
  MACRO__MAKE_P_CONSTS_INITIALIZER(TauPl_ConstsInitializer); \
  MACRO__MAKE_P_CONSTS_INITIALIZER(X0__lD_GJ_ConstsInitializer);
// ******************************************************



void DimensionalConstsSetup::SetupFromConfig(FileInput &in)
{
  in.ChangeGroup("DIMENSIONAL_CONSTANTS");

  string initializer_name = in.get_name("InitializerType");
  ConstsInitializer* p_initializer = MakeInitializer(initializer_name);
  p_initializer->SetupFromConfigGroup(in);

  in.ChangeGroup();
}




/**
 * \ingroup setup_consts_grp
 *
 * \def  MACRO__MAKE_P_CONSTS_INITIALIZER
 * 
 * \brief this macro is associated with the function
 * MakeInitializer(string type)!
 *
 * this macro generates code for generation of the corresponding
 * Constant Initializer class
 */
#define MACRO__MAKE_P_CONSTS_INITIALIZER(NAME)	\
  initializer_name_list.push_back(#NAME);     \
  if ( initializer_name == #NAME ) {  p_initializer = new NAME(); };   


ConstsInitializer* DimensionalConstsSetup::MakeInitializer(string initializer_name)
{
  // list of names for known distributions
  list<string> initializer_name_list;
  // set ponter to NULL - need later for check of success
  ConstsInitializer* p_initializer = NULL;


  // macros for each known distribution
  MACRO__MAKE_P_CONSTS_INITIALIZER_FOR_ALL


    // check success
    if ( !p_initializer )
      {
	cout<<"Wrong  ConstsInitializer name: "<<initializer_name<<" !\n";
	cout<<"Valid types are:\n";
	for (list<string>::iterator iter = initializer_name_list.begin();
	     iter != initializer_name_list.end();
	     iter++)
	  {
	    cout<<" --> "<<*iter<<endl;
	  }
	cout<<"\n\n\n";
	exit(1);
      }
  
  return p_initializer;
}
