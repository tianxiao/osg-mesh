#pragma once
#include "linearmathcom.h"

class xtIndexTria3
{
public:
	xtIndexTria3() 
	{
		a[0] = -1;
		a[1] = -1;
		a[2] = -1;
	}
	xtIndexTria3(int a, int b, int c) {
		this->a[0] = a;
		this->a[1] = b;
		this->a[2] = c;
	}

	int a[3];
};



class xtIndexCquad4
{
public:
	xtIndexCquad4(int a, int b, int c, int d) {
		this->a[0] = a;
		this->a[1] = b;
		this->a[2] = c;
		this->a[3] = d;
	}

	int a[4];
};

struct xtEdge
{
	int a, b;
};

struct xtBBOX
{
	xtVector3d min;
	xtVector3d max;

	void TranRot(xtVector3d &tran, xtMatrix3d &rot)
	{
		min = tran + rot*min;
		max = tran + rot*max;
	}
};

struct xtCollidePair
{
	int i;
	int j;
};

struct xtColor
{
	xtColor(float r, float g, float b, float alpha)
		:r(r),g(g),b(b),alpha(alpha) {};
	float r,g,b,alpha;
};

class xtTriangle
{
public:
	xtTriangle(){};
	xtTriangle(xtVector3d &a, xtVector3d &b, xtVector3d &c) 
		: pa(a), pb(b), pc(c) {};

	xtVector3d TriNormal() 
	{
		xtVector3d pba = pb - pa;
		xtVector3d pca = pc - pa;
		xtVector3d normal = pba.cross(pca);
		normal.normalize();
		return normal;
	}

	void TranRot(xtVector3d &tran, xtMatrix3d &rot)
	{
		pa = tran + rot*pa;
		pb = tran + rot*pb;
		pc = tran + rot*pc;
	}

	xtVector3d *Intersect( xtVector3d &p0, xtVector3d &p1 );

	xtVector3d pa;
	xtVector3d pb;
	xtVector3d pc;
};



