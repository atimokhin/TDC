#include <limits>
#include <iostream>

int main()
{

  std::cout<<"min long "<<std::numeric_limits<long>::min()<<std::endl;
  std::cout<<"max long "<<std::numeric_limits<long>::max()<<std::endl;
  std::cout<<"sizeof long "<<sizeof(long)<<std::endl;
  std::cout<<"\n";
  
  std::cout<<"min int "<<std::numeric_limits<int>::min()<<std::endl;
  std::cout<<"max int "<<std::numeric_limits<int>::max()<<std::endl;
  std::cout<<"sizeof int "<<sizeof(int)<<std::endl;
  std::cout<<"\n";

  std::cout<<"min short "<<std::numeric_limits<short int>::min()<<std::endl;
  std::cout<<"max short "<<std::numeric_limits<short int>::max()<<std::endl;
  std::cout<<"sizeof short "<<sizeof(short int)<<std::endl;
  std::cout<<"\n";

  std::cout<<"min double "<<std::numeric_limits<double>::min()<<std::endl;
  std::cout<<"max double "<<std::numeric_limits<double>::max()<<std::endl;
  std::cout<<"sizeof double "<<sizeof(double)<<std::endl;
  std::cout<<"\n";
  
  return 0;
}
