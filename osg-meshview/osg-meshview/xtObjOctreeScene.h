#pragma once
#include "xtOctree.h"
#include "../buildpyramidGeometry/osgcommon.h"

class xtObjScene;
class xtOctreeAdapter;
class xtObjOctreeScene
{
public:
	xtObjOctreeScene(void);
	~xtObjOctreeScene(void);

	void LoadObjData( const char *basepath, const char *filepath );

	void BuildOctree(double resolution);
	int GetLargestDepth() { return mLargestDepth; };

	void GetDumpLevelNodeList(int level,std::vector<xtOctreeNode<int> *> &dumpnodelist) ;

	osg::Geode *CreatemOctreeScene(std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame=true, bool debugon=true);

	osg::Geode *SectionAxisX(double xplane);

	osg::Geode *SectionAxisY(double yplane);

	osg::Geode *SectionAxisZ(double zplane);

private:
	void BuildAdapterData(double resolution);

private:
	xtObjScene *mObjscene;

	xtOctree<int> *mOctree;
	xtOctreeAdapter *mOctreeDataA;
	int mLargestDepth;

	xtPnt3<int> mCubeCenter;
};

