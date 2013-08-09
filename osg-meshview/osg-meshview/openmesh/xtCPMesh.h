#pragma once
#include <vector>



// May consider add a XT_INTERSECT_FACE property to keep the original 
// surface before tessellated
// Reconsider if add the global triats and the local properites

struct xtOpenMeshW;
struct xtGeometrySurfaceDataS;
class xtAbosoluteVertexPool;
class xtCollisionEntity ;

void xtBuildRawMesh(xtGeometrySurfaceDataS *surf, xtOpenMeshW &data);

namespace osg
{
	class Group;
};

class xtCPMesh;
namespace xtOctreeDisplayUtility
{
	osg::Group *RenderCPMesh( xtCPMesh *mesh );
}

namespace xtRenderUtil
{
	osg::Group *RenderCPMesh( xtCPMesh *mesh );

};


class xtCPMesh
{
	friend osg::Group *xtRenderUtil::RenderCPMesh( xtCPMesh *mesh );
	//friend osg::Group *RenderCPMesh( xtCPMesh *mesh );
public:
	xtCPMesh(void);
	~xtCPMesh(void);

	void LoadRawMesh(xtGeometrySurfaceDataS *surf);

	void LoadRawData(xtCollisionEntity *ce, xtAbosoluteVertexPool *abpool);

	void Union (xtCPMesh &om );
	void Difference( xtCPMesh &om );
	void Intersection( xtCPMesh &om );

private:
	void BuildRawMesh();
	void AttachSSToMeshFaceTraitsOrder();
	void MarkIstPointType();
	void Remesh();
	void TrackInterface();
	// double check and accelerate by the face neighborhood info
	void TrackInterface2();
	void TagIstBoundaryHalfedge();
	void TagIstBoundaryHalfedge2();
	void TagFace();
	void TagFace2();
	bool IsIstEdgeFace( int fhandleidx ) ;
	bool IsTwoFaceSeperatedByIstEdge(int f0, int f1);
	// this red green is truly have the overlap meanings
	void ReMarkRedGreenTria();
	void ReReMarkRedGreenTria();

	// boolean operation 
	void ReSetmVHandleidxs();
	void MatchBoundary( xtCPMesh &om );
	void MarkNewAssignedVertex( xtCPMesh &om );
	void TagRedVertex();
	void TagGreenVertex();
	void AddNewDifferenceFace( xtCPMesh &om );
	void DeleteSelfGreenTrias();
	void DeleteSelfRedTrias();

	// for debug
	void DumpMeshOff();
	void DumpTaggedRed();
	
	


private:
	xtGeometrySurfaceDataS *mSurf;
	xtAbosoluteVertexPool *mAbpool;
	xtCollisionEntity *mCE;
	xtOpenMeshW *mData;

	int mIstStartPntIdx;
	int mNumIstPnts;
	std::vector<unsigned int> mIstHalfedgeIdxList;
	std::vector<unsigned int> mIstVertHandleIdxList;
	// Cache the intersect boundary order
	// cache the original split point's reorder vIdInPool index
	std::vector<unsigned int> mIstOrder;   
	// cache the addin split point's 
	std::vector<unsigned int> mVHandleidxs;
	std::vector<void *> mVertexList;

	// only for debug
	std::vector<unsigned int> mHEBs;
	std::vector<unsigned int> mFacepro;
};


class xtBooleanProblem
{
	void SetModel()
	{
		
	}
};

