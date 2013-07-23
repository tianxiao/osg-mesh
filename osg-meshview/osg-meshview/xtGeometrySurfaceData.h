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

struct xtGeometrySurfaceDataS
{
	std::vector<xtVector3d> verts;
	std::vector<xtIndexTria3> indices;
	std::vector<xtTriAdjacent> adjs;

	xtVector3d tran;
	xtMatrix3d rot;
};



