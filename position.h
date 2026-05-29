/*
 * position.h
 *
 *  Created on: 6 avr. 2026
 *      Author: dtromeur
 */

#ifndef POSITION_H_
#define POSITION_H_
#include <vector>
#include <map>
#include <string>
#include <iostream>
using namespace std;
class position {
protected:
	static int n;
	static int m;
	static vector<char> alpha;
	char h;
	int  v;
	int  l;
public:
	position(char h ='A', int v =0, int l =0);
	position(const position &o);
	position& operator=(const position &o);
	virtual ~position();

	static position NumPosition(int);
	/**
	 * \fn returns the position associated to
	 * the 1D position number between 0..n*m*l of the board
	 * this method is static (independent of an position instance)
	 */
	char geth();
	int getv();
	int getl();
	char getalpha(int);

	void seth(char A);
	void setv(int v);

	void affiche();
	int positionNumMPI();
	/**
	 * \fn return the MPI coding number associated
	 *  to the calling position instance
	 *  as the move (2D (char,int) is coded as an int number
	 */
	friend ostream & operator<<(ostream & , position p);

};

#endif /* POSITION_H_ */
