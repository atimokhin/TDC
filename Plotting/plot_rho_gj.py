# append module search path for ATbase
import sys
sys.path.append('/home/atim/WORK/C++/ATbase2/lib_python/')
# import wrapper module for PlotData1D generating function
import _py_PDG__rho_gj
# import ATbase module
import ATbase as AT

#initialize PlotData1D object
pd=AT.PlotData1D(_py_PDG__rho_gj.PDG__rho_gj('../cascade.input',100))

# do whatever your want :) ..
