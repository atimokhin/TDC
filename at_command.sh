#!/bin/bash

#PBS -N RS_nGJ2.5e4_nx2.5e3_s1
#PBS -e a.err
#PBS -o a.out
#PBS -l nodes=1:ppn=1,walltime=72:00:00
#PBS -q queuename
#PBS -V
#
#cd $PBS_O_WORKDIR
#
./run_cascade.exe