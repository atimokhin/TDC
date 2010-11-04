#include <iostream>

#include "Cutpoint.h"

using namespace std;


int main()
{
  Cutpoint cm;

  const int nn=10;

  vector<double> x(nn+1);
  vector<double> f(nn+1);

  for ( int i=0; i<=nn; i++)
    {
      x[i]=double(i)/nn;
      f[i]=pow(x[i],2);
    }
  f[0]=0.001;

  cm.Initialize(x,f,nn/2);

  const int nksi = 6;
  double ksi[nksi] ={.72, .36, .94, .37, 0.0002, 0};

  for (int i=0; i<nksi; i++)
    {
      cout<<"ksi="<<ksi[i]<<"  x="<<cm(ksi[i])<<endl;  
    }

  return 0;
}

