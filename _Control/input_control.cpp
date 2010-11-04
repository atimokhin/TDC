#include "input_control.h"

#include <sstream>
#include "argtable2.h"

#include "../utils/io_filenames.h"


std::string InputControl::_ID;
std::string InputControl::_ConfigFileName=InOut::SetupFile;

/** 
 * Reads parameters from the command line:
 *
 * - call ResultsDir::SetupFromCommandLine
 *
 * @param argc 
 * @param argv 
 */
void InputControl::SetupFromCommandLine(int argc, char *argv[])
{
  // help
  struct arg_lit *help_arg =
    arg_lit0("h", "help", "print help");
  // id
  struct arg_str *id_arg =
    arg_str0(NULL, "id",  "<ID>", "calculation id");
  // end
  struct arg_end *end_arg = arg_end(20);
  // constrcut argtable
  void *argtable[] = {help_arg,id_arg,end_arg};
  // parse command line arguments
  int nerrors = arg_parse(argc,argv,argtable);

  if( help_arg->count == 1 ) 
    {
      _AskedForHelp = true;
      std::cout<<argv[0]<<"  -- InputControl::\n";
      arg_print_glossary(stdout, argtable, "   %-25s %s\n");
      std::cout<<std::flush;
    }

  Directories::SetupFromCommandLine(argc,argv);

  // if -id tag was set - use it
  if ( id_arg->count == 1 )
    {
      _ConfigFileName = ResultsDir() + 
        std::string(id_arg->sval[0]) + "/" + InOut::SetupFile;
    }

  // clear memory
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
}


/** 
 * Read parameters from config file:
 * - read ID to be used for data input
 *
 * Usually should be used with config file provided by ConfigFileName()
 * 
 * @param in input file class
 */
void InputControl::SetupFromConfig( FileInput &in )
{
  // save currenbt group position
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup();
  in.ChangeGroup("OUTPUT_CONTROL");
      
  _ID = in.get_name("CalcId");
      
  // return to the previous group
  in.ChangeGroup(p_current_group);
}




/** 
 * Print InputControl parameters
 * 
 */
std::ostream& InputControl::Print(std::ostream& s) const
{
  s<<"\n Input Control:\n\n";
  s<<"    Config file   = "<<ConfigFileName()<<"\n";
  s<<"    InputDirName  = "<<InputDirName()<<"\n\n";

  return s;
}
