#ifndef  STOP_AND_SAVE_H
#define  STOP_AND_SAVE_H

#include <list>
#include "watcher.h"

/**
 * \brief StopAndSave class for control of safe termination of
 * calculations and flushing intermediate state of computations
 * 
 */
class StopAndSave
{
public:
  void AddWatcher(Watcher* pw);
  int  Watch();

  void Request_Stop()     { _Stop_FLAG = true; };
  bool Asked_Stop() const { return _Stop_FLAG; };

  void Request_Pause()     { _Pause_FLAG = true;  };
  void Request_Resume()    { _Pause_FLAG = false; };
  bool Asked_Pause() const { return _Pause_FLAG;  };

  void Request_SaveCurrentState()     { _SaveCurrentState_FLAG = true; };
  bool Asked_SaveCurrentState() const { return _SaveCurrentState_FLAG; };

private:
  static std::list<Watcher*> _WatcherList;

  static bool _Stop_FLAG;
  static bool _SaveCurrentState_FLAG; //! true if results should be saved first 

  static bool _Pause_FLAG; //! true if results should be flashed and computations paused 
};

#endif
