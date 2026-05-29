/*
 * position.cpp
 *
 *  Created on: 6 avr. 2026
 *      Author: dtromeur
 */

#include "position.h"
#include <iostream>
int position::n=4;
int position::m=5;
vector<char> position::alpha={'A','B','C','D'};
position::position(char h, int v, int l) {
	this->h=h;
	this->v=v;
	this->l=l;
}

position::~position() {
}

position::position(const position &o) {
	this->h=o.h;
	this->v=o.v;
	this->l=o.l;
}

position& position::operator=(const position &o) {
  if (this != &o){
	this->h=o.h;
	this->v=o.v;
	this->l=o.l;
  }
  return (*this);
}

position position::NumPosition(int i){
	int ii=(i%(n*m))%n;
	int jj=(i%(n*m))/n;
	int l=i/(n*m);
	//cout << i << "=" << ii << ":" << jj << ":" << l << endl;
	position P(alpha[ii],jj,l);
	return P;
}

int position::positionNumMPI(){
	int ii;
	ii= ((int) this->h - (int) 'A')*10+this->v;
	return ii;
}

char position::geth(){return this->h;}
int  position::getv(){return this->v;}
int  position::getl(){return this->l;}
char position::getalpha(int i){return position::alpha[i];}

void position::seth(char A){ this->h=A;}
void position::setv(int v){this->v=v;}

void position::affiche(){
	cout << "(" << this->h << this->v <<"):";
}

ostream & operator<<(ostream &sortie, position p){
	sortie << "(" << p.geth() << p.getv() <<"):";
	return sortie;
}
