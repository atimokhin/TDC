#!/bin/bash

#PBS -N Arons__j1.059_inj12
#PBS -e ../RESULTS/Arons__j1.059_inj12.err
#PBS -o ../RESULTS/Arons__j1.059_inj12.out

#PBS -l nodes=1:ppn=1,walltime=72:00:00
#PBS -q henyey_serial 
#PBS -V

# if in on henyey cd to working directory 
if [ "$MY_HOST" = "henyey" ]
then
    cd $PBS_O_WORKDIR
fi

# start simulations
./run_cascade.exe --dir ../RESULTS/WD2TB/Crab/ 2>&1

