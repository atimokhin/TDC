#include "hdf5file.h"

HDF5File::HDF5File(bool parallel_io, bool debug)
  : ioContext_m(0), file_id_m(-1), pio_f_m(parallel_io), debug_f_m(debug)
{
  if (!debug)
    H5Eset_auto(NULL, NULL);
#if !HDF5FILE_MPI
  pio_f_m = false;
#endif
}

HDF5File::~HDF5File()
{
  if (debug())
    std::cout << Pooma::context() << ": closing file" << std::endl;
  // pop Loc's on all contexts (due to error reporting!)
  while (popLoc())
    ;
  if ((io() || pio()) && file_id_m != -1)
    H5Fclose(file_id_m);
}

bool HDF5File::open(const char *name, int io_context)
{
  //### store filename - My Modification
  _FileName = name;

  ioContext_m = io_context;

  if (!pio()) {
    bool res = true;

    if (!io())
      return RemoteProxy<bool>(res, ioContext()).value();

    file_id_m = H5Fopen(name, H5F_ACC_RDWR, H5P_DEFAULT);
    res = !(file_id_m < 0);

    if (res)
      loc_push(H5Gopen(file_id_m, "/"));

    return RemoteProxy<bool>(res, ioContext()).value();

  } else {
    hid_t access_plist = H5P_DEFAULT;
    bool res = true;

#if HDF5FILE_MPI
    access_plist = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_mpio(access_plist, MPI_COMM_WORLD, MPI_INFO_NULL);
#endif

    file_id_m = H5Fopen(name, H5F_ACC_RDWR, access_plist);
    res = !(file_id_m < 0);
    if (!allOk(res))
      return false;

    hid_t root = H5Gopen(file_id_m, "/");
    if (root < 0 && debug())
      std::cout << Pooma::context() << " cannot open root group" << std::endl;
    if (!allOk(!(root < 0)))
      return false;
    loc_push(root);

    return res;
  }
}

bool HDF5File::create(const char *name, bool excl, int io_context)
{
  ioContext_m = io_context;

  if (!pio()) {
    bool res = true;

    if (!io())
      return RemoteProxy<bool>(res, ioContext()).value();

    file_id_m = H5Fcreate(name,
                          H5F_ACC_TRUNC
                          |(excl ? H5F_ACC_EXCL : 0),
                          H5P_DEFAULT, H5P_DEFAULT);
    res = !(file_id_m < 0);

    if (res)
      loc_push(H5Gopen(file_id_m, "/"));

    return RemoteProxy<bool>(res, ioContext()).value();

  } else {
    hid_t access_plist = H5P_DEFAULT;
    bool res = true;

#if HDF5FILE_MPI
    access_plist = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_mpio(access_plist, MPI_COMM_WORLD, MPI_INFO_NULL);
#endif

    file_id_m = H5Fcreate(name,
                          H5F_ACC_TRUNC
                          |(excl ? H5F_ACC_EXCL : 0),
                          H5P_DEFAULT, access_plist);
    res = !(file_id_m < 0);
    if (!allOk(res))
      return false;

    hid_t root = H5Gopen(file_id_m, "/");
    if (root < 0 && debug())
      std::cout << Pooma::context() << " cannot open root group" << std::endl;
    if (!allOk(!(root < 0)))
      return false;
    loc_push(root);

    return res;
  }
}


bool HDF5File::pushLocLoc_(hid_t loc_id, const std::string& id, bool creat, bool excl)
{
  if (H5Iget_type(loc_id) != H5I_GROUP)
    return false;

  H5G_stat_t stat;
  herr_t oi_err = H5Gget_objinfo(loc_id, id.c_str(), 1, &stat);
  hid_t new_loc_id;

  if (oi_err < 0 && creat) {
    new_loc_id = H5Gcreate(loc_id, id.c_str(), 0);

  } else if (!(oi_err < 0)
             && !(creat && excl)) {
    if (stat.type == H5G_GROUP)
      new_loc_id = H5Gopen(loc_id, id.c_str());
    else if (stat.type == H5G_DATASET)
      new_loc_id = H5Dopen(loc_id, id.c_str());
    else
      return false;

  } else
    return false;

  if (new_loc_id < 0)
    return false;
  loc_push(new_loc_id);
  if (debug())
    std::cout << Pooma::context() << ": top loc is now " << id << " (" << new_loc_id << ")" << std::endl;
  return true;
}

bool HDF5File::pushLocLoc(hid_t loc_id, const std::string& id, bool creat, bool excl)
{
  if (!pio()) {
    bool res = false;
    if (!io())
      return RemoteProxy<bool>(res, ioContext()).value();
    res = pushLocLoc_(loc_id, id, creat, excl);
    return RemoteProxy<bool>(res, ioContext()).value();

  } else {
    if (debug())
      std::cout << Pooma::context() << "Parallel pushLoc of " << id << std::endl;
    return allOk(pushLocLoc_(loc_id, id, creat, excl));
  }
}

bool HDF5File::pushLoc(const std::string& id, bool creat, bool excl)
{
  return pushLocLoc(file_id_m, id, creat, excl);
}

bool HDF5File::pushLocRel(const std::string& id, bool creat, bool excl)
{
  return pushLocLoc((pio() || io()) ? loc() : -1, id, creat, excl);
}


bool HDF5File::peekLocLoc_(hid_t loc_id, const std::string& id)
{
  if (H5Iget_type(loc_id) != H5I_GROUP)
    return false;

  H5G_stat_t stat;
  herr_t oi_err = H5Gget_objinfo(loc_id, id.c_str(), 1, &stat);

  if (!(oi_err < 0)) {
    if (stat.type == H5G_GROUP
        || stat.type == H5G_DATASET)
      return true;
  }

  return false;
}

bool HDF5File::peekLocLoc(hid_t loc_id, const std::string& id)
{
  if (!pio()) {
    bool res = false;
    if (!io())
      return RemoteProxy<bool>(res, ioContext()).value();
    res = peekLocLoc_(loc_id, id);
    return RemoteProxy<bool>(res, ioContext()).value();

  } else {
    if (debug())
      std::cout << Pooma::context() << "Parallel peekLoc of " << id << std::endl;
    return allOk(peekLocLoc_(loc_id, id));
  }
}

bool HDF5File::peekLoc(const std::string& id)
{
  return peekLocLoc(file_id_m, id);
}

bool HDF5File::peekLocRel(const std::string& id)
{
  return peekLocLoc((pio() || io()) ? loc() : -1, id);
}


bool HDF5File::popLoc()
{
  bool res = false;
  if (!(io() || pio()))
    return RemoteProxy<bool>(res, ioContext()).value();
  if (!loc_empty()) {
    hid_t id = loc();
    if (H5Iget_type(id) == H5I_GROUP)
      H5Gclose(id);
    else /* if (H5Iget_type(id) == H5I_DATASET) */
      H5Dclose(id);
    loc_pop();
    res = true;
    if (debug())
      std::cout << Pooma::context() << ": popped loc " << id << ", stack size " << loc_id_stack_m.size() << std::endl;
  }
  if (!pio())
    res = RemoteProxy<bool>(res, ioContext()).value();
  else
    res = allOk(res);
  if (!res && debug())
    std::cout << Pooma::context() << ": unable to popLoc" << std::endl;
  return res;
}


bool HDF5File::allOk(bool myval) const
{
  PAssert(pio());
  bool val = myval;
  ReduceOverContexts<bool, OpAnd> reduction(val, 0, true);
  reduction.broadcast(val);
  if (myval != val && debug())
    std::cout << Pooma::context() << ": I'm ok while others are not" << std::endl;
  return val;
}


bool HDF5File::readOneSpacing(const char *id, Array<1, double, Brick> &s)
{
  bool res = false;

  /* We cannot use getAttribute here because RemoteProxy<> cannot deal
     with copying Engines properly.  */
  Array<1, double, Remote<Brick> > a;
  a.engine() = Engine<1, double, Remote<Brick> >(ioContext(), s.domain());
  if (io()) {
    hid_t attr_id = H5Aopen_name(loc(), id);
    if (!(attr_id < 0)) {
      HDF5Object<Engine<1, double, Brick> > o(a.engine().localEngine(), attr_id);
      hid_t type_id = o.type();
      res = !(H5Aread(attr_id, type_id, o.write()) < 0);
      if (!res && debug())
        std::cerr << Pooma::context() << ": cannot read attribute " << id << std::endl;
      H5Tclose(type_id);
      H5Aclose(attr_id);
    }
  }
  s = a;
  if (!RemoteProxy<bool>(res, ioContext()).value())
    return false;
}
