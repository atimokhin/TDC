
template <class T>
bool HDF5File::setAttribute(const char *id, const T& value)
{
  bool res = false;

  if (!(io() || pio()))
    return RemoteProxy<bool>(res, ioContext()).value();

  HDF5Object<T> o(value);
  hid_t type_id = o.type();
  hid_t space_id = o.dataspace();
  H5Adelete(loc(), id);
  hid_t attr_id = H5Acreate(loc(), id, type_id, space_id, H5P_DEFAULT);
  if (!(attr_id < 0)) {
    res = !(H5Awrite(attr_id, type_id, o.read()) < 0);
    H5Aclose(attr_id);
  }
  H5Sclose(space_id);
  H5Tclose(type_id);

  if (!pio())
    return RemoteProxy<bool>(res, ioContext()).value();
  else
    return allOk(res);
}

template <class T>
std::ostream &operator<<(std::ostream &o, const std::vector<T>&)
{
  return o;
}

/// Reads the value of the specified HDF5 attribute.

template <class T>
bool HDF5File::getAttribute(const char *id, T& value)
{
  bool res = false;
  T val(value);

  if (!pio()) {
    if (io()) {
      hid_t attr_id = H5Aopen_name(loc(), id);
      if (!(attr_id < 0)) {
	HDF5Object<T> o(val, attr_id);
	hid_t type_id = o.type();
	res = !(H5Aread(attr_id, type_id, o.write()) < 0);
	if (!res && debug())
	  std::cerr << Pooma::context() << ": cannot read attribute " << id << std::endl;
	H5Tclose(type_id);
	H5Aclose(attr_id);
      }
    }
    RemoteProxy<T> broadcast(val, ioContext());
    value = broadcast.value();
    return RemoteProxy<bool>(res, ioContext()).value();

  } else {
    hid_t attr_id = H5Aopen_name(loc(), id);
    if ((res = !(attr_id < 0))) {
      HDF5Object<T> o(val, attr_id);
      hid_t type_id = o.type();
      res = !(H5Aread(attr_id, type_id, o.write()) < 0);
      if (!res && debug())
	std::cerr << Pooma::context() << ": cannot read attribute " << id << std::endl;
      H5Tclose(type_id);
      H5Aclose(attr_id);
    }
    value = val;
    res = allOk(res);
    if (!res && debug())
      std::cout << Pooma::context() << ": cannot get attribute " << id << std::endl;
    return res;
  }
}


// little (nasty) utilities we need for proper implementation

template <class Engine>
struct LocalPatch
{
  typedef Engine Type_t;
  static Type_t apply(const Type_t& e) { return e; }
};

template <int Dim, class T, class PatchTag>
struct LocalPatch<Engine<Dim, T, Remote<PatchTag> > >
{
  typedef Engine<Dim, T, PatchTag> Type_t;
  static Type_t apply(const Engine<Dim, T, Remote<PatchTag> >& e) { return e.localEngine(); }
};

// wait until we can safely (synchronously) write to Expr, use like
//   Pooma::CountingSemaphore csem;
//   csem.height(1);
//   Pooma::Iterate_t *it = new WriteWaitingIterate<Expr>(e, csem);
//   Pooma::scheduler().handOff(it);
//   csem.wait();

template <class Expr>
struct WriteWaitingIterate : public Pooma::Iterate_t {
  WriteWaitingIterate(const Expr& e, Pooma::CountingSemaphore& csem)
    : Pooma::Iterate_t(Pooma::scheduler()),
      e_m(e), csem_m(csem)
  {
    DataObjectRequest<WriteRequest> writeReq(*this);
    engineFunctor(e_m, writeReq);
    Pooma::addIncomingMessage();
  }
  virtual void run()
  {
    csem_m.incr();
  }
  virtual ~WriteWaitingIterate()
  {
    DataObjectRequest<WriteRelease> writeRel;
    engineFunctor(e_m, writeRel);
  }
  Expr e_m;
  Pooma::CountingSemaphore& csem_m;
};

// wait until we can safely (synchronously) read from Expr, use like
//   Pooma::CountingSemaphore csem;
//   csem.height(1);
//   Pooma::Iterate_t *it = new ReadWaitingIterate<Expr>(e, csem);
//   Pooma::scheduler().handOff(it);
//   csem.wait();

template <class Expr>
struct ReadWaitingIterate : public Pooma::Iterate_t {
  ReadWaitingIterate(const Expr& e, Pooma::CountingSemaphore& csem)
    : Pooma::Iterate_t(Pooma::scheduler()),
      e_m(e), csem_m(csem)
  {
    //DataObjectRequest<WriteRequest> writeReq(*this);
    //DataObjectRequest<ReadRequest> readReq(writeReq);
    DataObjectRequest<ReadRequest> readReq(*this);
    engineFunctor(e_m, readReq);
    Pooma::addIncomingMessage();
  }
  virtual void run()
  {
    csem_m.incr();
  }
  virtual ~ReadWaitingIterate()
  {
    //DataObjectRequest<WriteRelease> writeRel;
    //DataObjectRequest<ReadRelease> readRel(writeRel);
    DataObjectRequest<ReadRelease> readRel;
    engineFunctor(e_m, readRel);
  }
  Expr e_m;
  Pooma::CountingSemaphore& csem_m;
};



template <int Dim, class T, class EngineTag, class T2>
bool HDF5File::writeEngine(const char *id, const Engine<Dim, T, EngineTag>& e,
			   const GuardLayers<Dim>& g)
{
  typedef Engine<Dim, T, EngineTag> Engine_t;
  hid_t filespace_id = -1, dataset_id = -1;
  bool res = true;

  if (io() || pio()) {
    // create dataset
    if (!((dataset_id = H5Dopen(loc(), id)) < 0)) {
      filespace_id = H5Dget_space(dataset_id);
      int ndims = H5Sget_simple_extent_ndims(filespace_id);
      if (ndims == Dim) {
	hsize_t dims[ndims];
	H5Sget_simple_extent_dims(filespace_id, dims, NULL);
	for (int i=0; i<Dim; ++i)
	  if (dims[i] != e.domain()[i].size())
	    res = false;
      } else
	res = false;
      if (res == false) {
	if (debug())
	  std::cerr << "dataspace dimensions do not match." << std::endl;
	H5Sclose(filespace_id);
	H5Dclose(dataset_id);
      }
    } else {
      hid_t filetype_id = HDF5TypeTraits<T2>::type();
      filespace_id = H5Screate_simple(e.domain());
      dataset_id = H5Dcreate(loc(), id, filetype_id, filespace_id, H5P_DEFAULT);
      H5Tclose(filetype_id);
      if (dataset_id < 0) {
	H5Sclose(filespace_id);
	res = false;
      }
    }
  }


  if (!pio()) {
    if (!RemoteProxy<bool>(res, ioContext()).value())
      return false;

    // iterate over engines layout
    typedef typename Engine_t::Layout_t Layout_t;
//     for (typename Layout_t::const_iterator subdomi = e.layout().beginGlobal();
// 	 subdomi != e.layout().endGlobal();
// 	 ++subdomi) {
    //###################### my change
    Layout_t e_layout = e.layout();
    for (typename Layout_t::const_iterator subdomi = e_layout.beginGlobal();
	 subdomi != e_layout.endGlobal();
	 ++subdomi) {
    //###################### my change

      // use part of subdomain inside engines domain (view)
      Interval<Dim> subdom = intersect((*subdomi).domain(), e.domain());

      // copy data to local (on I/O context) brick array by going via
      // an array view of the engine
      typedef Array<Dim, T, Remote<Brick> > LArray_t;
      LArray_t a;
      a.engine() = Engine<Dim, T, Remote<Brick> >(ioContext(), subdom);
      typedef typename NewEngine<Engine_t, Interval<Dim> >::Type_t ViewEngine_t;
      Array<Dim, T, typename ViewEngine_t::Tag_t> e_array(ViewEngine_t(e, subdom));
      a = e_array;

      // We get into serious trouble (out of memory), if not flushing I/O to the
      // nodes now.  Also we need to blockAndEvaluate() for getting the data available
      // for write.
      Pooma::blockAndEvaluate();

      if (io()) {
	// write into hyperslab of file - remeber to move subdom to its zero-based total domain
	if (H5Sselect_hyperslab(filespace_id, H5S_SELECT_SET, subdom - e.domain().mins()) >= 0) {
	  HDF5Object<typename LArray_t::Engine_t::LocalEngine_t> lo(a.engine().localEngine());
	  hid_t memtype_id = lo.type();
	  hid_t memspace_id = lo.dataspace();
	  if (H5Dwrite(dataset_id, memtype_id, memspace_id, filespace_id, H5P_DEFAULT, lo.read()) < 0)
	    res = false;
	  H5Sclose(memspace_id);
	  H5Tclose(memtype_id);
	} else
	  res = false;
      }
    }

    if (io()) {
      H5Sclose(filespace_id);
      H5Dclose(dataset_id);
    }

  } else {
    if (!allOk(res))
      return false;

    // before starting I/O we need to ensure we can actually read from the engine
#if 1
    Pooma::blockAndEvaluate();
#else
    {
      Pooma::CountingSemaphore csem;
      csem.height(1);
      Pooma::scheduler().beginGeneration();
      Pooma::Iterate_t *it = new ReadWaitingIterate<Engine_t>(e, csem);
      Pooma::scheduler().handOff(it);
      Pooma::scheduler().endGeneration();
      csem.wait();
    }
#endif

    // parallel I/O version, iterate over engines local patches of layout
    typedef typename Engine_t::Layout_t Layout_t;
//     for (typename Layout_t::const_iterator subdomi = e.layout().beginLocal();
// 	 subdomi != e.layout().endLocal();
// 	 ++subdomi) {
    //###################### my change
    Layout_t e_layout = e.layout();
    for (typename Layout_t::const_iterator subdomi = e_layout.beginLocal();
	 subdomi != e_layout.endLocal();
	 ++subdomi) {
    //###################### my change
      // use part of subdomain inside engines domain (view)
      Interval<Dim> subdom = intersect((*subdomi).domain(), e.domain());

      // Create patch engine that can be used for I/O - possibility #2 (doesn't work)
      typedef typename NewEngine<Engine_t, typename Layout_t::Value_t>::Type_t PatchEngine_t;
      PatchEngine_t pe(NewEngineEngine<Engine_t, typename Layout_t::Value_t>::apply(e, *subdomi));
      typedef typename LocalPatch<PatchEngine_t>::Type_t LocalPatchEngine_t;
      LocalPatchEngine_t lpe(LocalPatch<PatchEngine_t>::apply(pe));
      typedef typename NewEngine<LocalPatchEngine_t, Interval<Dim> >::Type_t PEngineV_t;
      PEngineV_t pev(NewEngineEngine<LocalPatchEngine_t, Interval<Dim> >::apply(lpe, subdom - subdomi->domain().mins()));

      // write into hyperslab of file - remeber to move subdom to its zero-based total domain
      if (H5Sselect_hyperslab(filespace_id, H5S_SELECT_SET, subdom - e.domain().mins()) >= 0) {
	HDF5Object<PEngineV_t> lo(pev);
	hid_t memtype_id = lo.type();
	hid_t memspace_id = lo.dataspace();
	if (H5Dwrite(dataset_id, memtype_id, memspace_id, filespace_id, H5P_DEFAULT, lo.read()) < 0)
	  res = false;
	H5Sclose(memspace_id);
	H5Tclose(memtype_id);
      } else
	res = false;
    }

    H5Sclose(filespace_id);
    H5Dclose(dataset_id);

    res = allOk(res);
  }

  // finally, set stored GuardLayers
  if (res /* && g != GuardLayers<Dim>(0) */) {
    if (pushLocRel(id)) {
      res = setAttribute("guards", g);
      popLoc();
    }
  }
  return res;
}

template <int Dim, class T, class EngineTag>
bool HDF5File::readEngine(const char *id, const Engine<Dim, T, EngineTag>& e,
			  const GuardLayers<Dim>& gr)
{
  typedef Engine<Dim, T, EngineTag> Engine_t;
  hid_t filespace_id = -1, dataset_id = -1;
  bool res = true;

  if (io() || pio()) {
    // open dataset
    dataset_id = H5Dopen(loc(), id);
    if (dataset_id < 0)
      res = false;
    else
      filespace_id = H5Dget_space(dataset_id);
  }

  if (!pio()) {
    if (!RemoteProxy<bool>(res, ioContext()).value())
      return false;

    GuardLayers<Dim> g(0);
    // try to get stored GuardLayers
    if (pushLocRel(id, false)) {
      getAttribute("guards", g);
      popLoc();
    }

    // iterate over engines layout
    typedef typename Engine_t::Layout_t Layout_t;
//     for (typename Layout_t::const_iterator subdomi = e.layout().beginGlobal();
// 	 res && subdomi != e.layout().endGlobal();
// 	 ++subdomi) {
    //###################### my change
    Layout_t e_layout = e.layout();
    for (typename Layout_t::const_iterator subdomi = e_layout.beginGlobal();
	 res && subdomi != e_layout.endGlobal();
	 ++subdomi) {
    //###################### my change
      // use part of subdomain inside engines domain (view)
      Interval<Dim> subdom = intersect((*subdomi).domain(), e.domain());

      // read data into local (on I/O context) brick array
      typedef Array<Dim, T, Remote<Brick> > LArray_t;
      LArray_t a;
      a.engine() = Engine<Dim, T, Remote<Brick> >(ioContext(), subdom);

      if (io()) {
	// read from hyperslab of file - remeber to move subdom to its zero-based total domain,
	// but honour differences in stored/to-be-read GuardLayers
	// this way we can "fix" the hyperslab to be read in the case out reading
	// view is different from the writing view
	Interval<Dim> hyperdom = subdom - e.domain().mins();
	for (int i=0; i<Dim; ++i)
	  hyperdom[i] += g.lower(i) - gr.lower(i);
	if (H5Sselect_hyperslab(filespace_id, H5S_SELECT_SET, subdom - e.domain().mins()) >= 0) {
	  HDF5Object<Engine<Dim, T, Brick> > lo(a.engine().localEngine());
	  hid_t memtype_id = lo.type();
	  hid_t memspace_id = lo.dataspace();
	  if (H5Dread(dataset_id, memtype_id, memspace_id, filespace_id, H5P_DEFAULT, lo.write()) < 0)
	    res = false;
	  H5Sclose(memspace_id);
	  H5Tclose(memtype_id);
	} else
	  res = false;
      }

      // copy data read into array view of engine
      typedef typename NewEngine<Engine_t, Interval<Dim> >::Type_t ViewEngine_t;
      Array<Dim, T, typename ViewEngine_t::Tag_t> e_array(ViewEngine_t(e, subdom));
      e_array = a;

      // We get into serious trouble (out of memory), if not flushing I/O to the
      // nodes now.
      Pooma::blockAndEvaluate();
    }

    if (io()) {
      H5Sclose(filespace_id);
      H5Dclose(dataset_id);
    }

  } else {
    if (!allOk(res))
      return false;

    GuardLayers<Dim> g(0);
    // try to get stored GuardLayers
    if (pushLocRel(id, false)) {
      getAttribute("guards", g);
      popLoc();
    }

    // before starting I/O we need to ensure we can actually write to the engine
#if 1
    Pooma::blockAndEvaluate();
#else
    {
      Pooma::CountingSemaphore csem;
      csem.height(1);
      Pooma::scheduler().beginGeneration();
      Pooma::Iterate_t *it = new WriteWaitingIterate<Engine_t>(e, csem);
      Pooma::scheduler().handOff(it);
      Pooma::scheduler().endGeneration();
      csem.wait();
    }
#endif

    // parallel I/O version, iterate over engines local patches of layout
    typedef typename Engine_t::Layout_t Layout_t;
//     for (typename Layout_t::const_iterator subdomi = e.layout().beginLocal();
// 	 res && subdomi != e.layout().endLocal();
// 	 ++subdomi) {
    //###################### my change
    Layout_t e_layout = e.layout();
    for (typename Layout_t::const_iterator subdomi = e_layout.beginLocal();
	 res && subdomi != e_layout.endLocal();
	 ++subdomi) {
    //###################### my change
      // use part of subdomain inside engines domain (view)
      Interval<Dim> subdom = intersect((*subdomi).domain(), e.domain());

      // Create patch engine that can be used for I/O - possibility # 2 (doesn't work)
      typedef typename NewEngine<Engine_t, typename Layout_t::Value_t>::Type_t PEngine_t;
      PEngine_t pe(NewEngineEngine<Engine_t, typename Layout_t::Value_t>::apply(e, *subdomi));
      typedef typename NewEngine<typename LocalPatch<PEngine_t>::Type_t, Interval<Dim> >::Type_t PEngineV_t;
      PEngineV_t pev(NewEngineEngine<typename LocalPatch<PEngine_t>::Type_t, Interval<Dim> >::apply(LocalPatch<PEngine_t>::apply(pe), subdom - subdomi->domain().mins()));

      // read from hyperslab of file - remeber to move subdom to its zero-based total domain,
      // but honour differences in stored/to-be-read GuardLayers
      // this way we can "fix" the hyperslab to be read in the case out reading
      // view is different from the writing view
      Interval<Dim> hyperdom = subdom - e.domain().mins();
      for (int i=0; i<Dim; ++i)
	hyperdom[i] += g.lower(i) - gr.lower(i);
      if (H5Sselect_hyperslab(filespace_id, H5S_SELECT_SET, subdom - e.domain().mins()) >= 0) {
	HDF5Object<PEngineV_t> lo(pev);
	//HDF5Object<Engine<Dim, T, Brick> > lo(a.engine());
	hid_t memtype_id = lo.type();
	hid_t memspace_id = lo.dataspace();
	if (H5Dread(dataset_id, memtype_id, memspace_id, filespace_id, H5P_DEFAULT, lo.write()) < 0)
	  res = false;
	H5Sclose(memspace_id);
	H5Tclose(memtype_id);
      } else {
	if (debug())
	  std::cout << Pooma::context() << ": Error selecting hyperslab" << std::endl;
	res = false;
      }
    }

    H5Sclose(filespace_id);
    H5Dclose(dataset_id);
  }

  // HDF5Object does this for one thread only
  notifyEngineWrite(e);

  return res;
}


template <int Dim>
bool HDF5File::writeSpacings(const Vector<Dim>& s)
{
  return setAttribute("spacings", s);
}

template <int Dim>
bool HDF5File::writeSpacings(const Vector<Dim>& s, const Interval<Dim>&)
{
  return setAttribute("spacings", s);
}

template <int Dim>
bool HDF5File::writeSpacings(const Array<1, double, Brick> s[Dim])
{
  bool res = setAttribute("spacings0", s[0]);
  if (Dim > 1)
    res = res && setAttribute("spacings1", s[1]);
  if (Dim > 2)
    res = res && setAttribute("spacings2", s[2]);
  return res;
}

template <int Dim>
bool HDF5File::writeSpacings(const Array<1, double, Brick> s[Dim], const Interval<Dim>& I)
{
  bool res = setAttribute("spacings0", s[0](I[0]));
  if (Dim > 1)
    res = res && setAttribute("spacings1", s[1](I[1]));
  if (Dim > 2)
    res = res && setAttribute("spacings2", s[2](I[2]));
  return res;
}

template <int Dim>
bool HDF5File::readSpacings(Vector<Dim>& s)
{
  return getAttribute("spacings", s);
}

template <int Dim>
bool HDF5File::readSpacings(Array<1, double, Brick> s[Dim])
{
  bool res = readOneSpacing("spacings0", s[0]);
  if (Dim > 1)
    res = res && readOneSpacing("spacings1", s[1]);
  if (Dim > 2)
    res = res && readOneSpacing("spacings2", s[2]);
  return res;
}


template <class MeshTag, class T, class EngineTag, class T2, int Dim>
bool HDF5File::writeSubField(const char *id, const Field<MeshTag, T, EngineTag>& f,
			     const GuardLayers<Dim>& g)
{
  PInsist(f.numMaterials() == 1 && f.centeringSize() == 1,
	  "called with more than one subfield");

  // trigger necessary relations
  forEach(f, PerformUpdateTag(), NullCombine());

  typedef typename Field<MeshTag, T, EngineTag>::Engine_t Engine_t;
  typedef typename NewEngine<Engine_t, Interval<Dim> >::Type_t ViewEngine_t;
  if (!writeEngine<Dim, T, typename ViewEngine_t::Tag_t, T2>
      (id, f(grow(f.physicalDomain(), g)).engine(), g))
    return false;


  // rest is relative to dataset
  if (!pushLocRel(id))
    Pooma::pAbort("cannot push written dataset");


  // store origin/spacings/domain which may differ from global ones
//   setAttribute("origin", positions(f).read(grow(f.physicalDomain(), g).firsts()));
//   writeSpacings<Dim>(f.mesh().spacings(), grow(f.physicalDomain(), g));
//   setAttribute("domain", grow(f.physicalDomain(), g).sizes());

  // store BCs
  writeBCs(f);

  popLoc();

  return true;
}

template <class MeshTag, class T, class EngineTag, int Dim>
bool HDF5File::readSubField(const char *id, const Field<MeshTag, T, EngineTag>& f,
			    GuardLayers<Dim>& g)
{
  PInsist(f.numMaterials() == 1 && f.centeringSize() == 1,
	  "called with more than one subfield");

  // try to get GuardLayer information
  g = GuardLayers<Dim>(0);
  if (pushLocRel(id, false)) {
    getAttribute("guards", g);
    GuardLayers<Dim> fg = f.layout().externalGuards();
    for (int i=0; i<Dim; ++i) {
      g.lower(i) = std::min(g.lower(i), fg.lower(i));
      g.upper(i) = std::min(g.upper(i), fg.upper(i));
    }
    popLoc();
  }

  if (!readEngine(id, f(grow(f.physicalDomain(), g)).engine(), g))
    return false;

  // rest is relative to dataset
  if (!pushLocRel(id, false))
    Pooma::pAbort("cannot push read dataset");
  readBCs(f);
  popLoc();

  return true;
}


#if 0
template <int Dim, class T2, class MeshTag, class T, class EngineTag>
bool HDF5File::writeFieldOther(const char *id, const Field<MeshTag, T, EngineTag>& f,
			       const GuardLayers<Dim>& g)
#else
template <class T2, class MeshTag, class T, class EngineTag>
bool HDF5File::writeFieldOther(const char *id, const Field<MeshTag, T, EngineTag>& f,
			       const GuardLayers<MeshTag::dimensions>& g)
#endif
{
  if (f.numMaterials() == 1 && f.centeringSize() == 1)
    return writeSubField<MeshTag, T, EngineTag, T2>(id, f, g);

  if (f.numMaterials() == 1) {
    bool res = true;
    if (!pushLocRel(id))
      return false;
    for (int i=0; i<f.centeringSize(); ++i) {
      char cid[16];
      snprintf(cid, 16, "centering%i", i);
      res = res && writeSubField<MeshTag, T, EngineTag, T2>(cid, f.center(i), g);
    }
    popLoc();
    return res;
  }

  // FIXME

  return false;
}


template <class MeshTag, class T, class EngineTag, int Dim>
bool HDF5File::readField(const char *id, const Field<MeshTag, T, EngineTag>& f,
			 GuardLayers<Dim>& g)
{
  bool res = true;

  if (f.numMaterials() == 1 && f.centeringSize() == 1) {
    res = readSubField(id, f, g);
    if (res)
      f.setDirty();
    return res;
  } 

  if (f.numMaterials() == 1) {
    if (!pushLocRel(id, false))
      return false;
    for (int i=0; i<f.centeringSize(); ++i) {
      char cid[16];
      snprintf(cid, 16, "centering%i", i);
      res = res && readSubField(cid, f.center(i), g);
    }
    popLoc();
    if (res)
      f.setDirty();
    return res;
  }

  // FIXME

  return false;
}


template <class T2, int Dim, class T, class EngineTag>
bool HDF5File::writeArrayOther(const char *id, const Array<Dim, T, EngineTag>& a)
{
  typedef typename Array<Dim, T, EngineTag>::Engine_t Engine_t;
  typedef typename NewEngine<Engine_t, Interval<Dim> >::Type_t ViewEngine_t;
  //if (!writeEngine<Dim, T, typename ViewEngine_t::Tag_t, T2>(id, a(a.physicalDomain()).engine(), GuardLayers<Dim>(0) /* FIXME */))
  if (!writeEngine<Dim, T, typename ViewEngine_t::Tag_t, T2>(id, a(a.domain()).engine(), GuardLayers<Dim>(0) /* FIXME */))
    return false;

  // rest is relative to dataset
  if (!pushLocRel(id))
    Pooma::pAbort("cannot push written dataset");

  setAttribute("domain", a.physicalDomain().sizes());
  popLoc();

  return true;
}


// My addition for Dynamic Array ********************************************
// **************************************************************************

template <class T, class EngineTag>
bool HDF5File::writeDynamicArrayOther(const char *id, const DynamicArray<T, EngineTag>& a)
{
  typedef typename DynamicArray<T, EngineTag>::Engine_t Engine_t;
  typedef typename NewEngine<Engine_t, Interval<1> >::Type_t ViewEngine_t;
  //if (!writeEngine<Dim, T, typename ViewEngine_t::Tag_t, T2>(id, a(a.physicalDomain()).engine(), GuardLayers<Dim>(0) /* FIXME */))
  if (!writeEngine<1, T, typename ViewEngine_t::Tag_t, T>(id, a(a.domain()).engine(), GuardLayers<1>(0) /* FIXME */))
    return false;

  // rest is relative to dataset
  if (!pushLocRel(id))
    Pooma::pAbort("cannot push written dataset");

  setAttribute("domain", a.domain().sizes());
  popLoc();

  return true;
}

// template <int Dim, class T, class EngineTag>
// bool HDF5File::readArray(const char *id, const DynamicArray<T, EngineTag>& a)
// {
//   return readEngine(id, a(a.domain()).engine(), GuardLayers<Dim>(0) /* FIXME */);
// }

// **************************************************************************
// My addition for Dynamic Array ********************************************


template <int Dim, class T, class EngineTag>
bool HDF5File::readArray(const char *id, const Array<Dim, T, EngineTag>& a)
{
  //  return readEngine(id, a(a.physicalDomain()).engine(), GuardLayers<Dim>(0) /* FIXME */);
  return readEngine(id, a(a.domain()).engine(), GuardLayers<Dim>(0) /* FIXME */);
}


/// store boundary conditions of field at current location
template <class MeshTag, class T, class EngineTag>
bool HDF5File::writeBCs(const Field<MeshTag, T, EngineTag>& f)
{
  enum { Dim = Field<MeshTag, T, EngineTag>::dimensions };

  // loop through list of relations and store information about boundary conditions

  typedef Field<MeshTag, T, EngineTag> Field_t;
  typedef Relation0<Field_t, PeriodicFaceBC<Dim> >* PeriodicFaceBC_t;
  typedef Relation0<Field_t, PosReflectFaceBC<Dim> >* PosReflectFaceBC_t;
  typedef Relation0<Field_t, NegReflectFaceBC<Dim> >* NegReflectFaceBC_t;
  typedef Relation0<Field_t, ConstantFaceBC<Dim, T> >* ConstantFaceBC_t;
  typedef Relation0<Field_t, ZeroGradientFaceBC<Dim> >* ZeroGradientFaceBC_t;

  RelationList& relations = f.fieldEngine().relations();
  for (int i=0; i<relations.size(); ++i) {
    RelationListItem *item = relations(i);
    char buf[256];
    // char *bctype;
    std::string bctype;
    int face;

    if (dynamic_cast<PeriodicFaceBC_t>(item)) {
      const PeriodicFaceBC<Dim>& bc = static_cast<PeriodicFaceBC_t>(item)->functor();
      bctype = "periodic";
      face = bc.face();

    } else if (dynamic_cast<PosReflectFaceBC_t>(item)) {
      const PosReflectFaceBC<Dim>& bc = static_cast<PosReflectFaceBC_t>(item)->functor();
      bctype = "reflecting+f";
      face = bc.face();
      /*bool*/int enforce = bc.enforceZeroBoundary();
      snprintf(buf, 256, "BC%ienforceZeroB", face);
      setAttribute(buf, enforce);

    } else if (dynamic_cast<NegReflectFaceBC_t>(item)) {
      const NegReflectFaceBC<Dim>& bc = static_cast<NegReflectFaceBC_t>(item)->functor();
      bctype = "reflecting-f";
      face = bc.face();
      /*bool*/int enforce = bc.enforceZeroBoundary();
      snprintf(buf, 256, "BC%ienforceZeroB", face);
      setAttribute(buf, enforce);

    } else if (dynamic_cast<ConstantFaceBC_t>(item)) {
      const ConstantFaceBC<Dim, T>& bc
	= static_cast<ConstantFaceBC_t>(item)->functor();
      bctype = "constant";
      face = bc.face();
      T value = bc.constant();
      snprintf(buf, 256, "BC%ivalue", face);
      setAttribute(buf, value);

    } else if (dynamic_cast<ZeroGradientFaceBC_t>(item)) {
      const ZeroGradientFaceBC<Dim>& bc = static_cast<ZeroGradientFaceBC_t>(item)->functor();
      bctype = "zero_gradient";
      face = bc.face();

    } else
      continue;

    snprintf(buf, 256, "BC%i", face);
    if (!setAttribute(buf, std::string(bctype)))
      Pooma::pAbort("cannot store BC type attribute");
  }

  return true;
}


/// read boundary conditions from current location and apply to field
template <class MeshTag, class T, class EngineTag>
bool HDF5File::readBCs(const Field<MeshTag, T, EngineTag>& f)
{
  enum { Dim = Field<MeshTag, T, EngineTag>::dimensions };

  // loop through list of attributes and restore boundary condition relations

  for (int face=0; face<2*Dim; ++face) {
    char buf[256];
    snprintf(buf, 256, "BC%i", face);
    std::string bctype;
    bctype.reserve(256);
    getAttribute(buf, bctype);
    if (bctype.compare("periodic") == 0) {
      Pooma::addPeriodicFaceBC(f, face);
    } else if (bctype.compare("reflecting+f") == 0) {
      int enforce = 0;
      snprintf(buf, 256, "BC%ienforceZeroB", face);
      getAttribute(buf, enforce);
      Pooma::addPosReflectFaceBC(f, face, enforce);
    } else if (bctype.compare("reflecting-f") == 0) {
      int enforce = 0;
      snprintf(buf, 256, "BC%ienforceZeroB", face);
      getAttribute(buf, enforce);
      Pooma::addNegReflectFaceBC(f, face, enforce);
    } else if (bctype.compare("constant") == 0) {
      T value;
      snprintf(buf, 256, "BC%ivalue", face);
      getAttribute(buf, value);
      Pooma::addConstantFaceBC(f, face, value);
    } else if (bctype.compare("zero_gradient") == 0) {
      Pooma::addZeroGradientFaceBC(f, face);
    } else
      Pooma::pwarn << "Unsupported BC attribute for face " << face << '\n';
  }

  return true;
}
