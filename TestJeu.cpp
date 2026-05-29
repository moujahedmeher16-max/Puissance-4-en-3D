/*
 ============================================================================
 Name        : TestJeu.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Compute Pi in MPI C++
 ============================================================================
 */
#include <math.h> 
#include <iostream>
#include "mpi.h" 
#include "arbitre.h"
#include "joueur.h"
using namespace std;
 
int main(int argc, char *argv[]) {
	int n, rank, size, i;
	bool joue=true;
    position p;
    int cpt=0;
	MPI::Init(argc, argv);
	size = MPI::COMM_WORLD.Get_size();
	rank = MPI::COMM_WORLD.Get_rank();

	if (rank == 2){
		arbitre AB;
	 while (joue!=false){
		p=AB.receiveCoup(0);
		cout << "AB receive from 0:"; p.affiche();
		cout << "AB send to 1:"; p.affiche();
		AB.sendCoup(p,1);

		p=AB.receiveCoup(1);
		cout << "AB receive from 1:" << p << endl;
		cout << "AB send to 0:" << p << endl;
		AB.sendCoup(p,0);
		cpt++;
		if (cpt==20) joue=false;
	 }
	}

	if (rank == 0){
		joueur A(rank);
	 while (joue!=false){
		 // Move of player A (to implement)
		 p=position::NumPosition(2*cpt); cpt++;
		    cout << "A send:" << p << endl;
			A.sendCoup(p,2);
			p=A.receiveCoup(2);
			cout << "A receive:" << p << endl;

			if (cpt==20) joue=false;
		}
	}

	if (rank == 1){
	   joueur B(rank);
	while (joue!=false){
			p=B.receiveCoup(2);
			cout << "B receive:"<< p << endl;;

			p=position::NumPosition(2*cpt+1); cpt++;
			cout << "B send:"<< p << endl;
			B.sendCoup(p,2);
			if (cpt==20) joue=false;
	}
		}

	MPI::Finalize();
	return 0;
}

