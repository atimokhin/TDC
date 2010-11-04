#ifndef MY_PARTICLES_H
#define MY_PARTICLES_H

#include<iostream>

#include "Pooma/Particles.h"
#include "ATbase.h"

#include "../utils/formats.h"
#include "../utils/to_string.h"
#include "../containers/My_AttributeMap.h"
#include "../containers/My_PropertyMap.h"
#include "../containers/My_Map.h"
#include "../inout/timetable.h"
#include "../inout/save2hdf.h"

#include "../_Control/start_control.h"

#include "InitialDistributions/initial_distribution_maker.h"
#include "ID/particle_id.h"

#include "particle_population.h"


/**
 * \defgroup particles_grp Particles
 * 
 * Ingredients and Actions related to Particles (charged particles and and pairs)
 */


/**
 * \ingroup particles_grp
 * 
 * \class My_Particles
 * \brief Base class for all PIC particles
 *
 * Base class for all my PIC particles. Derived from POOMA Particles class.
 * - has members common for all aprticles:
 *  -# Attribute Weight
 *  -# Properties N_MAX, N_OPT and N_BASE
 * - has methods for
 *  -# adding Attributes to the internal lists
 *  -# adding Properties to the internal lists
 *  -# initializing particles from config file
 *  -# saving particles to HDF file
 * - has typedefs for attributes
 *
 * for reading from HDF file following SetupParametrs are requied:
 * - StartControl
 *
 */
template <class PT>
class My_Particles : public Particles<PT>
{
public:
  // Typedefs ---------------------------------------------------------
  typedef My_Particles<PT>                         This_t;
  typedef Particles<PT>                            Base_t;
  typedef typename Base_t::AttributeEngineTag_t    AttributeEngineTag_t;
  typedef typename Base_t::ParticleLayout_t        ParticleLayout_t;
  typedef typename ParticleLayout_t::AxisType_t    AxisType_t;
  typedef typename ParticleLayout_t::PointType_t   PointType_t;

  typedef AxisType_t Scalar_t;
  typedef int        INT_t;   

  typedef DynamicArray<PointType_t,AttributeEngineTag_t>  Attribute_Point_t;
  typedef DynamicArray<Scalar_t,AttributeEngineTag_t>     Attribute_Scalar_t;
  typedef DynamicArray<INT_t,AttributeEngineTag_t>        Attribute_INT_t;
  typedef DynamicArray<char,AttributeEngineTag_t>         Attribute_CHAR_t;

  typedef Patch<typename This_t::Attribute_Point_t>  Patch_Point_t;
  typedef Patch<typename This_t::Attribute_Scalar_t> Patch_Scalar_t;
  typedef Patch<typename This_t::Attribute_INT_t>    Patch_INT_t;
  typedef Patch<typename This_t::Attribute_CHAR_t>   Patch_CHAR_t;
  // Typedefs ---------------------------------------------------------



  //! Constructor: set up layouts, register attributes
  My_Particles(const ParticleLayout_t &pl);


  //@{
  //! Add corresponding attribute to internal list 
  void AddAttribute_Point(string name,  Attribute_Point_t& attr);
  void AddAttribute_Scalar(string name, Attribute_Scalar_t& attr);
  void AddAttribute_INT(string name,    Attribute_INT_t& attr);
  void AddAttribute_CHAR(string name,   Attribute_CHAR_t& attr);
  //!@}


  //! add attribute to the internal map of saved attributes
  template<class T>
  void AddSavedAttribute(string name, T& attr);

  //! add property to the internal map of saved properties
  template<class T>
  void AddSavedProperty(string name, T& prop);

  //! print content 
  virtual void Print(ostream& o = std::cout) const;
  //! print attribute to std::cout
  void PrintAttribute(string attr_name, ostream& o = std::cout) const;

  //! setup particles from config file group
  virtual void SetupFromConfigGroup(FileInput& in, ParticleID& p_id);
  //! setup and read particle attributes from HDF file
  virtual void SetupFromHDFFileAndConfigGroup(std::string Filename,FileInput& in);

  //! saves all 'saved' attributes into HDF file
  void SaveToHDFFile(double time, int it, Save2HDF &hdf);
  //! flush HDF file
  void FlushHDFFile( Save2HDF &hdf );
  //! read particle parameters from HDF file for a given timeshot#
  void ReadFromHDFFile(int timeshot, Save2HDF& hdf);

  //! create n new particles, return interval with numbers of created particles
  virtual Interval<1> Create(int n);

  //! performe swap on particles
  virtual void Swap() = 0;
  //! performe sync on particles
  virtual void Sync() = 0;

public:

  // Properties -----------------------------
  //! Property: Maximum number of  particles
  int N_MAX;
  //! Property: Optimal number of  particles
  int N_OPT;
  //! Property: Physically motivated characteristic number of  particles
  int N_BASE;
  // ----------------------------------------

  // Attributes -----------------------------
  //! Attribute: Particle statistical weight
  Attribute_Scalar_t  Weight;
  //! Attribute: Origin of particle (e.g. primary, CR produced, inflow etc.)
  Attribute_CHAR_t    Origin;

  //! Attribute: Particle timestep ID (timestep# when particle was created)
  Attribute_INT_t  IDTS;
  //! Attribute: Particle ID (patch# and consequtive# of particle created in that patch)
  Attribute_INT_t  ID;
   // ----------------------------------------


public:

  //! Map with Point_t attributes
  My_Map<Attribute_Point_t>   Attributes_Point;  
  //! Map with Scalar_t attributes
  My_Map<Attribute_Scalar_t>  Attributes_Scalar; 
  //! Map with int attributes
  My_Map<Attribute_INT_t>     Attributes_INT;    
  //! Map with char attributes
  My_Map<Attribute_CHAR_t>    Attributes_CHAR;    


private:

  //! prints error message about missing attribute
  void NoSuchAttribute(string attr_name);

private:

   //! map of saved properties
  My_PropertyMap   _SavedProperties;
   //! map of saved attributes
  My_AttributeMap  _SavedAttributes;

  //! Timetable
  Timetable        _TTable;

  //! Particle Statistics class
  ParticlePopulation _PP;
};





template <class PT>
My_Particles<PT>::My_Particles(const ParticleLayout_t &pl): 
  Particles<PT>(pl)
{  
  // add attributes
  AddAttribute_Scalar("Weight", Weight);
  AddAttribute_CHAR(  "Origin", Origin);
  AddAttribute_INT(   "IDTS", IDTS);
  AddAttribute_INT(   "ID", ID);

  AddSavedAttribute(  "Weight", Weight);
  AddSavedAttribute(  "Origin", Origin);
  AddSavedAttribute(  "IDTS", IDTS);
  AddSavedAttribute(  "ID", ID);

  // add properties
  AddSavedProperty("N_MAX",N_MAX);
  AddSavedProperty("N_OPT",N_OPT);
  AddSavedProperty("N_BASE",N_BASE);
}




/** 
 *  addSavedAttribute add attribute to the internal map of attributes
 *  which will be save into HDF file
 *
 * @param name name of the attribute
 * @param attr attribute
 */
template <class PT>
template <class T>
void My_Particles<PT>::AddSavedAttribute(string name, T& attr)
{
  _SavedAttributes.Add(name,attr);
  _TTable.AddQuantityToList(name);
}


/** 
 *  addSavedProperty add attribute to the internal map of attributes
 *  which will be save into HDF file
 *
 * @param name name of the attribute
 * @param prop property 
 */
template<class PT>
template<class T>
void My_Particles<PT>::AddSavedProperty(string name, T& prop)
{
  _SavedProperties.Add(name,prop);
}


/** 
 * SaveToHDFFile saves all saved attributes into HDF file
 * uses DumpAttribute2HDF functor
 *
 * @param time time of the current timeshot
 * @param hdf  object doind HDF dump
 */
template<class PT>
void My_Particles<PT>::SaveToHDFFile(double time, int it, Save2HDF &hdf)
{
  //! update particle statistics
  _PP.Update(*this);

  // save properties fo the file only once, when
  // the save operation is performed for the first time
  if ( _TTable.TimeshotNumber() == 0 )
    _SavedProperties.SaveToHDFFileGroup("PROPERTIES", hdf );  

  _TTable.SetNewTime(time, it);
  _PP.SaveToHDFFile(_TTable.GetDatasetId(), hdf);
  _SavedAttributes.SaveToHDFFile(_TTable.GetDatasetId(), hdf );
  _TTable.UpdateTimetableInHDFFile(hdf);
};


/** 
 * Flush HDF File
 */
template<class PT>
void My_Particles<PT>::FlushHDFFile( Save2HDF &hdf )
{
  hdf.FlushHDFFile();
}


/** 
 * @{
 *
 *  Add attribute of corresponding type
 *  - to the internal map of Point_t attributes
 *  - to the original Particles's list of
 *    attributes
 * 
 * @param name name of the attribute
 * @param attr attribute
 */

template <class PT>
void My_Particles<PT>::AddAttribute_Point(string name, Attribute_Point_t& attr)
{
  Particles<PT>::addAttribute(attr);
  Attributes_Point.Add(name,attr);
}

template <class PT>
void My_Particles<PT>::AddAttribute_Scalar(string name, Attribute_Scalar_t& attr)
{
  Particles<PT>::addAttribute(attr);
  Attributes_Scalar.Add(name,attr);
}

template<class PT>
void My_Particles<PT>::AddAttribute_INT(string name, Attribute_INT_t& attr)
{
  Particles<PT>::addAttribute(attr);
  Attributes_INT.Add(name,attr);
}

template <class PT>
void My_Particles<PT>::AddAttribute_CHAR(string name, Attribute_CHAR_t& attr)
{
  Particles<PT>::addAttribute(attr);
  Attributes_CHAR.Add(name,attr);
}
//@}



/** 
 * Sets up particles class by reading parameters from config file.
 * 
 * This function reads "NumberOfParticles_Initial" and <b> creates requested number of particles </b>
 *
 * Then it sets up:
 * - <B> Limiting particle number densities </B> 
 *    My_Particles<PT>::N_MAX and My_Particles<PT>::N_OPT
 * - if at least one particle was created it sets <B> Weight </B> of all particles to 1, 
 *    but this can be changes by <B> InitialConditions </B> setup 
 * - if at least one particle was created it sets 
 *    <B> Initial distribution </B> of particle attributes.
 *   Particles boundary conditions are set in child classes
 * .
 * <B> InitialConditions </B> setup  works as follows:
 * - goes into the group "InitialConditions"
 * - reads "NumberOfParticles_Maximal" and "NumberOfParticles_Optimum"
 * - reads "NumberOfInitializedAttributes" and iterates over such attributes
 * - for each Initialized Attribute:
 *   - read its name (AttributeName)
 *   - goes into the corresponding group
 *   - if "DependsOnAttribute" is set reads its name and obtain
 *        pointer to this attribute <I> pattr_base </I>
 *   - reads the name of distribution (DistributionName) 
 *   - initialize the attribute with the corresponding distribution,
 *        generated by  InitialDistributionMaker<>, parametrized by
 *        the corresponding attribute type. If necessary - calls
 *        functor <I> p_dist </I> with two arguments the initialized attribute 
 *        and <I> pattr_base </I>
 *   - leaves AttributeName group
 *   .
 * - leaves "InitialConditions" group
 * .
 *
 * config file example:
 \verbatim
     ...
     NumberOfParticles_Initial = 1;
     NumberOfParticles_Maximal = 2000;
     NumberOfParticles_Optimum = 1000;
     Group "InitialConditions" {	 
        NumberOfInitializedAttributes = 2;
        AttributeName_#0 = 'X';
        AttributeName_#1 = 'Momentum';	 
        Group "X" {		
           DistributionName='Discrete';
           Group "Discrete" {		
           ....
           }   
        }
        Group "Momentum" {		
           DependsOnAttribute='X';
           DistributionName='SinX';
           Group "SinX" {		
           ....
           }
        }
     }
 \endverbatim
 * 
 * @param in InputFile object
 */
template <class PT>
void My_Particles<PT>::SetupFromConfigGroup(FileInput& in, ParticleID& p_id)
{
  // requested number of particles ***********************
  int number_of_particles = 
    static_cast<int>(in.get_param("NumberOfParticles_Initial"));


  // ************************************************************
  // Particle number limits *************************************
  // ************************************************************
  N_MAX = static_cast<int>(in.get_param("NumberOfParticles_Maximal"));
  N_OPT = static_cast<int>(in.get_param("NumberOfParticles_Optimum"));

  // Do setup only if at least one particle was created
  if ( number_of_particles>0 ) 
    {
      // create requested number of particles ********************
      Create(number_of_particles);

      // Default value for Weight attribute **********************
      Weight = 1;

      // Set particle id *****************************************
      p_id.SetPatch(0);
      for (int i=0; i<number_of_particles; i++)
	{
	  IDTS(i) = p_id.GetIDTS();
	  ID(i)   = p_id.GetID();
	}



      // *********************************************************
      // Initial conditions **************************************
      // *********************************************************

      in.ChangeGroup("InitialConditions");
      // number of initialized attributes
      int n_attr = static_cast<int>(in.get_param("NumberOfInitializedAttributes"));

      // iterate over attributes only if at least one particle was created
      for (int i_attr=0; i_attr<n_attr; i_attr++)
	{
	  string attr_name = in.get_name("AttributeName",i_attr);
	  in.ChangeGroup(attr_name);

	  string distr_name = in.get_name("DistributionName");

	  // Does the initial distribution of the attribute depends on another one?
	  // ponter to atribute on which another attribute depends
	  Attribute_Point_t* pattr_base;
	  bool               is_independent = true;


	  if ( in.name_is_set("DependsOnAttribute") )
	    {
	      is_independent = false;
	      pattr_base = Attributes_Point.Item(in.get_name("DependsOnAttribute"));

	      if (!pattr_base)
		{
		  NoSuchAttribute(attr_name);
		  exit(1);
		}
	    }

	  if ( Attribute_Point_t* pattr = Attributes_Point.Item(attr_name) )
	    {
	      InitialDistribution<Attribute_Point_t>* p_distr = 
		InitialDistributionMaker<Attribute_Point_t>(in).pMake(distr_name);
	      // independent or dependent?
	      is_independent? (*p_distr)(*pattr) : (*p_distr)(*pattr, *pattr_base);

	      delete p_distr;
	    }
	  else if ( Attribute_Scalar_t* pattr = Attributes_Scalar.Item(attr_name) )
	    {
	      InitialDistribution<Attribute_Scalar_t,Attribute_Point_t>* p_distr = 
		InitialDistributionMaker<Attribute_Scalar_t,Attribute_Point_t>(in).pMake(distr_name);
	      // independent or dependent?
	      is_independent? (*p_distr)(*pattr) : (*p_distr)(*pattr, *pattr_base);

	      delete p_distr;
	    }
	  else if ( Attribute_INT_t* pattr = Attributes_INT.Item(attr_name) )
	    {
	      InitialDistribution<Attribute_INT_t,Attribute_Point_t>* p_distr = 
		InitialDistributionMaker<Attribute_INT_t,Attribute_Point_t>(in).pMake(distr_name);
	      // independent or dependent?
	      is_independent? (*p_distr)(*pattr) : (*p_distr)(*pattr, *pattr_base);

	      delete p_distr;
	    }
	  else
	    {
	      NoSuchAttribute(attr_name);
	    }

	  in.ChangeGroup("..");
	}
      in.ChangeGroup("..");
    }
  
  // swap particles after setup
  this->Swap();
};



template <class PT>
void My_Particles<PT>::SetupFromHDFFileAndConfigGroup(std::string Filename, FileInput& in)
{
  StartControl calc_start;

  // HDF file object
  std::string hdf5_filename = calc_start.InputDirName() + "/" + Filename + ".h5";
  Save2HDF hdf;
  hdf.open( hdf5_filename.c_str() );

  // Read data from HDF file
  ReadFromHDFFile(calc_start.StartFromTimeshot(), hdf);

  // These properties can be overwritten from Config file ----
  N_MAX = static_cast<int>(in.get_param("NumberOfParticles_Maximal"));
  N_OPT = static_cast<int>(in.get_param("NumberOfParticles_Optimum"));
}



/**
 *  Read particles attributes and properties from hdf file.
 *  Particle number will be adjusted if necessary
 */
template <class PT>
void My_Particles<PT>::ReadFromHDFFile(int timeshot, Save2HDF& hdf)
{
  timeshot = _TTable.VerifiedTimeshotNumber(timeshot, hdf);
  std::string start_dataset_id = to_string(timeshot);

  // get particle number
  _PP.ReadFromHDFFile(start_dataset_id, hdf);
  int n_saved   = _PP.GetTotalNumberOfParticles();
  int n_current = this->size();

  // particle number adjustment
  if ( n_current <  n_saved )        // created particles
    {  
      this->create(n_saved-n_current);
    }
  else if (  n_current >  n_saved )  // delete superflucial
    {
      this->destroy(Interval<1>(0,n_current-n_saved-1),-1);
    }
  this->Sync();
  
  // read properties
  _SavedProperties.ReadFromHDFFileGroup("PROPERTIES", hdf );
  // read attributes (only if particles were created)
  if ( n_saved > 0 )  
    _SavedAttributes.ReadFromHDFFile(start_dataset_id, hdf );

  // swap
  this->Swap();
}




  
template <class PT>
void My_Particles<PT>::NoSuchAttribute(string attr_name)
{
  cout<<"\n\n WARNING: no Attribute \""<<attr_name<<"\" ";
  cout<<"is registered in particle class!\n\n"; 
}




/** 
 * Creates n new particles.  This function enhance the standard
 * Particles create method by returning an interval with indexes of
 * the newly created particles, so later their attributes can be
 * initialized
 * 
 * @param n number of particles to be created
 * 
 * @return interval of indexies of the newly created particles
 */
template <class PT>
Interval<1>  My_Particles<PT>::Create(int n)
{
  // index of the last particle before creation of new particles
  int i_last_old = this->size();

  // create particles in the last patch and do renumbering
  this->create(n);

  return Interval<1>(i_last_old,this->size()-1);
}



/** 
 * Print attribute to cout
 * 
 * @param attr_name attribute name
 * @param o output stream
 */
template <class PT>
void My_Particles<PT>::PrintAttribute(string attr_name, ostream& o) const
{
  o<<"Attribute: \""<<attr_name<<"\"  Type :";

  if ( Attribute_Point_t* pattr = Attributes_Point.Item(attr_name) )
    {      
      o<<" Point_t \n"<<*pattr;
    }
  else if ( Attribute_Scalar_t* pattr = Attributes_Scalar.Item(attr_name) )
    {
      o<<" Scalar_t \n"<<*pattr;
    }
  else if ( Attribute_INT_t* pattr = Attributes_INT.Item(attr_name) )
    {
      o<<" INT_t \n"<<*pattr;
    }
  else if ( Attribute_CHAR_t* pattr = Attributes_CHAR.Item(attr_name) )
    {
      o<<" CHAR_t \n"<<*pattr;
    }
  else
    {
      o<<"\n\n WARNING: no Attribute \""<<attr_name<<"\" ";
      o<<"is registered in particle class!\n\n";
    }
}


/** 
 * Print number of particles in the list
 * 
 * @param o output stream
 */
template <class PT>
void My_Particles<PT>::Print(ostream& o) const
{
  switch( fmt::Format::GetFormat() )
    {
    case fmt::SHORT :
      o<<": "<<this->size()<<" ";
      break;

    case fmt::LONG :
      Base_t::print(o);
      break;
    }
  fmt::Format::SetDefaultFormat();
}



template <class PT>
inline std::ostream& operator<< ( std::ostream& s, const My_Particles<PT>& p )
{
  p.Print(s);
  return s;
};


#endif




