#include "PDG__rho_gj.h"

#include <iostream>
#include <string>

int main()
{
  const std::string inputfile_name = "cascade.input";
  const int n_points = 100;
  PlotData1D pd = PDG__rho_gj(inputfile_name, n_points);

  std::cout<<pd;

  return 0;
}
