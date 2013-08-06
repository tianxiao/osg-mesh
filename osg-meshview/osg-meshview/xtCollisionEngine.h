#pragma once
#include <vector>
#include <queue>
#include <map>
#include "xtPrimitive.h"

struct xtRtreeImp;
struct xtGeometrySurfaceDataS;

#define RTREE_DEBUG_RESULT 0



class xtCollisionEngine
{
	friend class xtInterferecenTest;
	friend class xtSplitBuilder;
public:
	xtCollisionEngine(void);
	~xtCollisionEngine(void);

	xtGeometrySurfaceDataS *AccessSurfI() { return mSurfI; };
	xtGeometrySurfaceDataS *AccessSurfJ() { return mSurfJ; };

	void SetSurfIJ(xtGeometrySurfaceDataS *surfI, xtGeometrySurfaceDataS *surfJ) 
	{
		this->mSurfI = surfI;
		this->mSurfJ = surfJ;

		BuildRTree();
	}

	bool Collide();

	std::vector<xtCollidePair> &GetColliedPairs() { return mCollide; };

	xtGeometrySurfaceDataS *GetSurfI() { return mSurfI; };
	xtGeometrySurfaceDataS *GetSurfJ() { return mSurfJ; };

private:
	void BuildSurfBBOX();
	void BuildRTree();
	void SetM4J2I();
	void SetM4I2J();

	bool TriTriCollision(int i, int j);

private:
	xtRtreeImp *mT;
	xtGeometrySurfaceDataS *mSurfI;
	xtGeometrySurfaceDataS *mSurfJ;

	std::vector<xtBBOX> mIBBoxList;
	std::vector<xtBBOX> mJBBoxList;

	std::vector<xtCollidePair> mCollide;

	xtVector3d tranj2i; 
	xtMatrix3d rotj2i; 

	xtVector3d trani2j; 
	xtMatrix3d roti2j; 


	// For debug
#if RTREE_DEBUG_RESULT
	std::vector<int> tempJlist;
	std::vector<std::vector<int>> tempIBoxList;
#endif

};

