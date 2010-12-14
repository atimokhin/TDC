#include "PDG__rho_gj.h"

#include "../SetupDimensionalConstants/dimensional_consts_setup.h"
#include "../SetupParameters/geometry_params.h"
#include "../_Environment/RhoGJ/rho_gj_maker.h"

PlotData1D PDG__rho_gj(string inputfile_name, int n_points)
{
  // Input file: Config file 
  FileInput in;
  in.ReadFile( inputfile_name );

  // Setup dimensional constants: Need sign of the magnetic field
  DimensionalConstsSetup consts;
  consts.SetupFromConfig(in);

  // Setup GeometryParams: need length of the domain
  GeometryParams geom_params; 
  geom_params.SetupFromConfig(in);
  double L = geom_params.L();

  // ********************************************
  // Setup GJ charge density ********************
  // ********************************************
  in.ChangeGroup("RHO_GJ");

  string rhogj_functor_name = in.get_name("Type");
  RhoGJFunctor* p_rhogj_functor = RhoGJFunctorMaker(in).pMake(rhogj_functor_name); 
  p_rhogj_functor->SetupFromConfigGroup(in);

  in.ChangeGroup();
  // ********************************************
  
  // setup PlotData1D ---------------------------
  PlotData1D pd(n_points);
  // fill PlotData1D
  for (int i=0; i<n_points; i++)
    {
      pd.x[i]=i*L/(n_points-1);
      pd.y[i]=(*p_rhogj_functor)(pd.x[i]);
    }
  pd.set_xlim(pd.x[0],pd.x[n_points-1]);
  
  pd.ylabel="x";
  pd.ylabel="RhoGJ";
  pd.title="Goldreich-Julian charge density";
  // --------------------------------------------

  
  // return PlotData object
  return pd;
}
