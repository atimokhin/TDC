#ifndef QUANTITIES_VS_TIME_H
#define QUANTITIES_VS_TIME_H

#include <deque>
#include <string>
#include <iostream>

#include "ATbase.h"

#include "scalar_deque_inout_ops.h"


/**
 * \defgroup solution_properties_grp Solution Properties
 *
 * Physical properties of the system, obtained in course of
 * calculation as functions of time
 */


/**
 * \ingroup solution_properties_grp
 * \class QuantitiesVsTime
 * 
 * \brief Base class for storing/write/read quantities as a fuction of time
 *
 * - each quantity (in a child class) is stored as a deque
 * - each quantity correspond to a certain moment of time stored in TimeArray.
 * - before saving to HDF file current time must be set.
 *
 * Properties:
 * - each deque can grow but cannot exceed _Size
 * - permamently stored in file are quantities with Tlast<=T<=Tcurrent
 * - although data from deques with Tcurrent<T are written to datasets,
 *     they will be overwritten later during the next saving operation
 * - Tcurrent becomest the next Tlast after all writings to datasets is completed
 * - Tcurrent must always be in the deque, otherwise program exits ( SetCurrentTime(t) );
 * - IStart is the index of the first deque entry with T > Tcurrent
 * - ICurrent is the index of the deque entry with T = Tcurrent
 * - IStartDataset is the index of the dataset array where the [IStart] element of 
 *     the deque will be written at this save step
 *
 \verbatim
 Before saving

                        Tcurrent      (time)
                IStart  ICurrent      (index)
                #       #
         | | | |X|X|X|X|X|x|x|        <deque>

   | | | | | |L|X|X|X|X!X!x!x!        <HDF dataset>
                #
              ^ IStartDataset         (time)
              Tlast                   (index)


 After saving

   | | | | | | | | | | |L| | |        <HDF dataset>
                          #
                        ^ IStartDataset         (time)
                        Tlast                   (index)


  X values stored permamnetly
  x values to be overwritten

  ! cells added during save oprtaion
  | already existing cells

 \endverbatim
 *
 * Usage notes:
 * - before saving to HDF file current time must be set with SetCurrentTime(t)
 * - SetupIStart() must be called before SaveToHDFFile() (we need it only for save operation)
 *
 *
 * Config file example:
 *
 \verbatim
     Group "NS_PARTICLE_FLUX" {
        ! results will be saved in this file   
        OutputFilename = 'ns_particle_flux.h5';
        ! size of the deque array
        Size = 25;
     }
 \endverbatim
 *
 * In HDF files it stores:
 * /TIMEARRAY__DATASET_NAME - TimeArray
 * /T_CURRENT__DATASET_NAME - Tcurrent
 * /I_START_DATASET__DATASET_NAME - IStartDataset
 * 
 *
 * following SetupParametrs are requied:
 * - OutputControl
 * - StartControl
 *
 * by default time is always non-negative,
 * so initially Tlast is set to -1
 */
class QuantitiesVsTime
{
public:
  //! Constructor
  QuantitiesVsTime(int chunksize = 10);
  //! Destructor
  virtual ~QuantitiesVsTime();

  //! setup manually
  virtual void Setup(std::string filename, int size);
  //! setup from config file group
  virtual void SetupFromConfigGroup(FileInput& in);
  //! setup from config file group and read data from pervious calculations
  virtual void SetupFromHDFFileAndConfigGroup(FileInput& in);

  //! Setup output file
  void SetupOutputFile();
  void SetupOutputFile(string output_dir_name);

  //! Save each element into HDF file 
  virtual bool SaveToHDFFile();

  //! Flush HDF file
  void FlushHDFFile();

  //! add value to a deque
  template<class T>
  void AddNewValue(std::deque<T>& d, const T& value);

  //! set the last saved time (useful for initialization)
  void SetTlast(double t);

  //! set current time and setup ICurrent
  void SetCurrentTime(double t);
  //! current time
  double GetCurrentTime() const;

  //! get size of the deque
  int GetSize() const;
  //! get index of the current time entry
  int GetICurrent() const;

  //! print content 
  virtual void Print(ostream& o = std::cout) const;

public:

  std::deque<double> TimeArray;

protected:

  void SetupIStart();

  int GetIStart() const;
  int GetIStartDataset() const;
  int GetNWrite() const;

  bool IsHDFFileInitialized() const;

  void OpenAndDoSetupFromHDFFile(Save2HDF& input_hdf);

protected:

  std::string _Filename; //<! name of the output HDF file

  Save2HDF            _HDF;        //!< HDF file object
  ScalarDequeInoutOps _DequeInout; //!< help class to save scalar deque

private:


  int _Size;

  int _IStart;
  int _ICurrent;
  int _IStartDataset;

  double _Tcurrent;
  double _Tlast;

  bool   _IsInitialized;
};


/**
 * \ingroup qvt_group
 *
 * names for HDF datasets associated with QuantitiesVsTime
 * 
 */
namespace QuantitiesVsTime_names{

  using namespace std;

  const string TIMEARRAY__DATASET_NAME = "TimeArray";
  const string T_CURRENT__DATASET_NAME = "T_current";
  const string I_START_DATASET__DATASET_NAME = "i_start_dataset";
};



/** 
 * Adds new value to a dueque. Controls deque size (does now allow
 * the deque to grow beyound size _Size
 * 
 * @param d     the deque to be modified
 * @param value value inserted
 */
template<class T>
inline void QuantitiesVsTime::AddNewValue(std::deque<T>& d, const T& value)
{
  d.push_back(value);
  if ( d.size() > _Size ) d.pop_front();
}



/**
 * Set the last saved time. May be useful for initialization, by
 * default Tlast ios initially set to -1
 * 
 */
inline void QuantitiesVsTime:: SetTlast(double t)
{
  _Tlast = t;
}


inline double QuantitiesVsTime::GetCurrentTime() const
{
  return _Tcurrent;
}

inline int QuantitiesVsTime::GetSize() const
{
  return _Size;
}

inline int QuantitiesVsTime::GetICurrent() const
{ 
  return _ICurrent; 
};


inline int QuantitiesVsTime::GetIStart() const
{ 
  return _IStart; 
};
inline int QuantitiesVsTime::GetIStartDataset() const
{ 
  return _IStartDataset; 
};

inline int QuantitiesVsTime::GetNWrite() const
{ 
  return TimeArray.size() - GetIStart(); 
};

inline bool QuantitiesVsTime::IsHDFFileInitialized() const 
{ 
  return _IsInitialized; 
}



/**
 * \ingroup qvt_group
 * iostream interface for QuantitiesVsTime
 * 
 */
inline std::ostream& operator<< ( std::ostream& s, const QuantitiesVsTime& qt )
{
  qt.Print(s);
  return s;
};

#endif
