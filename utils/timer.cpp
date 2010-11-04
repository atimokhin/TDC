#include "timer.h"


Timer::Timer()
{
  _StartFlag = false;
  _StopFlag = false;
}




void Timer::Start() 
{ 
  _CurrentTime=_StartTime = time(NULL); 
  _StartFlag = true;
};


void Timer::Stop()  
{ 
  _CurrentTime=_StopTime  = time(NULL); 
  _StopFlag=true;
};

double Timer::ElapsedTime() const
{ 

  if ( !_StartFlag || !_StopFlag )
    {
      std::cerr<<"Timer: timing is wrong!\n";
      return -1;
    }

  return difftime(_StopTime,_StartTime); 
};

std::ostream& Timer::PrintElapsedTime(std::ostream& os) const
{ 

  if ( !_StartFlag || !_StopFlag )
    {
      os<<"Timer: timing is wrong!<<<<<<<<<<<<<<<\n";
    }

  os<<"\nTiming :\n";
  os<<"start @ "<<ctime(&_StartTime);
  os<<"stop  @ "<<ctime(&_StopTime);
  os<<"---------------\n";
  os<<"Elapsed Time : "<<difftime(_StopTime,_StartTime)<<" seconds\n";

  return os;
};

std::ostream& Timer::PrintCurrentTime(std::ostream& os) const
{ 

  if ( !_StartFlag && !_StopFlag )
    {
      os<<"Timer: time is wrong!<<<<<<<<<<<<<<<\n";
    }

  os<<ctime(&_CurrentTime);

  return os;
};
