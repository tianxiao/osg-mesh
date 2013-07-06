#pragma once
//#include "importexport.h"
#include <math.h>

template<class T>
class xtVec3d
{
public:
	xtVec3d(void) {};
	xtVec3d(T x, T y, T z) {
		v[0] = x, v[1] = y, v[2] = z;
	}

	~xtVec3d(void) {};

	void CrossProduct(const xtVec3d &r, xtVec3d &out) {
		out.v[0] = v[1]*r.v[2] - v[2]*r.v[1];
		out.v[1] = v[2]*r.v[0] - v[0]*r.v[2];
		out.v[2] = v[0]*r.v[1] - v[1]*r.v[0];
	}

	void Zero() {
		for ( int i=0; i<3; ++i ) {
			v[i] = 0.0;
		}
	}

	void Normal() {
		double squareX = v[0]*v[0];
		double squareY = v[1]*v[1];
		double squareZ = v[2]*v[2];
		double len = sqrt(squareX+squareY+squareZ);
		v[0]/=len;
		v[1]/=len;
		v[2]/=len;
	}

	void Scale(const double s) {
		for ( int i=0; i<3; ++i ) {
			v[i] = v[i]/s;
		}
	}

	void FOOL();

	xtVec3d operator-(const xtVec3d &r) {
		xtVec3d rlt;
		for ( int i=0; i<3; ++i ) {
			rlt.v[i] = v[i] - r.v[i];
		}
		return rlt;
	}

	xtVec3d operator+(const xtVec3d &r) {
		xtVec3d rlt;
		for ( int i=0; i<3; ++i ) {
			rlt.v[i] = v[i] + r.v[i];
		}
		return rlt;
	}

	xtVec3d operator/(const double s) {
		xtVec3d rlt;
		for ( int i=0; i<3; ++i ) {
			rlt[i] = v[i]/s;
		}
	}

public:
	T v[3];
};

typedef xtVec3d<float> xtVec3df;

