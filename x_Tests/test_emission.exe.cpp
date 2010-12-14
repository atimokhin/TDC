#include <iostream>

#include "ATbase.h"

#include "../_Environment/MagneticField/magnetic_field_maker.h"

#include "../SetupDimensionalConstants/dimensional_consts_setup.h"
#include "../SetupParameters/geometry_params.h"

#include "../_Control/output_control.h"

#include "../_MC/Emission/CR.h"


//! config fielname
const string config_file ="test_emission.input";
const string tests_dir="x_Tests/";


int main(int argc, char *argv[])
{
  double P_par,P_perp, x0, dt; 
  double p_par, p_perp; 
  double weight = 1;

  FileInput in;
  in.ReadFile(tests_dir+config_file);

  DimensionalConstsSetup consts;
  consts.SetupFromConfig(in);

  GeometryParams geom;
  geom.SetupFromConfig(in);

  OutputControl out;
  out.SetupFromConfig(in);
  out.CreateResultsDir();
  ATbase::filesys_utils::copyfile(tests_dir+config_file, out.OutputDirName()+config_file);


  // Photons +++++++++++++++++++++
  in.ChangeGroup("TEST__Photons");
  int n_ph = static_cast<int>(in.get_param("PhotonCacheSize"));
  in.ChangeGroup();

  PhotonCache ph;
  ph.Initialize(n_ph);
  // +++++++++++++++++++++++++++++

  // ********************************************
  // Setup Magnetic field ***********************
  // ********************************************
  in.ChangeGroup("MAGNETIC_FIELD");

  string magnetic_field_name = in.get_name("Type");
  MagneticField* p_magnetic_field = MagneticFieldMaker(in).pMake(magnetic_field_name); 
  p_magnetic_field->SetupFromConfigGroup(in);

  in.ChangeGroup();
  std::cout<<*p_magnetic_field;
  // ********************************************

  // Curvature radiation +++++++++
  CR  cr;
  cr.SetMagneticField(p_magnetic_field);
  // +++++++++++++++++++++++++++++


  if ( in.get_answer("Test_CR") )
    {
      // =====================================================
      // Test Discrete emission mode 
      // =====================================================
      // setup -------------------------
      in.ChangeGroup("TEST__DISCRETE_EMISSION");

      cr.SetupFromConfigGroup(in);

      // emitting particle parameters
      in.ChangeGroup("EmittingParticle");
      dt     = in.get_param("dT");
      x0     = in.get_param("X0");
      P_par  = in.get_param("P_par");
      P_perp = in.get_param("P_perp");

      in.ChangeGroup();
      // --------------------------------

      cout<<"\n\nDiscrete emission ================== \n";

      p_par = P_par;
      cr.EmitPhotons(dt, weight, x0, p_par, ph);
      int number_of_emitted_photons = ph.Size();

      cout<<"Optical depth to Curvature emission Tau = "<<cr.Get_Tau()<<endl;
      cout<<"Total number of emitted CR photons = "<<number_of_emitted_photons<<endl;
      cout<<"EpsilonC = "<<cr.Get_EpsilonC()<<endl;

      string cr_photons_filename = out.OutputDirName() + "photons_cr_discrete.h5";
      Save2HDF hdf_cr_discrete;
      hdf_cr_discrete.create(cr_photons_filename.c_str());
      ph.Save2HDFFile(hdf_cr_discrete);
      hdf_cr_discrete.writeScalar("Tau",cr.Get_Tau());
      hdf_cr_discrete.writeScalar("F_min",cr.Get_F_min());
      hdf_cr_discrete.writeScalar("EpsilonC",cr.Get_EpsilonC());

      ph.Clear();



      // =====================================================
      // Test Continuous emission mode 
      // =====================================================      
      // setup -------------------------
      in.ChangeGroup("TEST__CONTINUOUS_EMISSION");

      cr.SetupFromConfigGroup(in);
      // emitting particle parameters
      in.ChangeGroup("EmittingParticle");
      dt     = in.get_param("dT");
      x0     = in.get_param("X0");
      P_par  = in.get_param("P_par");
      P_perp = in.get_param("P_perp");

      in.ChangeGroup();
      // -------------------------------

      cout<<"\n\nContinuous emission ================= \n";

      p_par = P_par;
      cr.EmitPhotons(dt, weight, x0, p_par, ph);
      number_of_emitted_photons = ph.Size();

      cout<<"Optical depth to Curvature emission Tau = "<<cr.Get_Tau()<<endl;
      cout<<"Total number of emitted CR photons = "<<number_of_emitted_photons<<endl;
      cout<<"EpsilonC = "<<cr.Get_EpsilonC()<<endl;

      cr_photons_filename = out.OutputDirName() + "photons_cr_cont.h5";
      Save2HDF hdf_cr_cont;
      hdf_cr_cont.create(cr_photons_filename.c_str());
      ph.Save2HDFFile(hdf_cr_cont);
      hdf_cr_cont.writeScalar("Tau",cr.Get_Tau());
      hdf_cr_cont.writeScalar("F_min",cr.Get_F_min());
      hdf_cr_cont.writeScalar("EpsilonC",cr.Get_EpsilonC());
 
      ph.Clear();
    }


  // =====================================================
  // Test Energy Losses 
  // =====================================================
  if ( in.get_answer("Test_EnergyLosses") )
    {
      cout<<"\n\n\nTest Energy Losses\n\n";
      // setup -------------------------
      in.ChangeGroup("TEST__EnergyLosses");
      int n_losses = static_cast<int>(in.get_param("IterationsOfEnergyLosses"));

      cr.SetupFromConfigGroup(in);
      // emitting particle parameters
      in.ChangeGroup("EmittingParticle");
      dt     = in.get_param("dT");
      x0     = in.get_param("X0");
      P_par  = in.get_param("P_par");
      P_perp = in.get_param("P_perp");

      in.ChangeGroup();
      // -------------------------------
      
      p_par = P_par;
      for ( int i=0; i< n_losses; i++)
	{
	  cr.EmitPhotons(dt, weight, x0, p_par, ph);	  
	}

      double e_photons=0;
      for (int i=0; i<ph.Size(); i++)
	{
	  e_photons += ph.E[i]*ph.Weight[i];
	}

      double dp=P_par-p_par;
      cout<<"\n\nEnergy Losses ======================= \n";
      cout<<"Particle energy start: "<<P_par<<"  end: "<<p_par<<"  losses: "<<dp<<"\n";
      cout<<"Energy of all emitted photons: "<<e_photons<<"\n";
      cout<<"Difference: absolute= "<<dp-e_photons<<" relative= "<<(dp-e_photons)/dp<<"\n";      
    }

  return 0;
}
