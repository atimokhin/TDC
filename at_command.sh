#!/bin/bash

#PBS -N SCLF_jp0.5_Ap0.3
#PBS -e ../RESULTS/SCLF_jp0.5_Ap0.3.err
#PBS -o ../RESULTS/SCLF_jp0.5_Ap0.3.out

#PBS -l nodes=1:ppn=1,walltime=72:00:00
#PBS -q henyey_serial 
#PBS -V

# if in on henyey cd to working directory 
if [ "$MY_HOST" = "henyey" ]
then
    cd $PBS_O_WORKDIR
fi

# start simulations
./run_cascade.exe