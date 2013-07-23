#pragma once
#include "xtOctree.h"
#include "xtPrimitive.h"

class xtOctreeAdapter;
class xtOctreeDisplayUtil
{
public:
	xtOctreeDisplayUtil(void);
	~xtOctreeDisplayUtil(void);
};

struct xtGeometrySurfaceDataS;
namespace osg
{
	class Geode;
	class Node;
};
namespace xtOctreeDisplayUtility
{
	
	osg::Geode *CreatemOctreeScene(xtOctreeAdapter *mOctreeDataA, xtPnt3<int> &mCubeCenter, std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame=true, bool debugon=true);

	osg::Geode *CreateOctreeSection( double v, int axis, xtOctreeAdapter *mOctreeDataA, xtPnt3<int> &mCubeCenter, std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame=true, bool debugon=true);

	osg::Geode *DisplayxtGeometrySurfaceDataS(xtGeometrySurfaceDataS *surface, bool isWireFrame=true);

	osg::Node *DisplayxtGeometrySurfaceDataWithTranRot(xtGeometrySurfaceDataS *surface, bool isWireFrame=true);

	osg::Node *RenderCollided(xtGeometrySurfaceDataS *surfaceI, xtGeometrySurfaceDataS *surfaceJ, std::vector<xtCollidePair> &pairs);

	osg::Geode *RenderCollideList(xtGeometrySurfaceDataS *surface, std::vector<int> &index, xtColor color, bool isWireFrame=true);
};
