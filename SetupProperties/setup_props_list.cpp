#include "setup_props_list.h"

#include "../_Control/output_control.h"

#include "fmp_props.h"
#include "plasma_props.h"
#include "grid_props.h"
#include "pulsar_gap_props.h"

#include "rho_gj_props.h"
#include "RS_cascade_props.h"


/** 
 * Constructor.
 *
 * Adds each memeber into internal memeber list _List
 * <B> must be modified each time a new member is added !</B>
 * 
 * following SetupParametrs are requied:
 * - OutputParams
 * - StartControl
 */
SetupPropsList::SetupPropsList()
{
  _List.push_back( new FMPProps() );
  _List.push_back( new PlasmaProps() );
  _List.push_back( new GridProps() );
  _List.push_back( new PulsarGapProps() );

  _List.push_back( new RSCascadeProps() );
  _List.push_back( new RhoGJProps() );
}


/** 
 * Destructor.
 *
 *  Destrouys each element in list
 */
SetupPropsList::~SetupPropsList()
{
  for ( CI p = _List.begin();  p != _List.end(); ++p )
    delete *p;  
}



void SetupPropsList::Initialize(FileInput &in)
{
  for ( CI p = _List.begin();  p != _List.end(); ++p )
      (*p)->Initialize(in);  
}


void SetupPropsList::SaveToHDFFile() const
{
  Save2HDF hdf;
  OutputControl out;
  std::string filename = out.OutputDirName()+InOut::SetupPropertiesFile;
  hdf.create(filename.c_str());

  for ( CI p = _List.begin();  p != _List.end(); ++p )
      (*p)->SaveToHDFFile(hdf);  
}


void SetupPropsList::ReadFromHDFFile(Save2HDF& hdf)
{
  for ( CI p = _List.begin();  p != _List.end(); ++p )
      (*p)->ReadFromHDFFile(hdf);  
}





/** 
 * Prints each memeber into the output steram s
 * 
 * @param s output stream
 */
ostream& SetupPropsList::Print(ostream& s) const
{
  string separator=string(40,'=')+"\n";
  
  s<<"\n"<<separator;
  s<<"Physical properties of the system :\n";
  s<<separator;
  for ( CI p = _List.begin();  p != _List.end(); ++p )
    {
      s<<*(*p);
    }
  s<<separator<<"\n";
  return s;
}

