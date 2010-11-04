#ifndef CONTROL_LIST_H
#define CONTROL_LIST_H

#include <ostream>
#include <list>

#include "../utils/io_filenames.h"
#include "control.h"


/**
 *  \ingroup control_grp 
 *
 *  \class ControlList
 *  \brief List containing all control classes
 * 
 */
class ControlList
{
public:
  ControlList(); 
  ~ControlList(); 

  //! Setup control parameters controlled by the command line options
  void SetupFromCommandLine(int argc, char *argv[]);
  //! Setup control parameters from config file
  void SetupFromConfig(FileInput& in);
  //! Initialize control classes
  void Initialize();

  bool AskedForHelp() const;

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

private:

  list<Control*> _List;
  
  typedef list<Control*>::const_iterator CI;
};



inline std::ostream& operator<< ( std::ostream& s, const ControlList& cl )
{
  return cl.Print(s);
};

#endif


