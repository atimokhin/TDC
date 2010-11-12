#!/usr/bin/python

import matplotlib.pyplot as plt

# PDG Boiler plate ==========================================
# append module search path for ATbase
import sys
sys.path.append('/home/atim/WORK/C++/ATbase2/lib_python/')
# import wrapper module for PlotData1D generating function
import _py_PDG__rho_gj
# import ATbase module
import ATbase as AT
# ============================================================


filename='cascade.input'

#initialize PlotData1D object
pd=AT.PlotData1D(_py_PDG__rho_gj.PDG__rho_gj(filename,100))

# do whatever your want :)

ax = plt.subplot(111)
ax.plot(pd.x,pd.y)
ax.set_xlim(pd.xlim)
ax.set_ylim([-2.1,2.1])

ax.set_xlabel('$x$')
ax.set_ylabel(r'$\rho_\mathrm{GJ}$')
ax.set_title(r'$\rho_\mathrm{GJ}$' +' from \"%s\"' % filename)
plt.show()
