#pragma once
#include <vector>

enum xtIntersectionPnt
{
	XT_BOUNDARY_PNT,
};

enum xtIntersectFace
{
	UN_TAGED,
	XT_INTERSECT_FACE,
	RED_FACE,
	GREEN_FACE,
};

// May consider add a XT_INTERSECT_FACE property to keep the original 
// surface before tessellated
// Reconsider if add the global triats and the local properites

struct xtOpenMeshW;
struct xtGeometrySurfaceDataS;
class xtAbosoluteVertexPool;
class xtCollisionEntity ;

void xtBuildRawMesh(xtGeometrySurfaceDataS *surf, xtOpenMeshW &data);

class xtCPMesh
{
public:
	xtCPMesh(void);
	~xtCPMesh(void);

	void LoadRawMesh(xtGeometrySurfaceDataS *surf);

	void LoadRawData(xtCollisionEntity *ce, xtAbosoluteVertexPool *abpool);

private:
	void BuildRawMesh();
	void Remesh();
	void TrackInterface();
	void TagFace();
	// for debug
	void DumpMeshOff();
	


private:
	xtGeometrySurfaceDataS *mSurf;
	xtAbosoluteVertexPool *mAbpool;
	xtCollisionEntity *mCE;
	xtOpenMeshW *mData;

	int mIstStartPntIdx;
	int mNumIstPnts;
	std::vector<unsigned int> mIstHalfedgeIdxList;
};

