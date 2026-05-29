
CC = mpic++

exe : exe1 exe2 exe3

exe1 :  TestJeu.o position.o arbitre.o joueur.o
	${CC} -o exe1 *.o
exe2 :  TestJeu.o position.o arbitre.o joueur.o
	${CC} -o exe2 *.o
exe3 :  TestJeu.o position.o arbitre.o joueur.o
	${CC} -o exe3 *.o

%.o: %.cpp
	${CC} -c $< -o $@ 

clean :
	rm -f *.o exe

run :
	mpirun.openmpi -n 3 ./exe1 ./exe2 ./exe3


