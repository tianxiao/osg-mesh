#pragma once
#include <vector>
#include <queue>
#include "xtPrimitive.h"

struct xtRtreeImp;
struct xtGeometrySurfaceDataS;



class xtCollisionEngine
{
public:
	xtCollisionEngine(void);
	~xtCollisionEngine(void);

	void SetSurfIJ(xtGeometrySurfaceDataS *surfI, xtGeometrySurfaceDataS *surfJ) 
	{
		this->mSurfI = surfI;
		this->mSurfJ = surfJ;

		BuildRTree();
	}

	bool Collide();

	std::vector<xtCollidePair> &GetColliedPairs() { return mCollide; };

private:
	void BuildSurfBBOX();
	void BuildRTree();

	bool TriTriCollision(int i, int j);

private:
	xtRtreeImp *mT;
	xtGeometrySurfaceDataS *mSurfI;
	xtGeometrySurfaceDataS *mSurfJ;

	std::vector<xtBBOX> mIBBoxList;
	std::vector<xtBBOX> mJBBoxList;

	std::vector<xtCollidePair> mCollide;

};

