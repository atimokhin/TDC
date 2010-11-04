#include "stop_and_save.h"

#include<cstddef>

bool StopAndSave::_Stop_FLAG              = false;
bool StopAndSave::_Pause_FLAG             = false;
bool StopAndSave::_SaveCurrentState_FLAG  = false;

std::list<Watcher*> StopAndSave::_WatcherList = std::list<Watcher*>();


void StopAndSave::AddWatcher( Watcher* pw )
{
  _WatcherList.push_back(pw);
}


int StopAndSave::Watch()
{
  std::list<Watcher*>::iterator it;
  for ( it=_WatcherList.begin(); it !=_WatcherList.end(); it++ )
    {
      (*it)->Watch();
    };

  return 0;
}
