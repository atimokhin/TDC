#include "control_list.h"

#include "code_control.h"
#include "output_control.h"
#include "start_control.h"

/** 
 * Constructor.
 *
 * Adds each memeber into internal memeber list _List
 * <B> must be modified each time a new member is added !</B>
 * 
 */
ControlList::ControlList()
{
  _List.push_back( new CodeControl() );
  _List.push_back( new OutputControl()   );
  _List.push_back( new StartControl()   );
}


/** 
 * Destructor.
 *
 */
ControlList::~ControlList()
{
  for ( CI p = _List.begin();  p != _List.end(); ++p )
    delete *p;  
}


/** 
 * Setup control parameters controlled by the command line options
 * 
 * @param argc 
 * @param argv 
 */
void ControlList::SetupFromCommandLine(int argc, char *argv[])
{
  for ( CI p = _List.begin();  p != _List.end(); ++p )
      (*p)->SetupFromCommandLine(argc, argv);  
}

/** 
 * Setup Control parameters
 * 
 * @param in FileInput object associated with config file
 */
void ControlList::SetupFromConfig(FileInput& in)
{
  for ( CI p = _List.begin();  p != _List.end(); ++p )
      (*p)->SetupFromConfig(in);  
}


/** 
 * Initialize control classes
 * 
 */
void ControlList::Initialize()
{
  for ( CI p = _List.begin();  p != _List.end(); ++p )
    (*p)->Initialize();
}


/** 
 * Return true if help was asked
 * 
 */
bool ControlList::AskedForHelp() const
{
  for ( CI p = _List.begin();  p != _List.end(); ++p )
    if ( (*p)->AskedForHelp() )
      return true;  
  return false;
}




/** 
 * Prints each memeber into the output steram s
 * 
 * @param s output stream
 */
ostream& ControlList::Print(ostream& s) const
{
  string separator=string(40,'=')+"\n";
  
  s<<"\n"<<separator;
  s<<"Control classes :\n";
  s<<separator;
  for ( CI p = _List.begin();  p != _List.end(); ++p )
    {
      s<<*(*p);
    }
  s<<separator<<"\n";
  return s;
}

