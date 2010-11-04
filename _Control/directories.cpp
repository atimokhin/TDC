#include "directories.h"

#include "argtable2.h"

#include "../utils/io_filenames.h"

string Directories::_ResultsDir = InOut::ResultsDir;  //!< Dirertory with calculation results



/** 
 * Reads parameters from the command line:
 *
 * - results directory name from the command line; if no name is given,
 *   use the default value stored in InOut::ResultsDir
 *
 * @param argc 
 * @param argv 
 */
void Directories::SetupFromCommandLine(int argc, char *argv[])
{
  // --------------------------------------
  // Read command line arguments
  // --------------------------------------
  // directory *****
  struct arg_lit *help_arg =
    arg_lit0("h", "help", NULL);
  struct arg_str *dir_arg =
    arg_str0(NULL, "dir", "<DIR>","general directory with calculation results");
  // constrcut argtable
  struct arg_end *end_arg = arg_end(20);
  void *argtable[] = {help_arg,dir_arg,end_arg};
  // parse command line
  int nerrors = arg_parse(argc,argv,argtable);

  if( help_arg->count == 1 ) 
      arg_print_glossary(stdout, argtable, "   %-25s %s\n");

  if( dir_arg->count == 1 ) 
    {
      _ResultsDir = dir_arg->sval[0]; 
      _ResultsDir += "/";
    }

  // clear memory
  arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
  // -------------------------------------
}


/** 
 * Print Results dirname
 * 
 */
std::ostream& Directories::Print(std::ostream& s) const
{
  s<<"\n Results directory:   "<<ResultsDir()<<"\n\n";
  s<<std::flush;

  return s;
}

