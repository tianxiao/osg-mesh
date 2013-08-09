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
class xtSplitBuilder;
class xtCPMesh;
namespace osg
{
	class Geode;
	class Node;
	class Cylinder;
	class Group;
};

namespace xtOctreeDisplayUtility
{
	
	osg::Geode *CreatemOctreeScene(xtOctreeAdapter *mOctreeDataA, xtPnt3<int> &mCubeCenter, std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame=true, bool debugon=true);

	osg::Geode *CreateOctreeSection( double v, int axis, xtOctreeAdapter *mOctreeDataA, xtPnt3<int> &mCubeCenter, std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame=true, bool debugon=true);

	osg::Geode *DisplayxtGeometrySurfaceDataS(xtGeometrySurfaceDataS *surface, bool isWireFrame=true);

	osg::Node *DisplayxtGeometrySurfaceDataWithTranRot(xtGeometrySurfaceDataS *surface, bool isWireFrame=true);

	osg::Node *RenderCollided(xtGeometrySurfaceDataS *surfaceI, xtGeometrySurfaceDataS *surfaceJ, std::vector<xtCollidePair> &pairs);

	osg::Geode *RenderCollideList(xtGeometrySurfaceDataS *surface, std::vector<int> &index, xtColor color, bool isWireFrame=true);

	osg::Geode *RenderTriangle(xtTriangle *tri, xtColor color, bool wirefrme=true);

	osg::Geode *RenderSplitSegments(xtSplitBuilder *splitBuilder, xtColor color, float linewidth=4.0);

	osg::Geode *RednerSplitPntsAsSphere(xtSplitBuilder *splitBuilder, xtColor color, float linewidth=4.0);

	osg::Geode *RenderRaySegment(xtSplitBuilder *sb);

	osg::Geode *RenderSplitSegmentsWithCyliner(xtSplitBuilder *splitBuilder, xtColor color, float linewidth=4.0);

	osg::Geode *RenderSplitSegmentsWBODebug(xtSplitBuilder *splitBuilder, xtColor color, float linewidth=4.0);

	osg::Cylinder *CreateCyliner(xtVector3d &start, xtVector3d &end, double radius);

	osg::Geode *RenderArrow( xtVector3d &start, xtVector3d &end, double radius );

	osg::Geode *RenderPlanarTriSplitSegs(xtSplitBuilder *sb);

	osg::Group *RenderPlanarTris(xtSplitBuilder *sb);

	osg::Group *RenderCPMesh( xtCPMesh *mesh );
};
