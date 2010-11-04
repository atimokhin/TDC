#include "output_control.h"

#include "argtable2.h"

#include "../utils/io_filenames.h"

string    OutputControl::_ID;                        //!< Calculation ID  
bool      OutputControl::_Verbose=false;             //!< Verbose 
ofstream  OutputControl::_LogStream;                 //!< output stream for logfile

bool      OutputControl::_Initialized=false;         //!< Initialized? 

int       OutputControl::_TimeStepStdoutPrintInterval=1;
int       OutputControl::_TimeStepLogFilePrintInterval=1;
int       OutputControl::_TimeStepHDFFileSaveInterval=1;


/** 
 * Reads parameters from the command line:
 *
 * - call ResultsDir::SetupFromCommandLine
 *
 * @param argc 
 * @param argv 
 */
void OutputControl::SetupFromCommandLine(int argc, char *argv[])
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
      std::cout<<argv[0]<<"  -- OutputControl::\n";
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
void OutputControl::SetupFromConfig( FileInput &in )
{
  // save currenbt group position
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup();
  in.ChangeGroup("OUTPUT_CONTROL");
      
  _ID = in.get_name("CalcId");
      
  _TimeStepLogFilePrintInterval = 
    static_cast<int>(in.get_param("TimeStepLogFilePrintInterval"));
  _TimeStepStdoutPrintInterval = 
    static_cast<int>(in.get_param("TimeStepStdoutPrintInterval"));      
  _TimeStepHDFFileSaveInterval = 
    static_cast<int>(in.get_param("TimeStepHDFFileSaveInterval"));

  // return to the previous group
  in.ChangeGroup(p_current_group);
}


/** 
 * Initializes OutputControl
 * 
 * - creates results directory and copies necessary setup files there
 * - initializes logstream
 * 
 * @return 
 */
void OutputControl::Initialize()
{
  if ( !_Initialized )
    {
      // Setup Results directory
      SetupResultsDir();
  
      // create logstream
      string logfilename = OutputDirName()+InOut::LogFile;
      _LogStream.open( logfilename.c_str() );
  
      _Initialized=true;
    }
}



/** 
 * Makes Results directory and copy config file(s) there
 * 
 * 
 * @return 0 if cucceeded
 */
int OutputControl::SetupResultsDir()
{
  using namespace ATbase::filesys_utils;
  makedir(OutputDirName());
  copyfile(InOut::SetupFile,OutputDirName()+InOut::SetupFile);
  copyfile(InOut::CompilerOptsFile,OutputDirName()+InOut::CompilerOptsFile);
  return 0;
}



/** 
 * Print Pulsar Gap parameters
 * 
 */
std::ostream& OutputControl::Print(std::ostream& s) const
{
  s<<"\n Results will be written in:\n\n   ";
  s<<OutputDirName()<<"\n\n";
  s<<std::flush;

  return s;
}


