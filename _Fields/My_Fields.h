#ifndef MY_FIELDS_H
#define MY_FIELDS_H


#include "Pooma/Fields.h"

#include "../containers/My_FieldMap.h"
#include "../containers/My_PropertyMap.h"
#include "../inout/timetable.h"
#include "../inout/save2hdf.h"
#include "../utils/io_filenames.h"
#include "../utils/to_string.h"

#include "../_Control/output_control.h"


/**
 * \defgroup fields_grp Fields
 */


/**
 * \ingroup fields_grp
 *
 * \class My_Fields 
 * \brief Base class for fields
 * 
 * Base class for fields. Contains and mantains collections of
 * fields. Setups fields from config and saves them into HDF file
 *
 * following SetupParametrs are requied:
 * - OutputControl
 * - StartControl
 */
template<class Field_t>
class My_Fields
{
public:
  // Typedefs --------------------------------------------
  typedef Field_t         Type_t;
  typedef Patch<Field_t>  Patch_Field_t;

  typedef typename Field_t::Mesh_t              Mesh_t;
  typedef typename Field_t::Engine_t::Layout_t  FLayout_t;

  typedef Field< NoMesh<1>, 
		 typename Field_t::Element_t, 
		 typename Field_t::EngineTag_t  >  FieldAux_t;
  typedef Patch<FieldAux_t>  Patch_FieldAux_t;
  // -----------------------------------------------------

public:

  // Constructor (empty)
  My_Fields() {};
  // Destructor (empty)
  virtual ~My_Fields() {};

  //! Initialization: requires layout, mesh
  virtual void Initialize(const FLayout_t &layout, const Mesh_t &mesh ) = 0;

  //! Setup fields from config file - empty function
  virtual void SetupFromConfigGroup(FileInput& in) {};
  //! Read fields from HDF file
  virtual void SetupFromHDFFilesAndConfigGroup(FileInput& in);

  //!  add attribute to be saved to HDF file into internal list
  template<class T>
  void AddSavedField(string name, T& attr);
  //! add property to the internal map of saved properties
  template<class T>
  void AddSavedProperty(string name, T& prop);

  //! setup output hdf file
  virtual void SetupOutputFile(std::string filename);
  //! saves all saved attributes into HDF file
  void SaveToHDFFile(double time, int it);
  //! flush HDF file
  void FlushHDFFile();
  //! read from HDF file
  void ReadFromHDFFile(int timeshot, Save2HDF& hdf);


private:

  My_FieldMap     _SavedFields;     //!< List of saved fields   
  My_PropertyMap  _SavedProperties; //!< List of saved properties

  Timetable    _TTable;        //!< TimeTable object
  Save2HDF     _HDF;           //!< HDF file object
};




/**
 * Reads fields from HDF filed according config file settings
 * 
 */
template<class Field_t>  
void My_Fields<Field_t>::SetupFromHDFFilesAndConfigGroup(FileInput& in)
{
  // caslculation start parameters (needs for input HDF file name)
  StartControl calc_start;
  // HDF file object: tied to input filename
  std::string hdf5_filename = calc_start.InputDirName() + InOut::FieldsFile;
  Save2HDF hdf;
  hdf.open( hdf5_filename.c_str() );

  // read field value from HDF file ------
  ReadFromHDFFile(calc_start.StartFromTimeshot(), hdf);
}



/** 
 *  addSavedAttribute add attribute to the internal list of
 *  attributes which will be save into HDF file
 * 
 * @param name name of the attribute
 * @param attr attribute
 */
template<class Field_t>  
template<class T>
void My_Fields<Field_t>::AddSavedField(string name, T& attr)
{
  _SavedFields.Add(name,attr);
  _TTable.AddQuantityToList(name);
}

/** 
 *  addSavedProperty add attribute to the internal map of attributes
 *  which will be save into HDF file
 *
 * @param name name of the attribute
 * @param prop property 
 */
template<class Field_t>
template<class T>
void My_Fields<Field_t>::AddSavedProperty(string name, T& prop)
{
  _SavedProperties.Add(name,prop);
}


/**
 * Setup and open HDF file where fields will be saved
 * 
 */
template<class Field_t>  
void My_Fields<Field_t>::SetupOutputFile(std::string filename)
{
  OutputControl out;
  string hdf_filename = out.OutputDirName()+filename;

  _HDF.create(hdf_filename.c_str());
}


/** 
 * SaveToHDFFile saves all fields marked as to be saved  into HDF file
 *
 * @param time time of the current timeshot
 */
template<class Field_t>  
void My_Fields<Field_t>::SaveToHDFFile(double time, int it)
{
  _TTable.SetNewTime(time, it);

  // save fields
  _SavedFields.SaveToHDFFile(_TTable.GetDatasetId(), _HDF );

  //save field point positions and properties only ones
  // the save operation is performed for the first time
  if ( _TTable.TimeshotNumber() == 1 )
    {
      _SavedFields.SaveComplimentaryInfoToHDFFile(_HDF );
      _SavedProperties.SaveToHDFFileGroup("PROPERTIES", _HDF );  
    }
 
  _TTable.UpdateTimetableInHDFFile(_HDF);
};


/** 
 * Flush HDF file
 * 
 */
template<class Field_t>  
void My_Fields<Field_t>::FlushHDFFile()
{
  _HDF.FlushHDFFile();
}


template<class Field_t>  
void My_Fields<Field_t>::ReadFromHDFFile(int timeshot, Save2HDF& hdf)
{
  // id of the start dataset
  timeshot = _TTable.VerifiedTimeshotNumber(timeshot, hdf);
  std::string start_dataset_id = to_string(timeshot);

  // read properties
  _SavedProperties.ReadFromHDFFileGroup("PROPERTIES", hdf );
  // read attributes 
  _SavedFields.ReadFromHDFFile(start_dataset_id, hdf );
}

#endif
