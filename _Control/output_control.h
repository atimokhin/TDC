#ifndef OUTPUT_CONTROL_H
#define OUTPUT_CONTROL_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

#include "ATbase.h"

#include "control.h"
#include "directories.h"



/**
 *  \ingroup control_grp 
 *
 *  \class OutputControl
 *  \brief Class with members for data and log output
 *
 * config file example:
 \verbatim

    Group "OUTPUT" {
       ! Identificator of calculation <------------------------------
       CalcId = 'test_2stream';  
    
       ! -> zero or negative output intervals means no output <--
       ! Number Of Timesteps after which timelabel is printed to cout 
       TimeStepStdoutPrintInterval  = 10;
       ! Number Of Timesteps after which timelabel is printed to logfile
       TimeStepLogFilePrintInterval = 40;
       ! Number Of Timesteps after which results are saved into HDF files
       TimeStepHDFFileSaveInterval  = 40;
    }

 \endverbatim
 */
class OutputControl: public Control, private Directories
{
public:

  //! Setup data from command line
  void SetupFromCommandLine(int argc, char *argv[]);
  //! Setup data from config file
  void SetupFromConfig(FileInput &in);

  //! Initialize OutputControl (create directory and copy config files)
  void Initialize();

  //! get results dirertory name
  string OutputDirName() const;
  //! get logfile file stream
  ofstream& LogStream() const;

  
  //! do verbose output&
  bool DoVerboseOutput() const;
  //! print timetep info?
  bool DoLogFileOutput(int i_time) const; 
  //! print timetep info?
  bool DoStdoutOutput(int i_time) const;
  //! save results to HDF file&
  bool DoSaveToHDFFiles(int i_time) const;

  void CreateResultsDir();

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

private:

  //! setup results dirertory
  int SetupResultsDir();
  //! diplay help on program usage
  void DisplayHelp(const char* prog_name, void* argtable[]);

private:

  static string _ID;  //!< Calculation ID
  
  static bool _Verbose; //!< Verbose flag

  static ofstream  _LogStream; //!< output stream for logfile

  static bool _Initialized;

  static int _TimeStepStdoutPrintInterval;
  static int _TimeStepLogFilePrintInterval;
  static int _TimeStepHDFFileSaveInterval;
};



//! get results dirertory name
inline string OutputControl::OutputDirName() const 
{ return ResultsDir() + "/" + _ID + "/"; };

//! get logfile files tream
inline ofstream& OutputControl::LogStream() const 
{ return _LogStream; };

//! do verbose output&
inline bool OutputControl::DoVerboseOutput() const 
{ return _Verbose; };



//! print timetep info?
inline bool OutputControl::DoLogFileOutput(int i_time) const 
{ 
  return (_TimeStepLogFilePrintInterval>0 ? 
          i_time%_TimeStepLogFilePrintInterval == 0 : false ); 
};


//! print timetep info?
inline bool OutputControl::DoStdoutOutput(int i_time)  const 
{ 
  return (_TimeStepStdoutPrintInterval>0 ? 
          i_time%_TimeStepStdoutPrintInterval == 0 : false ); 
};


//! save results to HDF file&
inline bool OutputControl::DoSaveToHDFFiles(int i_time) const
{ 
#ifdef TEST_CODE
  std::cout<<"->DoSaveToHDFFiles\n"<<std::flush;
#endif
  return (_TimeStepHDFFileSaveInterval>0 ? 
          i_time%_TimeStepHDFFileSaveInterval == 0 : false ); 
};

#endif


