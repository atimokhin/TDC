ASSIST_FILES_TO_BUILD= pairs_from_single_particle.exe

ASSIST_EXE_FILES=$(addprefix x_Assist/, $(ASSIST_FILES_TO_BUILD) )

ASSIST_OBJ=$(addsuffix .o, $(ASSIST_EXE_FILES))


.PHONY: assits clean_assist

assist: $(ASSIST_EXE_FILES)


SINGLE_PARTICLE_EMISSION_OBJ=\
	$(RANDOM_LIB_SRC) \
	$(OBJ_CONTROL) \
	$(OBJ_SETUP_DIMENSIONAL_CONSTANTS) \
	$(OBJ_SETUP_PARAMETERS)       \
	$(OBJ_INOUT)      \
	$(OBJ_UTILS)      \
	$(OBJ_MESH)       \
	$(OBJ_MC)         \
	$(OBJ_MC_SAMPLES)     \
	$(OBJ_RHO_GJ)     \
	$(OBJ_MAGNETIC_FIELD) \
	$(OBJ_CONTAINERS) \
	$(OBJ_FIELDS)     \
	$(OBJ_PARTICLES) $(OBJ_PARTICLE_ID) $(OBJ_PARTICLE_CACHES) 

SINGLE_PARTICLE_EMISSION_FLAGS=\
	$(INCLUDE_RANDOM_LIB) \
	$(INCLUDE_ARGTABLE2_FLAG) $(LIB_ARGTABLE2_FLAG) \
	$(ATBASE_LIB) $(ATBASE_INCLUDE) \
	$(LIB_POOMA_FLAGS) $(INCLUDE_POOMA_FLAGS) \
	$(LIB_HDF5_FLAGS) $(INCLUDE_HDF5_FLAGS) 


x_Assist/pairs_from_single_particle.exe:%.exe:%.exe.o  $(SINGLE_PARTICLE_EMISSION_OBJ) 
	$(CXX)  -o $@ $^ $(CXXFLAGS)  \
	$(TEST_OPTS) \
	$(SINGLE_PARTICLE_EMISSION_FLAGS)


$(ASSIST_OBJ):%.o:%.cpp
	$(CXX)  -o $@ -c $< $(CXXFLAGS)  \
	$(TEST_OPTS) \
	$(INCLUDE_POOMA_FLAGS) $(INCLUDE_HDF5_FLAGS) $(ATBASE_INCLUDE) $(LOCAL_LIBS_INCLUDE)


# <===========
# <=========== depend
# <===========
DEP_ASSIST_OBJ=$(patsubst %.o, %.d, $(ASSIST_OBJ) ) 

$(DEP_ASSIST_OBJ): %.d : %.cpp
	gcc -MM -MG -MT $(patsubst %.cpp, %.o, $<)  $< -MF $@
	@perl -pi -e 's|Pooma/\w*.h||g; s|^\s*\\\n||g' $@
	@perl -pi -e 's|(.*)*RandomLib/\w*.hpp||g; s|^\s*\\\n||g' $@
	@perl -pi -e 's|IO/HDF5.h||g' $@
	@perl -pi -e 's|ATbase.h||g' $@
	@perl -pi -e 's|hdf5.h||g' $@
	@echo >> $@ 


clean_assist:
	-(cd x_Assit; rm -f *~ *.o *.so *.exe)