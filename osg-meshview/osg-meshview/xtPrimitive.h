#pragma once
#include "linearmathcom.h"

class xtIndexTria3
{
public:
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

