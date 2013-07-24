#pragma once
#include "linearmathcom.h"
#include <vector>
#include "xtPrimitive.h"

class xtGeometrySurfaceData
{
public:
	xtGeometrySurfaceData(void);
	~xtGeometrySurfaceData(void);
};

struct xtTriAdjacent
{
	int a, b, c;
};

// tria surface
struct xtGeometrySurfaceDataS
{
	std::vector<xtVector3d> verts;
	std::vector<xtIndexTria3> indices;
	std::vector<xtTriAdjacent> adjs;

	xtVector3d tran;
	xtMatrix3d rot;
};


class xtPointsTria
{
public:
	xtPointsTria(xtIndexTria3 &tria, xtGeometrySurfaceDataS *surf)
	{
		pa = surf->verts[tria.a[0]];
		pb = surf->verts[tria.a[1]];
		pc = surf->verts[tria.a[2]];
		
		pa = surf->tran + surf->rot*pa;
		pb = surf->tran + surf->rot*pb;
		pc = surf->tran + surf->rot*pc;
	}

	xtVector3d Normal() {
		xtVector3d pac = pa-pc;
		xtVector3d pbc = pb-pc;
		xtVector3d normal = pac.cross(pbc);
		normal.normalize();
		return normal;
	}

	xtVector3d pa, pb, pc;
};

// why I add the static will cause a compile time error
// declared without define?
void BuildSurfaceBBox(xtGeometrySurfaceDataS *surf, std::vector<xtBBOX> &bbox);



