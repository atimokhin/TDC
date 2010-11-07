
PYTHON_INCLUDE=-I /usr/include/python2.6/ -I /usr/lib/python2.6/site-packages/numpy/core/include/numpy/

cpp_plotting_files = $(wildcard Plotting/*.cpp)
OBJ_PLOTTING  = $(cpp_plotting_files:.cpp=.o)

PYTHON_MODULES_PLOTTING=$(patsubst %.cpp,%.so, $(wildcard Plotting/_py_*.cpp))

MODULES=Plotting/_py_PDG__rho_gj.so

TEST_EXE_FILES=$(patsubst  %.cpp,%, $(wildcard Plotting/*.exe.cpp) )



.PHONY: plotting modules clean_plotting

plotting: $(OBJ_PLOTTING) $(MODULES) $(TEST_EXE_FILES)

$(OBJ_PLOTTING):%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<  \
	$(ATBASE_INCLUDE) -I$(ATBASE_DIR)/lib_python $(PYTHON_INCLUDE)


# #make Python libary file(s) ----------
# $(PYTHON_MODULES_PLOTTING): $(OBJ_LIB) $(OBJ_TEST)
# 	$(CXX) $(CXXFLAGS) $^ $(AT_LIB) -lhdf5 -shared -o $@

Plotting/_py_PDG__rho_gj.so:%.so:%.o Plotting/PDG__rho_gj.o \
	$(OBJ_RHO_GJ) $(OBJ_SETUP_PARAMETERS) $(OBJ_SETUP_DIMENSIONAL_CONSTANTS)
	$(CXX) $(CXXFLAGS) $^ -shared -o $@ -L$(ATBASE_DIR)/lib -lATbase -lhdf5 

$(TEST_EXE_FILES):%.exe:%.exe.o Plotting/PDG__rho_gj.o \
	$(OBJ_RHO_GJ) $(OBJ_SETUP_PARAMETERS) $(OBJ_SETUP_DIMENSIONAL_CONSTANTS)
	$(CXX) $(CXXFLAGS) $^ -o $@  -L$(ATBASE_DIR)/lib -lATbase -lhdf5


clean_plotting:
	-(cd Plotting; rm -f *~ *.o *.so *.exe) 