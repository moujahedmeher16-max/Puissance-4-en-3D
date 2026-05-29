/*
 * joueur.h
 *
 *  Created on: 29 avr. 2026
 *      Author: dtromeur
 */

#ifndef JOUEUR_H_
#define JOUEUR_H_
#include <iostream>
#include <list>
#include "position.h"
#include "mpi.h"
using namespace std;
class joueur {
	/**
	 * \class joueur implements the MPI communication between the player
	 * and the referee who have the MPI rank=2
	 * The two main methods are:
	 *  receiveCoup which return from the referee the opponent move in a position instance
	 *  sendCoup which send to the referee the player move
	 */
	int me;      /// rank number in the MPI communicator
    int color;   /// color of the player 0 or 1
public:
	joueur(int color);
	virtual ~joueur();
	joueur(const joueur &other);
	joueur& operator=(const joueur &other);


	position receiveCoup( int =2);
	/**
	 *\fn receives the opponent move from the referee
	 *    the move is given as a position instance
	 */
	void sendCoup(position, int =2);
	/**
     * \fn sends the player move to the referee
     *    the move is given as a position instance
	 */
};

#endif /* JOUEUR_H_ */
