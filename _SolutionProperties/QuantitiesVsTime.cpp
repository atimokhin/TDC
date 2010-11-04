#include "QuantitiesVsTime.h"

#include "../utils/stl_utils.h"

#include "../_Control/output_control.h"
#include "../_Control/start_control.h"


/** 
 * Constructor
 * 
 * _Tlast is set to -1; by default time is always positive
 *
 * @param chunksize chunksize od the datadasets in HDF file 
 */
QuantitiesVsTime::QuantitiesVsTime(int chunksize): 
  _IsInitialized(false),
  _DequeInout( ScalarDequeInoutOps(chunksize) )
{
  _IStart = 0;
  _ICurrent = 0;
  _IStartDataset = 0;

  _Tcurrent = 0;
  _Tlast = -1;
};


//! destructor - do nothing
QuantitiesVsTime::~QuantitiesVsTime() {};


/**
 * Sets _Tcurrent to t and sets up _ICurrent. 
 * Ensures that Tcurrent is in the deque, if not aborts calculations.
 * 
 * @param t time to be set as current time
 */
void QuantitiesVsTime::SetCurrentTime(double t)
{
  // set _Tcurrent
  if ( TimeArray[_ICurrent] == t )
    {
      _Tcurrent = t;
    }
  else 
    {
      int start_deque_iteration = 0;
      if ( TimeArray[_ICurrent] < t ) start_deque_iteration = _ICurrent;
      
      for (int i=start_deque_iteration; i<TimeArray.size() && TimeArray[i] <= t; i++) 
	_ICurrent = i;

      if (TimeArray[_ICurrent] == t)
	_Tcurrent = t;
      else
	{
	  std::cout<<" QuantitiesVsTime::SetCurrentTime: wrong current time=";
	  std::cout<<t<<" != "<<TimeArray[_ICurrent]<<"\n!";
	  exit(1);
	}
    }    
}



/**
 * Calculates IStart, index of the first element in TimeArray greater
 * than Tlast
 * 
 * <b> must be called before performing all saving operations </b>
 */
void QuantitiesVsTime::SetupIStart()
{
  // claculate _IStart only if it is necessary
  if ( _IStart==0 
       || 
       !( TimeArray[_IStart-1]<=_Tlast && TimeArray[_IStart]>_Tlast )  
       )
    {
      _IStart = 0;
      for ( int i=0; TimeArray[i]<=_Tlast && i<TimeArray.size(); i++ ) _IStart++;
    }
}


/** 
 * Setup manually: filename and size of the deque 
 * 
 * @param in FileInput object associated with config file
 */
void QuantitiesVsTime::Setup(std::string filename, int size)
{
  _Filename = filename;
  _Size = size;
}


/** 
 * Setup from config file group. 
 * 
 * @param in FileInput object associated with config file
 */
void QuantitiesVsTime::SetupFromConfigGroup(FileInput& in)
{
  _Filename = in.get_name("OutputFilename");
  _Size = static_cast<int>(in.get_param("Size"));
}


/**
 *  This methos is realodaed in all child classes
 * 
 * it must:
 * - setup from config file group
 * - define input_hdf as a local variable and read data 
 *    from HDF file associated with input_hdf
 */
void QuantitiesVsTime::SetupFromHDFFileAndConfigGroup(FileInput& in)
{
  // read data from config file
  SetupFromConfigGroup(in);

  // read data from HDF file
  Save2HDF input_hdf;
  OpenAndDoSetupFromHDFFile(input_hdf);
}


/** 
 * Setup from config file group. create hdf file for fields output
 * 
 * @param in FileInput object associated with config file
 */
void QuantitiesVsTime::SetupOutputFile()
{
  OutputControl out;
  string full_hdf_filename = out.OutputDirName()+_Filename;
  _HDF.create(full_hdf_filename.c_str());
}

/** 
 * Setup from config file group. create hdf file for fields output
 * 
 * @param in FileInput object associated with config file
 */
void QuantitiesVsTime::SetupOutputFile(string output_dir_name)
{
  string full_hdf_filename = output_dir_name+_Filename;
  _HDF.create(full_hdf_filename.c_str());
}



/**
 * - Saves TimeArray, Tcurrent and IStartDataset to HDF file
 * - updates Tcurrent, IStartDataset 
 *
 * This method is called <b> after </b> all save operations in the
 * child class
 *
 */
bool QuantitiesVsTime::SaveToHDFFile()
{
  using namespace QuantitiesVsTime_names; 

  // setup IStart if it was not devined previously
  SetupIStart();

  hid_t file_id;
  hid_t dataspace;
  hid_t dataset_index, dataset_time;
  herr_t status;  

  // save TimeArray **********************************
  _DequeInout.Save2HDFFile(TIMEARRAY__DATASET_NAME, 
			   _IStart, _IStartDataset, GetNWrite(),
			   TimeArray, 
			   IsHDFFileInitialized(),
			   _HDF);


  // save _IStartDataset  and _Tcurrent **************
  file_id = _HDF.GetFileId();

  if ( IsHDFFileInitialized() )
    {
      dataset_index = H5Dopen( file_id, I_START_DATASET__DATASET_NAME.c_str() );
      dataset_time  = H5Dopen( file_id, T_CURRENT__DATASET_NAME.c_str() );
    }
  else // create all datasets in the file
    {
      hsize_t dims[1] = {1};
      dataspace = H5Screate_simple (1, dims, dims); 

      dataset_index = H5Dcreate( file_id, I_START_DATASET__DATASET_NAME.c_str(), 
				 H5T_NATIVE_INT, dataspace,  H5P_DEFAULT ); 
      dataset_time  = H5Dcreate( file_id, T_CURRENT__DATASET_NAME.c_str(), 
				 H5T_NATIVE_DOUBLE, dataspace,  H5P_DEFAULT ); 
      H5Sclose(dataspace);

      _IsInitialized=true;
    }

  // update _IStartDataset
  _IStartDataset += _ICurrent - _IStart + 1;
  // Save _IStartDataset to HDF file
  status = H5Dwrite(dataset_index, H5T_NATIVE_INT, 
		    H5S_ALL, H5S_ALL, H5P_DEFAULT, 
		    &_IStartDataset );
  H5Dclose(dataset_index);

  // Save _Tcurrent to HDF file
  status = H5Dwrite(dataset_time, H5T_NATIVE_DOUBLE, 
		    H5S_ALL, H5S_ALL, H5P_DEFAULT, 
		    &_Tcurrent );
  H5Dclose(dataset_time);  
  // set Tlast
  _Tlast = _Tcurrent;

  return true;
};


/** 
 * Flush HDF file
 */
void QuantitiesVsTime::FlushHDFFile()
{
  _HDF.FlushHDFFile(); 
}



/**
 * Open inpuf HDF file and read all data necessary for QuantitiesVsTime class
 * 
 * - input_hdf must be defined in child class 
 *     SetupFromHDFFileAndConfigGroup(FileInput& in) fucntion
 * - must be called before reading child class data from HDF file
 */
void QuantitiesVsTime::OpenAndDoSetupFromHDFFile(Save2HDF& input_hdf)
{
  // open input HDF file
  StartControl calc_start;
  string full_input_hdf_filename = calc_start.InputDirName() + "/" + _Filename;
  // open input HDF file
  if ( !input_hdf.open(full_input_hdf_filename.c_str()) )
    {
      cout<<"cannot open file \""<<full_input_hdf_filename<<"\"\n";
    }

  // read data from HDF file
  using namespace QuantitiesVsTime_names; 

  hid_t file_id;
  hid_t dataspace;
  hid_t dataset_index, dataset_time;
  herr_t status;  

  file_id = input_hdf.GetFileId();

  dataset_index = H5Dopen( file_id, I_START_DATASET__DATASET_NAME.c_str() );
  dataset_time  = H5Dopen( file_id, T_CURRENT__DATASET_NAME.c_str() );


  // Read CurrentTime from HDF file
  status = H5Dread(dataset_time, H5T_NATIVE_DOUBLE, 
		   H5S_ALL, H5S_ALL, H5P_DEFAULT, 
		   &_Tcurrent );
  H5Dclose(dataset_time);

  _DequeInout.ReadFromHDFFile(TIMEARRAY__DATASET_NAME, 
			      _Size,
			      TimeArray, 
			      input_hdf);

  // setup Tlast
  _Tlast = _Tcurrent;

  //setup ICurrent
  _ICurrent=0;
  for ( int i=0; i<TimeArray.size() && TimeArray[i] < _Tcurrent ; i++) _ICurrent++;     
}



/**
 * print content 
 * 
 */
void QuantitiesVsTime::Print(ostream& o) const
{
  o<<"\nOutput filename: "<<_Filename<<"\n";

  o<<"Size     = "<<_Size<<"\n";

  o<<"Tcurrent = "<<_Tcurrent<<"\n";
  o<<"Tlast    = "<<_Tlast<<"\n";

  o<<"IStart        = "<<_IStart<<"\n";
  o<<"ICurrent      = "<<_ICurrent<<"\n";
  o<<"IStartDataset = "<<_IStartDataset<<"\n";

  o<<"TimeArray = ";
  PrintSTLContainer(o,TimeArray);
  o<<"\n";
}
