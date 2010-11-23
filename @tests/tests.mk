FILES_TO_BUILD= test_inout.exe test_absorption.exe test_emission.exe  test_limits.exe test_particles.exe


TEST_EXE_FILES=$(addprefix @tests/, $(FILES_TO_BUILD) )

TEST_OBJ=$(addsuffix .o, $(TEST_EXE_FILES))


.PHONY: tests clean_test

tests: $(TEST_EXE_FILES)


OBJ_ABSORPTION=\
	$(SRC_PARTICLES) \
	$(OBJ_CONTAINERS) \
	$(OBJ_CONTROL) \
	$(OBJ_SETUP_DIMENSIONAL_CONSTANTS) \
	$(OBJ_SETUP_PARAMETERS)       \
	$(OBJ_SETUP_PROPERTIES) \
	$(OBJ_INOUT)      \
	$(OBJ_UTILS)      \
	$(OBJ_MESH)      \
	$(OBJ_MC)         \
	$(OBJ_MAGNETIC_FIELD) \
	$(OBJ_FIELDS) 



@tests/test_emission.exe: %.exe : %.exe.o \
	$(RANDOM_LIB_SRC) $(OBJ_ABSORPTION) 
	$(CXX)  -o $@ $^ \
	$(CXXFLAGS)  \
	$(TEST_OPTS) \
	$(INCLUDE_RANDOM_LIB) \
	$(INCLUDE_ARGTABLE2_FLAG) $(LIB_ARGTABLE2_FLAG) \
	$(ATBASE_LIB) $(ATBASE_INCLUDE) \
	$(LIB_POOMA_FLAGS) $(INCLUDE_POOMA_FLAGS) \
	$(LIB_HDF5_FLAGS) $(INCLUDE_HDF5_FLAGS) 

@tests/test_absorption.exe: %.exe : %.exe.o \
	$(RANDOM_LIB_SRC) $(OBJ_ABSORPTION) 
	$(CXX)  -o $@ $^ \
	$(CXXFLAGS)  \
	$(TEST_OPTS) \
	$(INCLUDE_RANDOM_LIB) \
	$(INCLUDE_ARGTABLE2_FLAG) $(LIB_ARGTABLE2_FLAG) \
	$(ATBASE_LIB) $(ATBASE_INCLUDE) \
	$(LIB_POOMA_FLAGS) $(INCLUDE_POOMA_FLAGS) \
	$(LIB_HDF5_FLAGS) $(INCLUDE_HDF5_FLAGS) 


@tests/test_particles.exe: %.exe : %.exe.o  \
	$(RANDOM_LIB_SRC) $(OBJ_PARTICLES) $(OBJ_CONTROL) $(OBJ_SETUP_DIMENSIONAL_CONSTANTS) $(OBJ_SETUP_PARAMETERS) $(OBJ_UTILS) $(OBJ_INOUT) $(OBJ_POOMA_FIX) $(OBJ_CONTAINERS) 
	$(CXX)  -o $@ $^ \
	$(CXXFLAGS)  \
	$(TEST_OPTS) \
	$(MAGNETIC_FIELD_OPTS) \
	$(INCLUDE_RANDOM_LIB) \
	$(INCLUDE_ARGTABLE2_FLAG) $(LIB_ARGTABLE2_FLAG) \
	$(ATBASE_LIB) $(ATBASE_INCLUDE) \
	$(LIB_POOMA_FLAGS) $(INCLUDE_POOMA_FLAGS) \
	$(LIB_HDF5_FLAGS) $(INCLUDE_HDF5_FLAGS) 



@tests/test_inout.exe: %.exe : %.exe.o \
	$(OBJ_CONTROL) $(OBJ_SETUP_DIMENSIONAL_CONSTANTS) $(OBJ_SETUP_PARAMETERS) $(OBJ_INOUT)  $(OBJ_POOMA_FIX) $(OBJ_SOLUTION_PROPERTIES)
	$(CXX)  -o $@ $^ \
	$(CXXFLAGS)  \
	$(TEST_OPTS) \
	$(MAGNETIC_FIELD_OPTS) \
	$(INCLUDE_RANDOM_LIB)  \
	$(INCLUDE_ARGTABLE2_FLAG) $(LIB_ARGTABLE2_FLAG) \
	$(ATBASE_LIB) $(ATBASE_INCLUDE) \
	$(LIB_POOMA_FLAGS) $(INCLUDE_POOMA_FLAGS) \
	$(LIB_HDF5_FLAGS) $(INCLUDE_HDF5_FLAGS) 

@tests/test_limits.exe: %.exe : %.exe.o  
	$(CXX)  -o $@ $^ \
	$(CXXFLAGS)  



$(TEST_OBJ):%.o:%.cpp
	$(CXX)  -o $@ -c $< \
	$(CXXFLAGS)  \
	$(TEST_OPTS) \
	$(MAGNETIC_FIELD_OPTS) \
	$(INCLUDE_POOMA_FLAGS) $(INCLUDE_HDF5_FLAGS) $(ATBASE_INCLUDE) $(LOCAL_LIBS_INCLUDE)


# <===========
# <=========== depend
# <===========
DEP_TEST_OBJ=$(patsubst %.o, %.d, $(TEST_OBJ) ) 

$(DEP_TEST_OBJ): %.d : %.cpp
	gcc -MM -MG -MT $(patsubst %.cpp, %.o, $<)  $< -MF $@
	@perl -pi -e 's|Pooma/\w*.h||g; s|^\s*\\\n||g' $@
	@perl -pi -e 's|(.*)*RandomLib/\w*.hpp||g; s|^\s*\\\n||g' $@
	@perl -pi -e 's|IO/HDF5.h||g' $@
	@perl -pi -e 's|ATbase.h||g' $@
	@perl -pi -e 's|hdf5.h||g' $@
	@echo >> $@ 


clean_test:
	-(cd @test; rm -f *~ *.o *.so *.exe)