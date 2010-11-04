#include "timetable.h"

#include <iostream>
#include <sstream>

using namespace std;

int Timetable::_START_TIMESTEP;  //! global timestep number at calculation start

//! constructor
Timetable::Timetable()
{
  _Counter=0;
  _QuantityListSize=0;
}

//! destructor (empty)
Timetable::~Timetable()
{}


/** 
 * Sets global consecutive timestep number for the current calculation.
 * 
 * @param it timestep
 */
void Timetable::SetStartTimestep(int it_start)
{
  //new timestep
  _START_TIMESTEP=it_start;
}

/** 
 * Sets current time, increase internal counter and generate string
 * with the name of the current dataset
 * 
 * @param t  current time
 * @param it current timestep number <b>( local for the current computation )</b>
 */
void Timetable::SetNewTime(double t, int it)
{
  //new time
  _T = t;
  //new timestep
  _Timestep=_START_TIMESTEP+it;
  //increment counter
  ++_Counter;
  
  //new current name
  std::ostringstream ost;
  ost<<_Counter;
  _DatasetName=ost.str();
}


/** 
 * add name of the saved physical quantity to the list
 * 
 * @param qName name of the physical quantity which will be traced 
 */
void Timetable::AddQuantityToList( std::string qName ) 
{ 
  _QuantityList.push_back(qName); 
  _QuantityListSize = _QuantityList.size();
};


/**
 * Each time method UpdateTimetableInHDFFile(Save2HDF &hdf) is called
 * it goes trough the internal list of names _QuantityList and looks
 * for dataset with the name _QuantityList{i}/_Counter and saves
 * reference to this dataset in /Timetable/Refs/_QuantityList{i}
 * extending it
 *
 * It overwrites number in /Timetable/NumberOfTimeshots, so it correspond to
 * the current total number of timeshots
 *
 * It adds new value of time to /Timetable/Time datset
 * 
 */
void Timetable::UpdateTimetableInHDFFile(Save2HDF &hdf)
{
  using namespace Timetable_namespace;

  hid_t file_id, group_id, ref_group_id;
  hid_t dataspace, dataspace_timeshots;
  hid_t dset_time, dset_timestep, dset_timeshots;

  herr_t  status;  
  hsize_t dims[1], maxdims[1];

  hid_t       dataset_ref[_QuantityListSize];
  hobj_ref_t  wbuf[1];

  list<string>::const_iterator iter;
  int i;


  // ---------------------------------------------------
  // Create or open necessary groups and datasets
  // ---------------------------------------------------

  file_id = hdf.GetFileId();

  // ***************************************************
  // if this is a first timeshot, create necessary groups
  // ***************************************************
  if ( _Counter == 1 )
    {
      // Create the data space with unlimited dimensions.
      // it will be used for time and timestep datasets
      dims[0]    = 1;
      maxdims[0] = H5S_UNLIMITED;
      dataspace  = H5Screate_simple (1, dims, maxdims); 

      // Modify dataset creation properties, i.e. enable chunking 
      hsize_t chunk_dims[1] = {10};
      hid_t   cparms = H5Pcreate(H5P_DATASET_CREATE);
      status = H5Pset_chunk(cparms, 1, chunk_dims);

      // Create groups
      group_id     = H5Gcreate(file_id, TIMETABLE_GRP_NAME.c_str(), 0);
      ref_group_id = H5Gcreate(group_id, REF_GRP_NAME.c_str(), 0);


      // +++++++++++++++++++++++++++++++++++++++++++++++
      // Create a new "NumberOfTimeshots" dataset       
      dataspace_timeshots = H5Screate_simple (1, dims, dims); 
      dset_timeshots = H5Dcreate (group_id, NTIMESHOTS_NAME.c_str(), 
				  H5T_NATIVE_INT, dataspace_timeshots,
				  H5P_DEFAULT);
      H5Sclose(dataspace_timeshots);

      // +++++++++++++++++++++++++++++++++++++++++++++++
      // Create a new "Time" dataset 
      dset_time = H5Dcreate (group_id, TIMEARRAY_NAME.c_str(), 
			   H5T_NATIVE_DOUBLE, dataspace,
			   cparms);
      status = H5Dextend(dset_time, dims);

      // +++++++++++++++++++++++++++++++++++++++++++++++
      // Create a new "Timestep" dataset 
      dset_timestep = H5Dcreate (group_id, TIMESTEPARRAY_NAME.c_str(), 
				 H5T_NATIVE_INT, dataspace,
				 cparms);
      status = H5Dextend(dset_timestep, dims);

      // +++++++++++++++++++++++++++++++++++++++++++++++
      // Create a datasets to store references to physical quantities
      for ( iter = _QuantityList.begin(), i=0; 
	    iter != _QuantityList.end(); 
	    ++iter, ++i )
	{
	  dataset_ref[i] = H5Dcreate(ref_group_id, iter->c_str(), 
				     H5T_STD_REF_OBJ, dataspace,
				     cparms);
	  status = H5Dextend(dataset_ref[i], dims);
	}
    }
  // ***************************************************
  // if not - open alreasdy existring groups
  // ***************************************************
  else
    {
      // open groups
      group_id     = H5Gopen(file_id, TIMETABLE_GRP_NAME.c_str());
      ref_group_id = H5Gopen(group_id, REF_GRP_NAME.c_str());


      // +++++++++++++++++++++++++++++++++++++++++++++++
      // "NumberOfTimeshots" dataset
      dset_timeshots  = H5Dopen(group_id, NTIMESHOTS_NAME.c_str());

      // +++++++++++++++++++++++++++++++++++++++++++++++
      // "Time" dataset
      dset_time  = H5Dopen(group_id, TIMEARRAY_NAME.c_str());
      // retrieve dataspace size
      dataspace = H5Dget_space (dset_time); 
      H5Sget_simple_extent_dims(dataspace, dims, maxdims);
      H5Sclose(dataspace);
      // change dataspace
      dims[0]++;
      status = H5Dextend(dset_time, dims);
      dataspace = H5Dget_space (dset_time); 

      // +++++++++++++++++++++++++++++++++++++++++++++++
      // "Timestep" dataset
      dset_timestep  = H5Dopen(group_id, TIMESTEPARRAY_NAME.c_str());
      status = H5Dextend(dset_timestep, dims);

      // +++++++++++++++++++++++++++++++++++++++++++++++
      // open datasets with references to physical quantities
      for ( iter = _QuantityList.begin(), i=0; 
	    iter != _QuantityList.end(); 
	    ++iter, ++i )
	{	
	  dataset_ref[i] = H5Dopen(ref_group_id, iter->c_str());
	  status = H5Dextend(dataset_ref[i], dims);
	}
    }


  // ---------------------------------------------------
  // Write information to the timetable
  // ---------------------------------------------------

  // select hyperslab -- the newly added space at the end of the array
  hsize_t offset[1] = {_Counter-1};
  hsize_t count[1]  = {1};
  status = H5Sselect_hyperslab (dataspace, H5S_SELECT_SET, 
				offset, NULL, count, NULL);  
  // Define memory space
  hsize_t mem_dims[1] = {1};
  hid_t   mem_space = H5Screate_simple (1, mem_dims, NULL); 



  // ++++++++++++++++++++++++++++++++++++++++++++++++++
  // Write timeshot number to the "NumberOfTimeshots" dataset 
  status = H5Dwrite( dset_timeshots, H5T_NATIVE_INT, 
		     H5S_ALL, H5S_ALL, H5P_DEFAULT, 
		     &_Counter );
  H5Dclose(dset_timeshots);


  // ++++++++++++++++++++++++++++++++++++++++++++++++++
  // Write current time to "Time" dataset 
  status = H5Dwrite( dset_time, H5T_NATIVE_DOUBLE, 
		     mem_space, dataspace, H5P_DEFAULT, 
		     &_T );
  H5Dclose(dset_time);


  // ++++++++++++++++++++++++++++++++++++++++++++++++++
  // Write current global timestep#  to "Timestep" dataset 
  status = H5Dwrite( dset_timestep, H5T_NATIVE_INT, 
		     mem_space, dataspace, H5P_DEFAULT, 
		     &_Timestep );
  H5Dclose(dset_timestep);
  

  // ++++++++++++++++++++++++++++++++++++++++++++++++++
  // write references to the datasets with physical quantities
  for ( iter = _QuantityList.begin(), i=0; 
	iter != _QuantityList.end(); 
	++iter, ++i )
    {
      string obj_name = *iter + "/" + this->GetDatasetId();

      // Create reference to dataset
      status = H5Rcreate(wbuf, file_id, obj_name.c_str(), H5R_OBJECT,-1);

      // if nu such group exist (e.g. array empty) set ref to -1
      if ( status<0 ) wbuf[0] = -1;

      // Write selection to disk 
      status = H5Dwrite( dataset_ref[i], H5T_STD_REF_OBJ,
			 mem_space, dataspace, H5P_DEFAULT, 
			 wbuf);
      H5Dclose(dataset_ref[i]);
    }



  // ---------------------------------------------------
  // Close everything
  // ---------------------------------------------------
  
  H5Sclose(dataspace);
  H5Sclose(mem_space);
  H5Gclose(ref_group_id);
  H5Gclose(group_id);
}



int Timetable::GetNumbeOfTimeshots(Save2HDF &hdf) const
{
  using namespace Timetable_namespace;

  int   timeshots;

  hid_t  file_id, group_id;
  hid_t  dset_timeshots;
  herr_t status;  

  file_id = hdf.GetFileId();
  
  group_id       = H5Gopen(file_id,  TIMETABLE_GRP_NAME.c_str() );
  dset_timeshots = H5Dopen(group_id, NTIMESHOTS_NAME.c_str() );
  
  status = H5Dread(dset_timeshots, H5T_NATIVE_INT, H5S_ALL,H5S_ALL,H5P_DEFAULT, &timeshots);
  
  if ( status<0 )
    {
      cout<<"Timetable::GetNumbeOfTimeshots : Problems to read Timeshots number!\n";
      exit(1);
    }

  H5Dclose(dset_timeshots);
  H5Gclose(group_id);

  return timeshots;
}


int Timetable::VerifiedTimeshotNumber(int timeshot, Save2HDF &hdf) const
{
  // id of the start dataset
  int n_timeshots_max = GetNumbeOfTimeshots(hdf);

  if ( timeshot < 0 ) 
    {
      timeshot = n_timeshots_max;
    }
  else if ( timeshot >  n_timeshots_max )
    {
      std::cout<<"The requested timeshot# "<<timeshot;
      std::cout<<" is larger than the total number of timeshots\n"<<std::flush;
      exit(1);      
    }

  return timeshot;
}


/**
 * Timeshot number start with 1!
 *
 * if i_timeshot < 0 it gives the time of the last timeshot
 * 
 */
double Timetable::GetTimeOfTheTimeshot(Save2HDF &hdf, int i_timeshot) const
{
  using namespace Timetable_namespace;

  double time;

  hid_t  file_id, group_id;
  hid_t  dataset_id;
  hid_t  dataspace_id;
  herr_t status;  

  hsize_t dims[1];

  file_id      = hdf.GetFileId();
  group_id     = H5Gopen(file_id,  TIMETABLE_GRP_NAME.c_str());
  dataset_id   = H5Dopen(group_id, TIMEARRAY_NAME.c_str());
  dataspace_id = H5Dget_space(dataset_id);

  H5Sget_simple_extent_dims(dataspace_id, dims, NULL);
  if ( i_timeshot < 0 )
    {
      i_timeshot = dims[0];
    }
  if ( dims[0] < i_timeshot )
    {
      std::cout<<"Timetable::GetTimeOfTheTimeshot : ";
      std::cout<<" in file \""<<hdf.GetFilename();
      std::cout<<" i_timeshot="<<i_timeshot<<" is too large!\n";
      exit(1);
    }
  double* buf = new double[ dims[0] ]; 

  status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL,H5S_ALL,H5P_DEFAULT, buf);  

  H5Sclose(dataspace_id);
  H5Dclose(dataset_id);
  H5Gclose(group_id);

  // get time
  time = buf[i_timeshot-1];
  delete[] buf;

  return time;
}

//! time at which timeshot was made
int Timetable::GetTimestepOfTheTimeshot(Save2HDF &hdf, int i_timeshot) const
{
  using namespace Timetable_namespace;

  int timestep;

  hid_t  file_id, group_id;
  hid_t  dataset_id;
  hid_t  dataspace_id;
  herr_t status;  

  hsize_t dims[1];

  file_id      = hdf.GetFileId();
  group_id     = H5Gopen(file_id,  TIMETABLE_GRP_NAME.c_str());
  dataset_id   = H5Dopen(group_id, TIMESTEPARRAY_NAME.c_str());
  dataspace_id = H5Dget_space(dataset_id);

  H5Sget_simple_extent_dims(dataspace_id, dims, NULL);
  if ( i_timeshot < 0 )
    {
      i_timeshot = dims[0];
    }
  if ( dims[0] < i_timeshot )
    {
      std::cout<<"Timetable::GetTimestepOfTheTimeshot : ";
      std::cout<<" in file \""<<hdf.GetFilename();
      std::cout<<" i_timeshot="<<i_timeshot<<" is too large!\n";
      exit(1);
    }
  int* buf = new int[ dims[0] ]; 

  status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL,H5S_ALL,H5P_DEFAULT, buf);  

  H5Sclose(dataspace_id);
  H5Dclose(dataset_id);
  H5Gclose(group_id);

  // get time
  timestep = buf[i_timeshot-1];
  delete[] buf;

  return timestep;
}



//! get array of timeshots
std::vector<double> Timetable::GetTimeArray(Save2HDF &hdf) const
{
  using namespace Timetable_namespace;

  double time;

  hid_t  file_id, group_id;
  hid_t  dataset_id;
  hid_t  dataspace_id;
  herr_t status;  

  hsize_t dims[1];

  file_id      = hdf.GetFileId();
  group_id     = H5Gopen(file_id,  TIMETABLE_GRP_NAME.c_str());
  dataset_id   = H5Dopen(group_id, TIMEARRAY_NAME.c_str());
  dataspace_id = H5Dget_space(dataset_id);

  H5Sget_simple_extent_dims(dataspace_id, dims, NULL);

  double* buf = new double[ dims[0] ]; 

  status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL,H5S_ALL,H5P_DEFAULT, buf);  

  H5Sclose(dataspace_id);
  H5Dclose(dataset_id);
  H5Gclose(group_id);

  // get time (element following the last one in array)
  std::vector<double> time_array(buf,&buf[dims[0]]);
  delete[] buf;

  return time_array;
}


//! get array of timeshots
std::vector<int> Timetable::GetTimestepArray(Save2HDF &hdf) const
{
  using namespace Timetable_namespace;

  double time;

  hid_t  file_id, group_id;
  hid_t  dataset_id;
  hid_t  dataspace_id;
  herr_t status;  

  hsize_t dims[1];

  file_id      = hdf.GetFileId();
  group_id     = H5Gopen(file_id,  TIMETABLE_GRP_NAME.c_str());
  dataset_id   = H5Dopen(group_id, TIMESTEPARRAY_NAME.c_str());
  dataspace_id = H5Dget_space(dataset_id);

  H5Sget_simple_extent_dims(dataspace_id, dims, NULL);
  int* buf = new int[ dims[0] ]; 

  status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL,H5S_ALL,H5P_DEFAULT, buf);  

  H5Sclose(dataspace_id);
  H5Dclose(dataset_id);
  H5Gclose(group_id);

  // get timesteps
  std::vector<int> timestep_array(buf,&buf[dims[0]]);
  delete[] buf;

  return timestep_array;
}
