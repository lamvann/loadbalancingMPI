These are the instructions on how to execute and run bmptool using the MPI utility for
multiprocessing.

1. BUILD
Makefile takes full care of the build, just go to the project directory and  type 'make' in the terminal.

2. RUN
mpirun -np 4 bmptool

where [-np] flag specifies number of processors to run bmptool.

3. NOTE
This program assumes you have MPI utility installed on your system and is part of your PATH.
