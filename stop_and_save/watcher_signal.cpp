#include "watcher_signal.h"

#include <iostream>
#include <stdlib.h> 
#include <signal.h> 
#include <string.h>

#include "stop_and_save.h"


int WatcherSignal::_NumberOfRequests_SIGTERM = 0;
int WatcherSignal::_NumberOfRequests_SIGINT  = 0;


WatcherSignal::WatcherSignal()
{
  struct sigaction act;
  act.sa_handler = WatcherSignal_namespace::CatchSignal;
  sigemptyset(&act.sa_mask);   
  act.sa_flags = 0; 
#ifdef  SA_RESTART         
  act.sa_flags |= SA_RESTART; 
#endif 
  
  sigaction(SIGTERM,  &act, NULL);
  sigaction(SIGINT,   &act, NULL);
  sigaction(SIGUSR1,  &act, NULL);

  _NumberOfRequests_SIGTERM = 0;
  _NumberOfRequests_SIGINT  = 0;
}


/** 
 * Do actually signal catching. If number of interrupts is larger 
 * than 1 aborts calculation.
 * 
 * @param sig signal ID
 */
void WatcherSignal_namespace::CatchSignal(int sig)
{
  std::cout<<"\n >>>>>>>>>>>>> Signal \""<<strsignal(sig);
  std::cout<<"\" is received!\n\n"<<std::flush;
  
  StopAndSave stop;

  if ( sig == SIGTERM )
    {
      stop.Request_Stop();
      if ( ++WatcherSignal::_NumberOfRequests_SIGTERM > 1) abort();
    }
  else if ( sig == SIGINT )
    {
      stop.Request_SaveCurrentState();
      stop.Request_Stop();
      if ( ++WatcherSignal::_NumberOfRequests_SIGINT > 1) abort();
    }
  else if ( sig == SIGUSR1 )
    {
      if ( stop.Asked_Pause() )
        stop.Request_Resume();
      else
        stop.Request_Pause();
    }
}
