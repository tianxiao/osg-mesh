#pragma once
struct xtTriPnt2 
{
	double p[2];
};

inline double SquareDistance( const xtTriPnt2 &p0, const xtTriPnt2 &p1 ) 
{
	double xd = p1.p[0]-p0.p[0];
	double yd = p1.p[1]-p1.p[1];
	return xd*xd+yd*yd;
}

struct xtSeg2WithMarker
{
	int seg[2];
	int marker;
};

struct xtTriIndexO
{
	int idx[3];
};