#ifndef  WATCHER_SIGNAL_H
#define  WATCHER_SIGNAL_H

#include "watcher.h"

namespace WatcherSignal_namespace
{
  void CatchSignal(int sig);
}


class WatcherSignal: public Watcher
{
  friend void WatcherSignal_namespace::CatchSignal(int sig);
public:
  //! Setup catching of the signal sig
  WatcherSignal();

  int Watch() { return 0; };
  
private:
  static int _NumberOfRequests_SIGTERM;
  static int _NumberOfRequests_SIGINT;
};

#endif
