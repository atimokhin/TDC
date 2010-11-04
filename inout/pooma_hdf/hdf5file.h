// -*- C++ -*-

#ifndef RHALK_HDF5FILE_H
#define RHALK_HDF5FILE_H

#include <stack>
#include "Pooma/Arrays.h"
#include "Pooma/DynamicArrays.h"
#include "Pooma/Fields.h"
#include "IO/HDF5.h"

// guard all MPI usage by POOMA_MPI && H5_HAVE_PARALLEL
#if POOMA_MPI && defined(H5_HAVE_PARALLEL)
#define HDF5FILE_MPI 1
#endif

class HDF5File {
public:
  HDF5File(bool parallel_io = false, bool debug = false);
  HDF5File(const HDF5File&);            // not implemented, cause link error, if called
  HDF5File& operator=(const HDF5File&); // not implemented, cause link error, if called

  ~HDF5File();

  bool open(const char *name, int io_context = 0);
  bool create(const char *name, bool excl = false, int io_context = 0);


  /// MY ADDITION TO THIS CLASS #########################
  /// Return hdf file id  
  hid_t GetFileId() const { return file(); };
  bool close()
  {
    while (popLoc())
      ;
    if ((io() || pio()) && file_id_m != -1)
      return H5Fclose(file_id_m) == 0;
  }


  int getIOContext() const { return ioContext_m; }

  //@{
  /// location (aka group, dataset) reader/writer operate on

  /// push path, creating, if necessary group(s) that do not exist along the path
  bool pushLoc(const std::string& id, bool creat = true, bool excl = false);

  /// push path which is relative to current path, else like pushLoc()
  bool pushLocRel(const std::string& id, bool creat = true, bool excl = false);

  /// pop from the path stack
  bool popLoc();

  /// peek path, checking if it exists, not pushing if so.
  bool peekLoc(const std::string& id);

  /// peek path which is relative to current path, checking if it exists,
  /// not pushing if so.
  bool peekLocRel(const std::string& id);

  //@}


  //@{
  /// attribute methods

  /// store an attribute at the current location
  template <class T>
  bool setAttribute(const char *id, const T& value);

  /// read an attribute from the current location
  template <class T>
  bool getAttribute(const char *id, T& value);

  //@}


  //@{
  /// dataset (field) methods

  /// store a field at the current location
#if 0
  template <int Dim, class MeshTag, class T, class EngineTag /*, class T2 = T */>
  inline bool writeField(const char *id, const Field<MeshTag, T, EngineTag>& f,
			 const GuardLayers<Dim>& g = GuardLayers<Dim>(0))
#else
  template <class MeshTag, class T, class EngineTag /*, class T2 = T */>
  inline bool writeField(const char *id, const Field<MeshTag, T, EngineTag>& f,
			 const GuardLayers<MeshTag::dimensions>& g = GuardLayers<MeshTag::dimensions>(0))
#endif
  {
    return writeFieldOther<T, MeshTag, T, EngineTag>(id, f, g);
  }

  /// store a field at the current location, but use T2 as on-disk element type
#if 0
  template <int Dim, class T2, class MeshTag, class T, class EngineTag>
  bool writeFieldOther(const char *id, const Field<MeshTag, T, EngineTag>& f,
		       const GuardLayers<Dim>& g = GuardLayers<Dim>(0));
#else
  template <class T2, class MeshTag, class T, class EngineTag>
  bool writeFieldOther(const char *id, const Field<MeshTag, T, EngineTag>& f,
		       const GuardLayers<MeshTag::dimensions>& g = GuardLayers<MeshTag::dimensions>(0));
#endif

  /// read a field from the current location
  template <class MeshTag, class T, class EngineTag, int Dim>
  bool readField(const char *id, const Field<MeshTag, T, EngineTag>& f,
		 GuardLayers<Dim>& g);

  //@}


  //@{
  /// dataset (array) methods

  /// store an array at the current location
  template <int Dim, class T, class EngineTag>
  inline bool writeArray(const char *id, const Array<Dim, T, EngineTag>& a)
  {
    return writeArrayOther<T, Dim, T, EngineTag>(id, a);
  }

  /// store an array at the current location, but use T2 as on-disk element type
  template <class T2, int Dim, class T, class EngineTag>
  inline bool writeArrayOther(const char *id, const Array<Dim, T, EngineTag>& a);

  /// read an array from the current location
  template <int Dim, class T, class EngineTag>
  bool readArray(const char *id, const Array<Dim, T, EngineTag>& f);



  // My addition for Dynamic Array ********************************************
  // **************************************************************************

  /// store an **Dynamic** array at the current location
  template <class T, class EngineTag>
  inline bool writeArray(const char *id, const DynamicArray<T, EngineTag>& a)
  {
    return writeDynamicArrayOther<T, EngineTag>(id, a);
  }

  template <class T,  class EngineTag>
  bool writeDynamicArrayOther(const char *id, const DynamicArray<T, EngineTag>& a);

//   /// read an array from the current location
//   template <int Dim, class T, class EngineTag>
//   bool readArray(const char *id, const DynamicArray<T, EngineTag>& f);
  // **************************************************************************
  // My addition for Dynamic Array ********************************************

  //@}


  //@{
  /// boundary condition (field) methods

  /// store boundary conditions of field at current location
  template <class MeshTag, class T, class EngineTag>
  bool writeBCs(const Field<MeshTag, T, EngineTag>& f);

  /// read boundary conditions from current location and apply to field
  template <class MeshTag, class T, class EngineTag>
  bool readBCs(const Field<MeshTag, T, EngineTag>& f);

  //@}


  //@{
  /// handling spacings

  /// store spacings at the current location
  template <int Dim>
  bool writeSpacings(const Array<1, double, Brick> s[Dim], const Interval<Dim>& I);
  template <int Dim>
  bool writeSpacings(const Array<1, double, Brick> s[Dim]);
  template <int Dim>
  bool writeSpacings(const Vector<Dim>& s, const Interval<Dim>&);
  template <int Dim>
  bool writeSpacings(const Vector<Dim>& s);

  /// read spacings from the current location
  template <int Dim>
  bool readSpacings(Array<1, double, Brick> s[Dim]);
  template <int Dim>
  bool readSpacings(Vector<Dim>& s);

  //@}

protected:

  /// write an engines data to a new dataset,
  /// the guard layers is only a hint what is contained in the actual
  /// engine view - it doesn't affect the written domain.
  template <int Dim, class T, class EngineTag, class T2>
  bool writeEngine(const char *id, const Engine<Dim, T, EngineTag>& e,
		   const GuardLayers<Dim>& g);

  /// reads a dataset into an engine,
  /// the guard layers is only a hint what is contained in the actual
  /// engine view - it doesn't affect the read domain.
  template <int Dim, class T, class EngineTag>
  bool readEngine(const char *id, const Engine<Dim, T, EngineTag>& e,
		  const GuardLayers<Dim>& g);

  /// stores one subfield with metadata into a new dataset
  template <class MeshTag, class T, class EngineTag, class T2, int Dim>
  bool writeSubField(const char *id, const Field<MeshTag, T, EngineTag>& f,
		     const GuardLayers<Dim>& g);

  /// reads from a dataset into a subfield and restore metadata
  template <class MeshTag, class T, class EngineTag, int Dim>
  bool readSubField(const char *id, const Field<MeshTag, T, EngineTag>& f,
		    GuardLayers<Dim>& g);

  /// factored out subroutine to read one dimension of the spacings arrays
  bool readOneSpacing(const char *id, Array<1, double, Brick> &s);

  /// common implementation for pushLoc, pushLocRel, peekLoc and peekLocRel
  bool pushLocLoc(hid_t loc_id, const std::string& id, bool creat, bool excl);
  bool pushLocLoc_(hid_t loc_id, const std::string& id, bool creat, bool excl);
  bool peekLocLoc(hid_t loc_id, const std::string& id);
  bool peekLocLoc_(hid_t loc_id, const std::string& id);


  /// current location hid, stack top
  hid_t loc() const
  {
    PInsist(!loc_empty(), "empty loc stack");
    return loc_id_stack_m.top();
  }

  /// push to the hid stack
  void loc_push(hid_t id)
  {
    PInsist(id >= 0, "pushing invalid hid");
    loc_id_stack_m.push(id);
  }

  /// pop from the hid stack
  void loc_pop()
  {
    PInsist(!loc_empty(), "empty loc stack");
    loc_id_stack_m.pop();
  }

  /// wether the hid stack is empty
  bool loc_empty() const
  {
    return loc_id_stack_m.empty();
  }

  /// file hid accessor
  hid_t file() const { return file_id_m; }

  /// wether the current context does I/O
  bool io() const { return Pooma::context() == ioContext_m; }

  /// wether we do parallel I/O
  bool pio() const { return pio_f_m; }

  /// the I/O context number
  int ioContext() const { return ioContext_m; }

  /// wether to print debug messages
  bool debug() const { return debug_f_m; }


  /// Helper to allow checking for success on all nodes
  bool allOk(bool val) const;


protected:

  std::string _FileName;

private:
  int ioContext_m;
  hid_t file_id_m;
  std::stack<hid_t> loc_id_stack_m;
  bool pio_f_m;
  bool debug_f_m;
};


#include "hdf5file.cpp"

#endif /* defined RHALK_HDF5FILE_H */
