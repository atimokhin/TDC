/**
 *  \ingroup test_grp
 *
 *  \file  test_inout.exe.cpp
 *  \brief file for testing inpu-output
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

#include <vector>
#include <list>
#include <deque>

#include "Pooma/Fields.h"

#include "ATbase.h"

#include "../utils/stl_utils.h"
#include "../inout/save2hdf.h"
#include "../inout/timetable.h"

#include "../_Control/output_control.h"
#include "../_Control/start_control.h"

#include "../_SolutionProperties/QuantitiesVsTime.h"
#include "../_SolutionProperties/particle_flux.h"




//! config fielname
const string config_file ="test_inout.input";
const string tests_dir="x_Tests/";


//! Main function for cascade simulation
int main(int argc, char *argv[])
{
  using namespace std;

  // *********************************************************
  // setup parameters from input file                   
  // *********************************************************
  FileInput in;
  in.ReadFile(tests_dir+config_file);

  OutputControl out;
  out.SetupFromCommandLine(argc,argv);
  out.SetupFromConfig(in);
  out.CreateResultsDir();
  ATbase::filesys_utils::copyfile(tests_dir+config_file, out.OutputDirName()+config_file);

  StartControl calc_start;
  calc_start.SetupFromConfig(in);

  string timetable_output_file = out.OutputDirName() + "test_timetable.h5";
  Save2HDF hdf_timetable;
  hdf_timetable.create(timetable_output_file.c_str());


  // **********************************************
  // test timetable
  // **********************************************
  Timetable tt;
  tt.SetStartTimestep(201);
  tt.SetNewTime(20,1);
  tt.UpdateTimetableInHDFFile(hdf_timetable);
  tt.SetNewTime(40,2);
  tt.UpdateTimetableInHDFFile(hdf_timetable);
  tt.SetNewTime(40,3);
  tt.UpdateTimetableInHDFFile(hdf_timetable);
  tt.SetNewTime(40,4);
  tt.UpdateTimetableInHDFFile(hdf_timetable);

  hdf_timetable.close();

  Save2HDF hdf2_timetable;
  hdf2_timetable.open(timetable_output_file.c_str());
  cout<<"test Timetable:\n";
  cout<<"Timestep number for timeshots\n";
  
  Timetable tt2;
  int n_timeshots= tt2.GetNumbeOfTimeshots(hdf2_timetable);
  for (int i=1; i<=n_timeshots; i++)
    {
      cout<<"timeshot : "<<i<<" time "<<tt2.GetTimeOfTheTimeshot(hdf2_timetable, i);
      cout<<" timestep "<<tt2.GetTimestepOfTheTimeshot(hdf2_timetable, i)<<"\n";
    }
  hdf2_timetable.close();
  cout<<"\n";

  // **********************************************
  // test save STL container
  // **********************************************
  string stl_output_file = out.OutputDirName() + "test_stl.h5";
  Save2HDF hdf_stl;
  hdf_stl.create(stl_output_file.c_str());

  const int N = 10;
  vector<short> vs(N);
  list<double>  ld;
  deque<int> dd;
  for (int i=0; i<N; i++)
    {
      vs[i] = i;
      ld.push_back(i);
      dd.push_back(i);
    }

  hdf_stl.SaveSTLContainer("vs",vs);
  hdf_stl.SaveSTLContainer("ld",ld);
  hdf_stl.SaveSTLContainer("dd",dd);

  hdf_stl.close();



  // --------------------------------
  // Initialize POOMA 
  // --------------------------------
  Pooma::initialize(argc, argv);


  // **********************************************
  // Field and array tests
  // **********************************************
  int nx = 10;

  // Mesh type and Point type **************
  typedef MeshTraits<1,double,UniformRectilinearTag,CartesianTag,1>::Mesh_t  Mesh_t; 
  typedef Mesh_t::PointType_t PointType_t; 

  // Field type and Field layout ***********
  typedef Field<Mesh_t,double,MultiPatch<GridTag,Brick> >  Field_t;
  typedef Field_t::Engine_t::Layout_t                      FLayout_t;
  typedef Mesh_t::SpacingsType_t  SpacingsType_t;

  // Create field layout ****************************
  Interval<1>    meshDomain(0,nx);
  GuardLayers<1> gl(1);
  Loc<1>         blocks(1);
  // Field layout
  FLayout_t flayout(meshDomain,blocks,gl,DistributedTag());
  // *************************************************

  // Create Mesh ************************************
  double dx = 1.0/nx;
  SpacingsType_t spacings(dx);
  // Mesh
  Mesh_t mesh(flayout, Vector<1>(0), spacings);
  // *************************************************

  Centering<1> vertex = canonicalCentering<1>(VertexType, Continuous);  

  Field_t field, field2;
  field.initialize(vertex,flayout,mesh);
  field2.initialize(vertex,flayout,mesh);

  Interval<1> I=field.layout().domain();
  for ( int i=I.first(); i <=I.last(); i++)
    field(i)=i;

  // test saving field
  string field_output_file = out.OutputDirName() + "test_field.h5";
  Save2HDF hdf_field;
  hdf_field.create(field_output_file.c_str());
  hdf_field.writeField("test_field",field,gl);
  cout<<"test field : "<<field(I)<<"\n";

  // test saving field positions
  Array<1,PointType_t> a( field.domain() );
  a = Pooma::positions( field );
  cout<<"test field positions: "<<a<<"\n";
  hdf_field.writeArray( "test_field_positions", a );

  // test saving 1D array
  Array<1,double> arr(nx);
  for (int i=0; i<nx; i++)
    arr(i)=2*i;
  cout<<"test array: "<<arr<<"\n";
  hdf_field.writeArray( "test_array", arr );

  hdf_field.close();



  Save2HDF hdf2;
  hdf2.open(field_output_file.c_str());

  hdf2.readField("test_field",field2,gl);
  cout<<"read field: "<<field2(I)<<"\n";
  hdf2.close();





  // **********************************************
  // QuantitiesVsTime test
  // **********************************************

  ParticleFlux QT(10); 
  in.ChangeGroup("NS_PARTICLE_FLUX");
  QT.SetupFromConfigGroup(in);
  in.ChangeGroup();


  QT.AddNewValue(QT.TimeArray, 1.);
  QT.AddNewValue(QT.J, 10.);
  QT.AddNewValue(QT.EnergyFlux, 100.);

  QT.AddNewValue(QT.TimeArray, 2.);
  QT.AddNewValue(QT.J, 20.);
  QT.AddNewValue(QT.EnergyFlux, 200.);

  QT.AddNewValue(QT.TimeArray, 3.);
  QT.AddNewValue(QT.J, 30.);
  QT.AddNewValue(QT.EnergyFlux, 300.);

  QT.SetCurrentTime(3.);
  QT.SaveToHDFFile();
  cout<<"\n"<<QT;

  // these values will be skipped
  QT.AddNewValue(QT.TimeArray, 4.);
  QT.AddNewValue(QT.J, 40.);
  QT.AddNewValue(QT.EnergyFlux, 400.);

  QT.AddNewValue(QT.TimeArray, 5.);
  QT.AddNewValue(QT.J, 50.);
  QT.AddNewValue(QT.EnergyFlux, 500.);

  QT.AddNewValue(QT.TimeArray, 6.);
  QT.AddNewValue(QT.J, 60.);
  QT.AddNewValue(QT.EnergyFlux, 600.);

  QT.AddNewValue(QT.TimeArray, 7.);
  QT.AddNewValue(QT.J, 70.);
  QT.AddNewValue(QT.EnergyFlux, 700.);

  QT.SetCurrentTime(6.);
  QT.SaveToHDFFile();

  cout<<"\n"<<QT;

  // redefine values
  QT.J[QT.GetICurrent()] = 61;
  QT.EnergyFlux[QT.GetICurrent()] = 601;

  QT.J[QT.GetICurrent()+1] = 71;
  QT.EnergyFlux[QT.GetICurrent()+1] = 701;

  QT.AddNewValue(QT.TimeArray, 8.);
  QT.AddNewValue(QT.J, 80.);
  QT.AddNewValue(QT.EnergyFlux, 800.);

  QT.SetCurrentTime(8.);
  QT.SaveToHDFFile();

  cout<<"\n"<<QT;


  ParticleFlux QT1(10); 
  in.ChangeGroup("NS_PARTICLE_FLUX_1");
  QT1.SetupFromHDFFileAndConfigGroup(in);
  in.ChangeGroup();
  
  cout<<"\nQT1 before saving to hdf \n"<<QT1;

  QT1.AddNewValue(QT.TimeArray, 9.);
  QT1.SaveToHDFFile();

  cout<<"\nQT1\n"<<QT1;

  // --------------------------------
  // Finalize POOMA 
  // --------------------------------
  Pooma::finalize();




  return 0; 
}
