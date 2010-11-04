#ifndef CASCADE_PID_H
#define CASCADE_PID_H

#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

class PID
{
public:

  PID() {_ID = getpid(); };

  std::ostream& Print(std::ostream&) const;

  void SavePIDToFile(std::string) const;

private:

  pid_t _ID;
};


std::ostream& operator<< (std::ostream& os, const PID& pid);


#endif
