#ifndef TIMER_H
#define TIMER_H

#include <ctime>
#include <iostream>


class Timer
{
public:

  Timer();

  void Start();
  void Stop();

  double ElapsedTime() const;

  std::ostream& PrintCurrentTime(std::ostream&) const;
  std::ostream& PrintElapsedTime(std::ostream&) const;
  
private:

  time_t _StartTime;
  time_t _StopTime;
  time_t _CurrentTime;

  bool _StartFlag;
  bool _StopFlag;
};


inline std::ostream& operator<< (std::ostream& os, const Timer& timer )
{
  return timer.PrintCurrentTime(os);
}

#endif
