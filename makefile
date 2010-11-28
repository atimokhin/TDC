CXXFLAGS= -O3 -fPIC -funroll-loops -march=$(ARCH) -D POOMA_BOUNDS_CHECK=0 -D NOCTAssert -D NOPAssert 
#CXXFLAGS= -g
#CXXFLAGS= -D POOMA_BOUNDS_CHECK=0 -D NOCTAssert -D NOPAssert 

# **************************************
# ******* TEST OPTIONS *****************
# **************************************
#TEST_OPTS := $(TEST_OPTS) -D TEST_CODE
#TEST_OPTS := $(TEST_OPTS) -D ELECTROSTATIC_1D__TEST_GAUSS_LAW
#TEST_OPTS := $(TEST_OPTS) -D ELECTROSTATIC_1D__SOLVE_POISSON_EQUATION
#TEST_OPTS := $(TEST_OPTS) -D ELECTROSTATIC_1D__TEST_NO_RHOGJ
#TEST_OPTS := $(TEST_OPTS) -D TEST_GAMMA2PAIR
#TEST_OPTS := $(TEST_OPTS) -D TEST_CR



# **************************************
# ******* MACHINE-DEPENDENT PART *******
# **************************************
HOST := $(shell hostname)

ifeq ($(HOST),tosha)
ON_LAPTOP=true
else ifeq ($(HOST),mondatta)
ON_DESKTOP_UCB=true
else ifeq ($(HOST),ln000)
ON_HENYEY_UCB=true
endif


# desktop @ UCB++++++++++++++++++++++++
ifdef ON_DESKTOP_UCB
MYHOMEDIR=/export/mondatta1/atim
EXTERNAL_LIBS_DIR=/export/mondatta1/atim/MyPrograms/
INCLUDE_RANDOM_LIB=-I LIBS -DDEFAULT_GENERATOR=SRandomGenerator64
INCLUDE_ARGTABLE2_FLAG=-I$(EXTERNAL_LIBS_DIR)/argtable2/include 
LIB_ARGTABLE2_FLAG=$(EXTERNAL_LIBS_DIR)/argtable2/lib/libargtable2.a
ARCH=athlon64

POOMA_VERSION=my_freepooma_fast_debug
HDFHOMEDIR=$(EXTERNAL_LIBS_DIR)/hdf5-1.8.3/
ATBASE_VERSION= 
# HDF5 
INCLUDE_HDF5_FLAGS=-D H5_USE_16_API -I$(HDFHOMEDIR)/include/ 
LIB_HDF5_FLAGS=-D H5_USE_16_API $(HDFHOMEDIR)/lib/libhdf5_hl.a $(HDFHOMEDIR)/lib/libhdf5.a -lz -lm
# -------------------------------------
# henyey @ UCB+++++++++++++++++++++++++
else ifdef ON_HENYEY_UCB
MYHOMEDIR=/global/home/users/atimokhi/
EXTERNAL_LIBS_DIR=/global/home/users/atimokhi/MY_LIB/
INCLUDE_RANDOM_LIB=-I LIBS -DDEFAULT_GENERATOR=SRandomGenerator64
INCLUDE_ARGTABLE2_FLAG=-I$(EXTERNAL_LIBS_DIR)/argtable2/include 
LIB_ARGTABLE2_FLAG=$(EXTERNAL_LIBS_DIR)/argtable2/lib/libargtable2.a
ARCH=core2

POOMA_VERSION=freepooma_serial
HDFHOMEDIR=$(HDF5DIR)
ATBASE_VERSION= 
# HDF5 
INCLUDE_HDF5_FLAGS=-I$(HDFHOMEDIR)/include/    -D H5_USE_16_API  
LIB_HDF5_FLAGS=-L$(HDFHOMEDIR)/lib/    -D H5_USE_16_API  -lhdf5 -lz -lm
# -------------------------------------
# my laptop++++++++++++++++++++++++++++
else ifdef ON_LAPTOP
EXTERNAL_LIBS_DIR=/usr/local/
MYHOMEDIR=/home/atim
INCLUDE_RANDOM_LIB=-I LIBS -DDEFAULT_GENERATOR=SRandomGenerator32 
INCLUDE_ARGTABLE2_FLAG=
LIB_ARGTABLE2_FLAG= -largtable2
ARCH=pentium-m 

POOMA_VERSION=freepooma_serial
HDFHOMEDIR=$(HDF5DIR)
ATBASE_VERSION=
# HDF5 
INCLUDE_HDF5_FLAGS=-D H5_USE_16_API 
LIB_HDF5_FLAGS=-D H5_USE_16_API  -lhdf5 -lz -lm
endif
# -------------------------------------
# **************************************



# POOMA ********************************
POOMADIR=$(EXTERNAL_LIBS_DIR)/$(POOMA_VERSION)/linux/
INCLUDE_POOMA_FLAGS=-I$(POOMADIR)/src/ \
        -I$(POOMADIR)/lib/PoomaConfiguration 
LIB_POOMA_FLAGS=-L$(POOMADIR)/lib/ -l pooma 

# HDF5 *********************************
#INCLUDE_HDF5_FLAGS=-D H5_USE_16_API -I$(HDFHOMEDIR)/include/ 
#LIB_HDF5_FLAGS=-D H5_USE_16_API -L$(HDFHOMEDIR)/lib/ -lhdf5
#LIB_HDF5_FLAGS=-D H5_USE_16_API $(HDFHOMEDIR)/lib/libhdf5_hl.a $(HDFHOMEDIR)/lib/libhdf5.a -lz -lm

# ATbase *******************************
ATBASE_DIR=$(MYHOMEDIR)/WORK/C++/ATbase2$(ATBASE_VERSION)
ATBASE=$(ATBASE_DIR)/lib/
ATBASE_INCLUDE= -I$(ATBASE)
ATBASE_LIB= -L$(ATBASE) -lATbase -lstdc++


# RandomLib ****************************
RANDOM_LIB_SRC=LIBS/RandomLib/Random.cpp 

# LocalLibs *******************************
LOCAL_LIBS=-L LIBS 
LOCAL_LIBS_INCLUDE=-I LIBS $(INCLUDE_RANDOM_LIB)



# **************************************
# list of source files
# **************************************	
SRC_POOMA_FIX = \
	inout/pooma_hdf/hdf5file.cmpl.cpp \
	inout/pooma_hdf/my_hdf5file.cpp

SRC_CONTAINERS = $(wildcard containers/*.cmpl.cpp)
SRC_INOUT = $(wildcard inout/*.cpp ) 
SRC_UTILS = $(wildcard utils/*.cpp) 
SRC_STOP_AND_SAVE = $(wildcard stop_and_save/*.cpp) 

SRC_SETUP_PARAMETERS = $(wildcard SetupParameters/*.cpp)
SRC_SETUP_DIMENSIONAL_CONSTANTS = $(wildcard SetupDimensionalConstants/*.cpp)
SRC_SETUP_PROPERTIES = $(wildcard SetupProperties/*.cpp) 

SRC_RHO_GJ = $(wildcard _Environment/RhoGJ/*.cpp)
SRC_MAGNETIC_FIELD = $(wildcard _Environment/MagneticField/*.cpp) 

SRC_MESH = $(wildcard _Mesh/*.cpp ) 

SRC_FIELDS = $(wildcard _Fields/*.cpp) 

SRC_PARTICLES=_Particles/particle_population.cpp \
	$(wildcard _Particles/LPT/*.cpp) \
	$(wildcard _Particles/ParticleCaches/*.cpp) \
	_Particles/ID/particle_id.cpp

SRC_MC = $(filter-out %.exe.cpp,$(wildcard _MC/*.cpp)                \
                                $(wildcard _MC/Emission/*.cpp)       \
				$(wildcard _MC/PairProduction/*.cpp)  )

SRC_MC_SAMPLES = $(filter-out %.exe.cpp, $(wildcard _MC/Samples/*.cpp)  )

SRC_CONTROL = $(wildcard _Control/*.cpp)

SRC_SOLUTION_PROPERTIES = $(wildcard _SolutionProperties/*.cpp) 

SRC_CASCADE = cascade.cpp run_cascade.exe.cpp

# **************************************



# **************************************
# list of all object files	
# **************************************
OBJ_CONTAINERS=$(patsubst %.cpp, %.o, $(sort $(SRC_CONTAINERS) ) )
OBJ_INOUT=$(patsubst %.cpp, %.o, $(sort $(SRC_INOUT) $(SRC_POOMA_FIX)) )
OBJ_UTILS=$(patsubst %.cpp, %.o, $(sort $(SRC_UTILS) ) )
OBJ_STOP_AND_SAVE=$(patsubst %.cpp, %.o, $(sort $(SRC_STOP_AND_SAVE) ) )

OBJ_SETUP_PARAMETERS=$(patsubst %.cpp, %.o, $(sort $(SRC_SETUP_PARAMETERS) ) )
OBJ_SETUP_DIMENSIONAL_CONSTANTS=$(patsubst %.cpp, %.o, $(sort $(SRC_SETUP_DIMENSIONAL_CONSTANTS) ) )
OBJ_SETUP_PROPERTIES=$(patsubst %.cpp, %.o, $(sort $(SRC_SETUP_PROPERTIES) ) )

OBJ_RHO_GJ=$(patsubst %.cpp, %.o, $(sort $(SRC_RHO_GJ) ) )
OBJ_MAGNETIC_FIELD=$(patsubst %.cpp, %.o, $(sort $(SRC_MAGNETIC_FIELD) ) )

OBJ_MESH=$(patsubst %.cpp, %.o, $(sort $(SRC_MESH) ) )
OBJ_FIELDS=$(patsubst %.cpp, %.o, $(sort $(SRC_FIELDS) ) )
OBJ_MC=$(patsubst %.cpp, %.o, $(sort $(SRC_MC) ) )
OBJ_MC_SAMPLES=$(patsubst %.cpp, %.o, $(sort $(SRC_MC_SAMPLES) ) )
OBJ_CONTROL=$(patsubst %.cpp, %.o, $(sort $(SRC_CONTROL) ) )
OBJ_SOLUTION_PROPERTIES=$(patsubst %.cpp, %.o, $(sort $(SRC_SOLUTION_PROPERTIES) ) )

OBJ_PARTICLES=$(patsubst %.cpp, %.o, $(sort $(SRC_PARTICLES))) 

# cascade related object files
OBJ_CASCADE=\
	$(patsubst %.cpp, %.o, $(sort $(SRC_CASCADE)) ) \
	$(OBJ_CONTAINERS) \
	$(OBJ_SETUP_PARAMETERS)       \
	$(OBJ_SETUP_DIMENSIONAL_CONSTANTS) \
	$(OBJ_SETUP_PROPERTIES) \
	$(OBJ_INOUT)      \
	$(OBJ_UTILS)      \
	$(OBJ_STOP_AND_SAVE) \
	$(OBJ_MESH)       \
	$(OBJ_RHO_GJ)     \
	$(OBJ_MAGNETIC_FIELD) \
	$(OBJ_MC)         \
	$(OBJ_MC_SAMPLES)         \
	$(OBJ_FIELDS)     \
	$(OBJ_PARTICLES)  \
	$(OBJ_CONTROL)    \
	$(OBJ_SOLUTION_PROPERTIES) 

OBJ_PROPS=\
	$(OBJ_PARTICLES)  \
	$(OBJ_MC_SAMPLES)         \
	$(OBJ_CONTAINERS) \
	$(OBJ_SETUP_PARAMETERS)       \
	$(OBJ_SETUP_PROPERTIES) \
	$(OBJ_INOUT)      \
	$(OBJ_UTILS)      \
	$(OBJ_MESH)      \
	$(OBJ_RHO_GJ)     \
	$(OBJ_MAGNETIC_FIELD) \
	$(OBJ_FIELDS) \
	$(OBJ_SOLUTION_PROPERTIES) 


OBJ=$(sort $(OBJ_CASCADE) )
# ----------------------------------------




.PHONY: all clean cleanall depend save_compiler_opts RandomLib plotting


# <===========
# <=========== all
# <===========

all:  run_cascade.exe print_parameters.exe  save_compiler_opts prop.exe #tests
#all:  calculate_energy.exe  

run_cascade.exe: $(RANDOM_LIB_SRC) $(OBJ_CASCADE) 
	$(CXX)  -o $@ $^ \
	$(CXXFLAGS)  \
	$(TEST_OPTS) \
	$(MAGNETIC_FIELD_OPTS) \
	$(INCLUDE_RANDOM_LIB) \
	$(INCLUDE_ARGTABLE2_FLAG) $(LIB_ARGTABLE2_FLAG) \
	$(ATBASE_LIB) $(ATBASE_INCLUDE) \
	$(LIB_POOMA_FLAGS) $(INCLUDE_POOMA_FLAGS) \
	$(LIB_HDF5_FLAGS) $(INCLUDE_HDF5_FLAGS) 

print_parameters.exe: %.exe : %.exe.cpp $(OBJ_SETUP_DIMENSIONAL_CONSTANTS) $(OBJ_SETUP_PARAMETERS) $(OBJ_SETUP_PROPERTIES) $(OBJ_RHO_GJ) $(OBJ_MAGNETIC_FIELD) $(OBJ_MESH) $(OBJ_INOUT) $(OBJ_CONTROL) $(OBJ_UTILS) 
	$(CXX)  -o $@ $^ \
	$(CXXFLAGS)  \
	$(MAGNETIC_FIELD_OPTS) \
	$(TEST_OPTS) \
	$(INCLUDE_ARGTABLE2_FLAG) $(LIB_ARGTABLE2_FLAG) \
	$(ATBASE_LIB) $(ATBASE_INCLUDE) \
	$(LIB_POOMA_FLAGS) $(INCLUDE_POOMA_FLAGS) \
	$(LIB_HDF5_FLAGS) $(INCLUDE_HDF5_FLAGS) 

prop.exe: %.exe : %.exe.cpp   $(RANDOM_LIB_SRC) $(OBJ_SETUP_DIMENSIONAL_CONSTANTS) $(OBJ_PROPS) $(OBJ_CONTROL) $(OBJ_UTILS) 
	$(CXX)  -o $@ $^ \
	$(CXXFLAGS)  \
	$(TEST_OPTS) \
	$(MAGNETIC_FIELD_OPTS) \
	$(INCLUDE_ARGTABLE2_FLAG) $(LIB_ARGTABLE2_FLAG) \
	$(INCLUDE_RANDOM_LIB) \
	$(ATBASE_LIB) $(ATBASE_INCLUDE) \
	$(LIB_POOMA_FLAGS) $(INCLUDE_POOMA_FLAGS) \
	$(LIB_HDF5_FLAGS) $(INCLUDE_HDF5_FLAGS) 


# rules for objects files
-include make.depend

$(OBJ):%.o:%.cpp
	$(CXX)  -o $@ -c $< \
	$(CXXFLAGS) \
	$(TEST_OPTS) \
	$(MAGNETIC_FIELD_OPTS) \
	$(INCLUDE_POOMA_FLAGS) $(INCLUDE_HDF5_FLAGS) $(INCLUDE_ARGTABLE2_FLAG) $(ATBASE_INCLUDE) $(LOCAL_LIBS_INCLUDE)


# rules for test files 
-include @tests/tests.mk

# rules for plotting files 
-include Plotting/plotting.mk



# <===========
# <=========== depend
# <===========
DEP_OBJ=$(patsubst %.o, %.d, $(OBJ) ) 


depend: $(DEP_OBJ) $(DEP_TEST_OBJ) $(DEP_PLOTTING_OBJ)
	@cat $^ > make.depend
	@-rm -f $^
	@echo '>>>>>>>>' make.depend is created 

$(DEP_OBJ): %.d : %.cpp
	gcc -MM -MG -MT $(patsubst %.cpp, %.o, $<)  $< -MF $@
	@perl -pi -e 's|Pooma/\w*.h||g; s|^\s*\\\n||g' $@
	@perl -pi -e 's|(.*)*RandomLib/\w*.hpp||g; s|^\s*\\\n||g' $@
	@perl -pi -e 's|IO/HDF5.h||g' $@
	@perl -pi -e 's|ATbase.h||g' $@
	@perl -pi -e 's|hdf5.h||g' $@
	@perl -pi -e 's|argtable2.h||g' $@
	@echo >> $@ 





# <===========
# <=========== clean
# <===========
# directories not related to the main code <<<<<<<<<<<<<<<
NON_CODE_DIRS= LIBS MATHEMATICA

# all directories i this project
ALL_DIRS=$(shell  find -type d)
# directories with files of the main code
CODE_DIRS=$(filter-out $(addprefix ./,$(addsuffix %,$(NON_CODE_DIRS))), $(ALL_DIRS))

# siffixies of the files to be deleted when clean command is issued
TMP_SUFFIXIES=*~ *.o *.exe 
# files to be deleted when clean command is issued
TMP_FILENAMES=$(wildcard $(foreach SFX,$(TMP_SUFFIXIES),$(addsuffix /$(SFX),$(1)) ) )

# siffixies of the files to be deleted when clean command is issued
ALL_TMP_SUFFIXIES=*~ *.o *.exe *.d *.depend \#*\#
# files to be deleted when clean command is issued
ALL_TMP_FILENAMES=$(wildcard $(foreach SFX,$(ALL_TMP_SUFFIXIES),$(addsuffix /$(SFX),$(1)) ) )


clean: 
	@echo '>>>>>>>>' start clean 
	-rm -f $(call TMP_FILENAMES, $(CODE_DIRS))
	-rm -f $(COMPILER_OPTS_FILENAME)
	@echo '>>>>>>>>' clean finished

cleanall:
	@echo '>>>>>>>>' start cleanall 
	-rm -f $(call ALL_TMP_FILENAMES, $(ALL_DIRS))
	-rm -f $(wildcard callgrind.*)
	-rm -f $(COMPILER_OPTS_FILENAME)
	@echo '>>>>>>>>' cleanall finished



# <===========
# <=========== save_compiler_opts
# <===========
COMPILER_OPTS_FILENAME=COMPILER_OPTS

save_compiler_opts:
	@echo TEST_OPTS= $(TEST_OPTS) >  $(COMPILER_OPTS_FILENAME)
	@echo CXXFLAGS = $(CXXFLAGS)  >> $(COMPILER_OPTS_FILENAME)
	@echo ============================================  >> $(COMPILER_OPTS_FILENAME)
	@echo MAGNETIC_FIELD_OPTS = $(MAGNETIC_FIELD_OPTS)  >> $(COMPILER_OPTS_FILENAME)
	@echo ============================================  >> $(COMPILER_OPTS_FILENAME)
