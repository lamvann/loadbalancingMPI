bmptool:	bmplib.o main.o
	/usr/lib64/openmpi/bin/mpicc bmplib.o main.o -o $@

bmplib.o:	bmplib.c bmplib.h
	/usr/lib64/openmpi/bin/mpicc -Wall -c $< -o $@

main.o:	main.c bmplib.h 
	/usr/lib64/openmpi/bin/mpicc -Wall -c $< -o $@

clean:
	rm -f bmptool *.o core *~
