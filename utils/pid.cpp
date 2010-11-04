#include "pid.h"

#include<fstream>
#include<cstdlib>


std::ostream& PID::Print(std::ostream& os) const
{
  os<<"process id : "<<_ID<<"\n";
  return os;
}

void PID::SavePIDToFile(std::string dirname) const
{
  std::string filename = dirname + "pid";
  std::ofstream fs(filename.c_str());
  if ( !fs.is_open() )
    {
      std::cerr<<"PID: cannont open \""<<filename<<"\"\n";
      exit(1);
    }

  fs<<_ID;
  fs.close();
}

std::ostream& operator<< (std::ostream& os, const PID& pid)
{
  return pid.Print(os);
}
