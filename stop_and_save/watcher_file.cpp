#include "ATbase.h"

#include "watcher_file.h"

#include "stop_and_save.h"


namespace filenames
{
  const std::string stop_and_save = "___STOP_AND_SAVE";
  const std::string stop          = "___STOP";
  const std::string pause         = "___PAUSE";
  const std::string resume        = "___RESUME";
}


int WatcherFile::Watch()
{
  using namespace ATbase;
  StopAndSave stop;

  if ( filesys_utils::file_exists(_OutputDir+filenames::resume) )
    {
      stop.Request_Resume();
      filesys_utils::deletefile(_OutputDir+filenames::pause);
      return 1;
    }
  else if ( filesys_utils::file_exists(_OutputDir+filenames::stop) )
    {
      stop.Request_Stop();
      return 1;
    }
  else if ( filesys_utils::file_exists(_OutputDir+filenames::stop_and_save) )
    {
      stop.Request_SaveCurrentState();
      stop.Request_Stop();
      return 1;
    }
  else if ( filesys_utils::file_exists(_OutputDir+filenames::pause) )
    {
      stop.Request_Pause();
      return 1;
    }
  else
    return 0;
}
