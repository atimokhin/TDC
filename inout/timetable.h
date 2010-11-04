#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <string>
#include <list>
#include <vector>
#include "save2hdf.h"


/**
 * \ingroup inout_grp 
 *
 * \class Timetable
 * \brief Helper class for managing of timetable in HDF file
 * 
 * Timetable is the class responsible for supporting of valid
 * timetable in HDF results files. It has internal counter which
 * increases at each update of the current time (method
 * SetNewTime(t,it)), so an unique name for a time shot is
 * generated after each time update. It has internal list of name of
 * the physical quantities which are saved into the HDF file.
 * 
 * In the HDF file it creates and updates the group "/Timetable"
 * containinf following datasets: 
 *
 *  - /Timetable/Time      <b> array </b> global time of each timeshot 
 *
 *  - /Timetable/Timestep  <b> array </b> global timestep number of each timeshot 
 *
 *  - /Timetable/NumberOfTimeshots <b>scalar</b>  total number of timeshots in the file
 *
 *  - /Timetable/Ref    <b> array </b> group containing datasets 
 *                      with references to datasets with
 *                      each saved physical quantity listed in the internal list
 *
 * It can retrieve the total number of timeshots from HDF file via
 * call of GetNumbeOfTimeshots()
 *
 */
class Timetable
{
public:
  //! Constructor
  Timetable();
  //! Destructor
  ~Timetable();

  //! add name of the saved physical quantity to the list
  void AddQuantityToList( std::string qName ); 

  //! Set global timestep number where claculation  started
  static void SetStartTimestep(int it_start);
  //! Set new time and timestep number
  void SetNewTime(double t, int it);

  //! Number of the current timeshot
  int TimeshotNumber() const { return _Counter; };
  //! get current name
  std::string GetDatasetId() const { return _DatasetName; }

  //! update timetable in the HDF file
  void UpdateTimetableInHDFFile(Save2HDF &hdf);

  //! total number of timeshots in HDF file
  int GetNumbeOfTimeshots(Save2HDF &hdf) const;
  //! get timeshot number 
  int VerifiedTimeshotNumber(int timeshot, Save2HDF &hdf) const;

  //! time at which timeshot was made
  double GetTimeOfTheTimeshot(Save2HDF &hdf, int i_timeshot) const;
  //! (global) timestep number at which timeshot was made
  int    GetTimestepOfTheTimeshot(Save2HDF &hdf, int i_timeshot) const;

  //! get array of timeshot time
  std::vector<double> GetTimeArray(Save2HDF &hdf) const; 
  //! get array of timeshot (global) timestep number
  std::vector<int>    GetTimestepArray(Save2HDF &hdf) const; 

private:

  static int _START_TIMESTEP; //! global timestep number at calculation start
  int        _Timestep;       //! global current timestep number

  int    _Counter; //! internal counter
  double _T;       //! current time

  std::string _DatasetName; //! current name of the dataset

  std::list<std::string> _QuantityList;     //! internal list of saved physical quantities
  int                    _QuantityListSize; //! size of that list
};



namespace Timetable_namespace{

  using namespace std;

  const string TIMETABLE_GRP_NAME = "Timetable";

  const string TIMEARRAY_NAME     = "Time";
  const string TIMESTEPARRAY_NAME = "Timestep";

  const string NTIMESHOTS_NAME    = "NumberOfTimeshots";

  const string REF_GRP_NAME       = "Ref";

}

#endif
