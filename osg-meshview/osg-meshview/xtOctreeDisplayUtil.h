#pragma once
#include "xtOctree.h"
#include "../buildpyramidGeometry/osgcommon.h"
class xtOctreeAdapter;
class xtOctreeDisplayUtil
{
public:
	xtOctreeDisplayUtil(void);
	~xtOctreeDisplayUtil(void);
};

namespace xtOctreeDisplayUtility
{

	osg::Geode *CreatemOctreeScene(xtOctreeAdapter *mOctreeDataA, xtPnt3<int> &mCubeCenter, std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame=true, bool debugon=true);

};
