PYTHON_INCLUDE=\
	-I $(PYTHON_DISTR_INCLUDE_DIR) \
	-I $(PYTHON_MODULES_DIR)/numpy/core/include/numpy/

cpp_plotting_files = $(wildcard x_Plotting/*.cpp)
OBJ_PLOTTING  = $(cpp_plotting_files:.cpp=.o)

MODULES=$(patsubst %.cpp, %.so, $(wildcard x_Plotting/_py_*.cpp))

TEST_EXE_FILES=$(patsubst  %.cpp, %, $(wildcard x_Plotting/*.exe.cpp))


.PHONY: plotting clean_plotting

plotting: $(OBJ_PLOTTING) $(MODULES) $(TEST_EXE_FILES)

$(OBJ_PLOTTING):%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<  \
	$(ATBASE_INCLUDE) -I$(ATBASE_DIR)/lib_python $(PYTHON_INCLUDE)

# Python modules --------------------
x_Plotting/_py_PDG__rho_gj.so:%.so:%.o x_Plotting/PDG__rho_gj.o \
	$(OBJ_RHO_GJ) $(OBJ_SETUP_PARAMETERS) $(OBJ_SETUP_DIMENSIONAL_CONSTANTS)
	$(CXX) $(CXXFLAGS) $^ -shared -o $@ -L$(ATBASE_DIR)/lib -lATbase -lhdf5 

# C++ test files (test PlotData generating functions)
$(TEST_EXE_FILES):%.exe:%.exe.o x_Plotting/PDG__rho_gj.o \
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
	-(cd x_Plotting; rm -f *~ *.o *.so *.exe) 