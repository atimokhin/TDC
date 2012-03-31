#!/bin/bash

#PBS -N Prop_jm1.5
#PBS -e ../RESULTS/Prop_jm1.5.err
#PBS -o ../RESULTS/Prop_jm1.5.out

#PBS -l nodes=1:ppn=1,walltime=72:00:00
#PBS -q henyey_serial 
#PBS -V

# if in on henyey cd to working directory 
if [ "$MY_HOST" = "henyey" ]
then
    cd $PBS_O_WORKDIR
fi

# start simulations
./prop.exe --noPairs --id=SCLF__jm1.5_Pcf1e8_L1_nGJ5e4_nx5e3_dt4e-5__RhoGJConst__R6C_Xb0.7__dP5e-2_inj8_s1
