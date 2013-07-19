#pragma once
#include "Vec2.h"
#include "Vec3.h"
#include <vector>
#include "xtPrimitive.h"

class xtWRLData
{
public:
	xtWRLData(void);
	~xtWRLData(void);

};

struct xtWRLDataS
{
	std::vector<Vec3> mVertices;
	std::vector<xtIndexTria3> mTris;
	std::vector<xtIndexCquad4> mQuads;
	std::vector<Vec2> mTextureCoords;
};

