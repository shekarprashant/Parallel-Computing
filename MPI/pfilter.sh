#!/bin/sh

#WINDOWS="3 5 8 13"
#NPROCS="2 4 8 16"
#FILTERS="mean median"

WINDOWS="3"
NPROCS="4"
FILTERS="mean"

for FILTER in $FILTERS ; do 
   for WSIZE in $WINDOWS ; do
     for NPROC in $NPROCS ; do 
		 echo $FILTER
		 echo $WSIZE
		 echo $NPROC
       OUT=moon_f${FILTER}_w${WSIZE}_n${NPROC}.tiff
       mpirun --hostfile hosts -np ${NPROC} pfilter input/moon_in.tiff ${OUT} ${FILTER} ${WSIZE}
     done
   done
done
 
