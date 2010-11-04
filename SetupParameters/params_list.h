#ifndef PARAMS_LIST_H
#define PARAMS_LIST_H

#include <ostream>
#include <list>

#include "../utils/io_filenames.h"
#include "setup_params.h"


/**
 *  \ingroup setup_params_grp 
 *
 *  \class ParameterList
 *  \brief List containing all calculation parameters 
 * 
 */
class ParameterList
{
public:
  ParameterList(); 
  ~ParameterList(); 

  //! Setup main calculation mode (for doing initial calculations)
  void SetupFromConfig(FileInput& in);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

private:

  list<SetupParams*> _List;
  
  typedef list<SetupParams*>::const_iterator CI;
};



inline std::ostream& operator<< ( std::ostream& s, const ParameterList& params )
{
  return params.Print(s);
};

#endif


