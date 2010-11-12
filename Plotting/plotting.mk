
PYTHON_INCLUDE=-I /usr/include/python2.6/ -I /usr/lib/python2.6/site-packages/numpy/core/include/numpy/

cpp_plotting_files = $(wildcard Plotting/*.cpp)
OBJ_PLOTTING  = $(cpp_plotting_files:.cpp=.o)

MODULES=$(patsubst %.cpp, %.so, $(wildcard Plotting/_py_*.cpp))

TEST_EXE_FILES=$(patsubst  %.cpp, %, $(wildcard Plotting/*.exe.cpp))


.PHONY: plotting clean_plotting

plotting: $(OBJ_PLOTTING) $(MODULES) $(TEST_EXE_FILES)

$(OBJ_PLOTTING):%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<  \
	$(ATBASE_INCLUDE) -I$(ATBASE_DIR)/lib_python $(PYTHON_INCLUDE)

# Python modules --------------------
Plotting/_py_PDG__rho_gj.so:%.so:%.o Plotting/PDG__rho_gj.o \
	$(OBJ_RHO_GJ) $(OBJ_SETUP_PARAMETERS) $(OBJ_SETUP_DIMENSIONAL_CONSTANTS)
	$(CXX) $(CXXFLAGS) $^ -shared -o $@ -L$(ATBASE_DIR)/lib -lATbase -lhdf5 

# C++ test files (test PlotData generating functions)
$(TEST_EXE_FILES):%.exe:%.exe.o Plotting/PDG__rho_gj.o \
	$(OBJ_RHO_GJ) $(OBJ_SETUP_PARAMETERS) $(OBJ_SETUP_DIMENSIONAL_CONSTANTS)
	$(CXX) $(CXXFLAGS) $^ -o $@  -L$(ATBASE_DIR)/lib -lATbase -lhdf5


# <===========
# <=========== depend
# <===========
DEP_PLOTTING_OBJ=$(patsubst %.o, %.d, $(OBJ_PLOTTING) ) 

$(DEP_PLOTTING_OBJ): %.d : %.cpp
	gcc -MM -MG -MT $(patsubst %.cpp, %.o, $<)  $< -MF $@
	@perl -pi -e 's|Pooma/\w*.h||g; s|^\s*\\\n||g' $@
	@perl -pi -e 's|(.*)*RandomLib/\w*.hpp||g; s|^\s*\\\n||g' $@
	@perl -pi -e 's|IO/HDF5.h||g' $@
	@perl -pi -e 's|ATbase\w*.h||g' $@
	@perl -pi -e 's|hdf5.h||g' $@
	@echo >> $@ 


clean_plotting:
	-(cd Plotting; rm -f *~ *.o *.so *.exe) 