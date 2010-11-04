#include "params_list.h"

#include "geometry_params.h"
#include "grid_params.h"
#include "particles_params.h"
#include "time_params.h"

/** 
 * Constructor.
 *
 * Adds each memeber into internal memeber list mList
 * <B> must be modified each time a new member is added !</B>
 * 
 */
ParameterList::ParameterList()
{
  _List.push_back( new GeometryParams() );
  _List.push_back( new GridParams()   );
  _List.push_back( new ParticlesParams()   );
  _List.push_back( new TimeParams()   );
}


/** 
 * Destructor.
 *
 */
ParameterList::~ParameterList()
{
  for ( CI p = _List.begin();  p != _List.end(); ++p )
      delete *p;  
}


/** 
 * Setup main calculation mode
 *
 * Calculation is not performed yet. Initial parameters are read fron
 * config file
 * 
 * @param in FileInput object associated with config file
 */
void ParameterList::SetupFromConfig(FileInput& in)
{
  for ( CI p = _List.begin();  p != _List.end(); ++p )
      (*p)->SetupFromConfig(in);  
}





/** 
 * Prints each memeber into the output steram s
 * 
 * @param s output stream
 */
ostream& ParameterList::Print(ostream& s) const
{
  string separator=string(40,'=')+"\n";
  
  s<<"\n"<<separator;
  s<<"Parameters of calculation :\n";
  s<<separator;
  for ( CI p = _List.begin();  p != _List.end(); ++p )
    {
      s<<*(*p);
    }
  s<<separator<<"\n";
  return s;
}

