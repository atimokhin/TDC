#!/bin/bash

#PBS -N SCLF_jp0.5_Ap0.3
#PBS -e SCLF_jp0.5_Ap0.3.err
#PBS -o SCLF_jp0.5_Ap0.3.out

#PBS -l nodes=1:ppn=1,walltime=72:00:00
#PBS -q queuename
#PBS -V

# if in on henyey cd to working directory 
HOST=`hostname`
if [ "$HOST" = "ln000" ]
then
    cd $PBS_O_WORKDIR
fi

# start simulations
./run_cascade.exe