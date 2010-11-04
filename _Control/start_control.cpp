#include "start_control.h"

#include "argtable2.h"

#include <sstream>


bool        StartControl::_RestartCalculations = false;
std::string StartControl::_ID;
int         StartControl::_StartFromStep = -1; 


/** 
 * Reads parameters from the command line:
 *
 * - call ResultsDir::SetupFromCommandLine
 *
 * @param argc 
 * @param argv 
 */
void StartControl::SetupFromCommandLine(int argc, char *argv[])
{
  // help
  struct arg_lit *help_arg =
    arg_lit0("h", "help", "print help");
  // end
  struct arg_end *end_arg = arg_end(20);
  // constrcut argtable
  void *argtable[] = {help_arg,end_arg};
  // parse command line arguments
  int nerrors = arg_parse(argc,argv,argtable);

  if( help_arg->count == 1 ) 
    {
      _AskedForHelp = true;
      std::cout<<argv[0]<<"  -- StartControl::\n";
      arg_print_glossary(stdout, argtable, "   %-25s %s\n");
      std::cout<<std::flush;
    }

  Directories::SetupFromCommandLine(argc,argv);

  // clear memory
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
}


/** 
 * Read parameters from config file
 * 
 * @param in input file class
 */
void StartControl::SetupFromConfig( FileInput &in )
{
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup();
  in.ChangeGroup("CALCULATION_START_CONTROL");

  _RestartCalculations = in.get_answer("RestartCalculations");

  if ( _RestartCalculations )
    {
      _ID = in.get_name("StartFromCalcId");
      
      _StartFromStep = static_cast<int>( in.get_param("StartFromStep") ); 
    }
  in.ChangeGroup(p_current_group);
}




/** 
 * Print Pulsar Gap parameters
 * 
 */
std::ostream& StartControl::Print(std::ostream& s) const
{
  s<<"\n Calculation Start:\n\n";
  s<<"    Restart Calculation = "<<std::boolalpha<<_RestartCalculations<<"\n";
  s<<"    InputDirName        = "<<InputDirName()<<"\n";
  s<<"    StartFromStep       = "<<StartFromTimeshot()<<"\n";
  s<<"\n";
  s<<std::resetiosflags(std::ios_base::boolalpha)<<std::flush;

  return s;
}
