#include "magnetic_field_maker.h"

#include <iostream>

// ******************************************************
//  Add new rhogj_...
//  no further modifications necessary
// ******************************************************
#include "dipole.h"
#include "r6.h"
#include "r6_cutoff.h"


/**
 * \ingroup magnetic_field_grp
 *
 * \def  MACRO__MAKE_P_MAGNETIC_FIELD_FOR_ALL
 * \brief Single macro for all initial distributions
 * 
 * <I> update this list for each new ! </I>
 */
#define MACRO__MAKE_P_MAGNETIC_FIELD_FOR_ALL	\
  MACRO__MAKE_P_MAGNETIC_FIELD(Dipole); \
  MACRO__MAKE_P_MAGNETIC_FIELD(R6);     \
  MACRO__MAKE_P_MAGNETIC_FIELD(R6_CutOff);

// ******************************************************



/**
 * \ingroup magnetic_field_grp
 *
 * \def  MACRO__MAKE_P_RHOGJ
 * 
 * \brief this macro is associated with the function
 * MagneticFieldMaker::pMake!
 *
 * this macro generates code for:
 * - insertion of RhoGJ name into the list
 * - generation of the corresponding distribution class
 */
#define MACRO__MAKE_P_MAGNETIC_FIELD(NAME)			\
  magnetic_field_name_list.push_back(#NAME);				\
  if ( magnetic_field_name == #NAME ) {  p_magnetic_field = new NAME(); };                                   



/** 
 * \brief make MagneticField
 * 
 * uses macro MACRO__MAKE_P_MAGNETIC_FIELD via MACRO__MAKE_P_MAGNETIC_FIELD_FOR_ALL
 *
 * \return pointer to the MagneticField class object
 */
MagneticField* MagneticFieldMaker::pMake(string magnetic_field_name) const
{
  // list of names for known distributions
  std::list<string> magnetic_field_name_list;
  // set ponter to NULL - need later for check of success
  MagneticField* p_magnetic_field = NULL;


  // macros for each known distribution
  MACRO__MAKE_P_MAGNETIC_FIELD_FOR_ALL;


  // check success
  if ( !p_magnetic_field )
    {
      cout<<"Wrong RhoGJ name: "<<magnetic_field_name<<" !\n";
      cout<<"Valid types are:\n";
      for (std::list<string>::iterator iter = magnetic_field_name_list.begin();
           iter != magnetic_field_name_list.end();
           iter++)
        {
          cout<<" --> "<<*iter<<endl;
        }
      cout<<"\n\n\n";
      exit(1);
    }
  
  // setup RhoGJ by reading config file
  p_magnetic_field->SetupFromConfigGroup(_Input);

  return p_magnetic_field;
};

