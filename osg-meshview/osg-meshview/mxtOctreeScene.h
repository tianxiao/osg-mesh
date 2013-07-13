#pragma once
#include "xtOctree.h"
#include "../buildpyramidGeometry/osgcommon.h"

class txFemSurf;
class xtOctreeAdapter;
class mxtOctreeScene
{
public:
	mxtOctreeScene(void);
	~mxtOctreeScene(void);

	void LodaFemSurfData();
	void SetResolution(double x) { this->mResolution = x; } ;

	osg::Geode *GetFemSurfGeom();

	void GetDumpLevelNodeList(int level,std::vector<xtOctreeNode<int> *> &dumpnodelist) ;

public:
	void BuildOctree();

	osg::Geode *CreatemOctreeScene(std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame=true, bool debugon=true);

private:
	txFemSurf *femsurf;

	xtOctreeAdapter *mOctreeDataA;
	xtOctree<int> *mOctree;
	int mLargestDepth;
	double mResolution;


	xtPnt3<int> mCubeCenter;
	// cache data recevei the dump level
	//std::vector<xtOctreeNode<int> *> mDumpLevelNodelist;
};



