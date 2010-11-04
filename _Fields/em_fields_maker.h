#ifndef EM_FIELDS_MAKER_H
#define EM_FIELDS_MAKER_H

#include <string>
#include <list>

#include "ATbase.h"
#include "em_fields.h"


// ******************************************************
// Add new fields here
//  no further modifications necessary
// ******************************************************

#include "electrostatic_1D.h"

/**
 * \ingroup fields_grp
 *
 * \def  MACRO__MAKE_P_FIELDS_FOR_ALL
 * \brief Single macro for all Fileds
 * 
 * <I> update this list for each new initial distribution! </I>
 */
#define MACRO__MAKE_P_FIELDS_FOR_ALL     \
  MACRO__MAKE_P_FIELDS(Electrostatic_1D);
// ******************************************************



/**
 * \ingroup fields_grp
 * \class   EMFieldsMaker
 *
 * Makes EMFields object by reading information from configuration
 * file
 */
template<class Field_t>
class EMFieldsMaker
{
public:

  //! make EMFields class object from config file
  EMFields<Field_t>* pMake( string type ) const;
};




/**
 * \ingroup fields_grp
 *
 * \def  MACRO__MAKE_P_FIELDS
 * 
 * \brief this macro is associated with the function
 * EMFieldsMaker<Field_t>::pMake!
 *
 * this macro generates code for generation of the corresponding
 * fields class
 */
#define MACRO__MAKE_P_FIELDS(NAME)	\
  fields_name_list.push_back(#NAME);     \
  if ( fields_name == #NAME ) {  p_fields = new NAME<Field_t>(); };   


/** 
 * \brief make EMFields 
 * 
 * uses macro MACRO__MAKE_P_FIELDS via MACRO__MAKE_P_FIELDS_FOR_ALL
 *
 * \return pointer to the InitialDistribution<A> class object
 */
template<class Field_t>
EMFields<Field_t>* 
EMFieldsMaker<Field_t>::pMake(string fields_name) const
{
  // list of names for known distributions
  std::list<string> fields_name_list;
  // set ponter to NULL - need later for check of success
  EMFields<Field_t>* p_fields = NULL;


  // macros for each known distribution
  MACRO__MAKE_P_FIELDS_FOR_ALL


    // check success
    if ( !p_fields )
      {
	cout<<"Wrong EMFields name: "<<fields_name<<" !\n";
	cout<<"Valid types are:\n";
	for (std::list<string>::iterator iter = fields_name_list.begin();
	     iter != fields_name_list.end();
	     iter++)
	  {
	    cout<<" --> "<<*iter<<endl;
	  }
	cout<<"\n\n\n";
	exit(1);
      }
  
  return p_fields;
};

#endif
