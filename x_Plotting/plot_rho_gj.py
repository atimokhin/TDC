#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy             as np

# PDG Boiler plate ==========================================
# append module search path for ATbase
import sys
sys.path.append('/home/atim/WORK/C++/ATbase2/lib_python/')
# import wrapper module for PlotData1D generating function
import _py_PDG__rho_gj
# import ATbase module
import ATbase as AT
# ============================================================

# parse command line arguments and get input filename --------
import argparse
import tdc_main_filenames
# setting object responsible for directory names
fn = tdc_main_filenames.tdc_main_Filenames()
# parse command line for filename
parser = argparse.ArgumentParser()
parser.add_argument('--configfile', nargs='?', type=str, help='name of config file')
args = parser.parse_known_args()
# set filename
filename = args[0].configfile if args[0].configfile else 'cascade.input'
filename = fn.get_full_filename(filename)
# ------------------------------------------------------------ 


#initialize PlotData1D object
pd=AT.PlotData1D(_py_PDG__rho_gj.PDG__rho_gj( filename, 100) )

fig=plt.figure()

ax = fig.add_subplot(111)
ax.plot(pd.x,pd.y)
ax.set_xlim(pd.xlim)
## ax.set_ylim([-2.1,2.1])
ax.set_ylim([-2,0.1])

ax.xaxis.set_ticks(np.arange(0,1,0.1), minor=True )
ax.yaxis.set_ticks(np.arange(-2,0.1,0.1), minor=True )
# draw grid lines
ax.grid(b=True, which='both')

ax.set_xlabel('$x$',fontsize='x-large')
ax.set_ylabel(r'$\rho_\mathrm{GJ}$',fontsize='x-large')
## ax.set_title(r'$\rho_\mathrm{GJ}$' +' from \"%s\"' % filename, fontsize='x-small')
plt.show()
