/*
 * arbitre.cpp
 *
 *  Created on: 30 avr. 2026
 *      Author: dtromeur
 */

#include "arbitre.h"

arbitre::arbitre() {

}

arbitre::~arbitre() {
}

arbitre::arbitre(const arbitre &o) {
this->partie=o.partie;
}

arbitre& arbitre::operator=(const arbitre &o) {
	if (this != &o){

		this->partie=o.partie;
	}
	return (*this);

}


position arbitre::receiveCoup( int rank){
	int ii;
	position p;
	MPI_Status status;
int ier=MPI_Recv(&ii,1, MPI::INT,rank, 100,MPI::COMM_WORLD, &status);
     p.seth(p.getalpha(ii/10));
     p.setv(ii%10);
     return p;
}
void arbitre::sendCoup(position p, int rank){

	int ii;
    ii = p.positionNumMPI();
    int ier=MPI_Send(&ii,1, MPI::INT,rank, 100,MPI::COMM_WORLD);
}
