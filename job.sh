#!/bin/bash
# The #PBS directives are the equivalent of passing the same argument on 
# the qsub command line.

#PBS -N buddhabrot

#PBS -l nodes=2:ppn=4
#PBS -l walltime=01:00:00

#PBS -j oe

#PBS -M ethanjbirch@gmail.com

#PBS -m abe
. /rc/tools/utils/dkinit
use OpenMPI-GCC-4.8

LD_LIBRARY_PATH=/home/A01514050:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

mpirun -np $PBS_NP /home/A01514050/buddhabrot/buddhabrot

convert /home/A01514050/buddhabrot/buddhabrot.ppm /home/A01514050/buddhabrot/buddhabrot.png
