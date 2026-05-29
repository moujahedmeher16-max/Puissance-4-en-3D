/*
 * joueur.cpp
 *
 *  Created on: 29 avr. 2026
 *      Author: dtromeur
 */

#include "joueur.h"

joueur::joueur(int color) {
	me = MPI::COMM_WORLD.Get_rank();
    this->color=color;
}

joueur::~joueur() {
}

joueur::joueur(const joueur &o) {
    this->color=o.color;

}

joueur& joueur::operator=(const joueur &o) {
    this->color=o.color;
	return *this;
}
/*

bool joueur::jejoue(position p){
	int ii;
    ii = p.PositionNum();
int ier=MPI_Send(&ii,1, MPI::INT,(me+1)%2, 100,MPI::COMM_WORLD);
	return false;
}

position joueur::tujoues(){
	int ii;
	position p;
	MPI_Status status;
int ier=MPI_Recv(&ii,1, MPI::INT,(me+1)%2, 100,MPI::COMM_WORLD, &status);
     p.setalpha(p.alpha[ii/10]);
     p.setv(ii%10);
return p;
}

*/

position joueur::receiveCoup( int rank){
	int ii,ier;
	position p;
	MPI_Status status;
    ier=MPI_Recv(&ii,1, MPI::INT,rank, 100,MPI::COMM_WORLD, &status);
     p.seth(p.getalpha(ii/10));
     p.setv(ii%10);
     return p;
}

void joueur::sendCoup(position p, int rank){
	int ii;
    ii = p.positionNumMPI();
    int ier=MPI_Send(&ii,1, MPI::INT,rank, 100,MPI::COMM_WORLD);
}
