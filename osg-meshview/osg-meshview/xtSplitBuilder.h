#pragma once
#include "linearmathcom.h"
#include <vector>
#include <map>
#include <tuple>
#include "xtPrimitive.h"
#include "./trianglev2/triangleprimitive.h"

#define XT_DEBUG_PERCE_POINT 0;
#define XT_DEBUG_PLANAR_TRI_SET 1;

struct xtSegmentKey
{
	int pi0;
	int pi1;
};

// http://www.wuqifu.com/2012/01/07/c-stl-mapinsert%E5%B0%8F%E7%BB%93/
struct xtSegmentKeyComp
{
	bool operator() ( const xtSegmentKey &key0,  const xtSegmentKey &key1 )
	{
		return ( key0.pi0<key1.pi0 || ( (key0.pi0==key1.pi0) && key0.pi1<key1.pi1) );
	}
};

struct xtSegmentFaceK
{
	int pi0, pi1;
	int fi;
};

struct xtFaceFaceKey
{
	int f0, f1;
};

struct xtFaceFaceKeyComp
{
	bool operator() ( const xtFaceFaceKey &key0, const xtFaceFaceKey &key1 )
	{
		return ( key0.f0<key1.f0 || ( (key0.f0==key1.f0) && (key0.f1<key1.f1) ) );
	}
};


struct xtSegFaceComp
{
	bool operator() ( const xtSegmentFaceK &sfk0, const xtSegmentFaceK &sfk1 ) 
	{
		return ( 
			sfk0.pi0<sfk1.pi0 || 
			( (sfk0.pi0==sfk1.pi0) && sfk0.pi1<sfk1.pi1) ||
			( (sfk0.pi0==sfk1.pi0) && (sfk0.pi1==sfk1.pi1) && sfk0.fi<sfk1.fi));
	}
};

struct xtSegmentSlot
{
	int startIdx, endIdx;

	//====================================
	std::vector<xtVector3d *> pointOnSeg;
};

struct xtSegment
{
	xtVector3d *seg0;
	xtVector3d *seg1;
};

enum xtVertexRobustType
{
	NEW_SPLIT,
	ORI_VERTEXI,
	ORI_VERTEXJ,
};

struct xtVertexRobust
{
	xtVertexRobustType vtype;
	xtVector3d *v; // new split point on surf
	int idx;       // triangle's vertex idx
};

struct xtSegmentRobust
{
	xtVertexRobust v[2]; 
};

struct xtSurfaceSlot
{
	int idx;
	xtSegmentSlot *segs[3];
	std::vector<int> cflist;

	//====================================
	std::vector<xtVector3d *> pointsOnSurf;
	std::vector<xtVector3d *> pointsOnSurfVerbos;  // same as pointsOnSurf in case miss
	std::vector<xtSegment *> segsonsurf;
	//====================================
	// Handle the point is to close or site on the triangle's original vertex
	std::vector<xtSegmentRobust *> segrobust;

	std::vector<xtIndexTria3 > tris;  
	// pre 3 is the tirangle idx should be reference to the surface data
	// for 3- is the local segment data should reference the the pointsOnSurfVerbos
};

struct xtSegmentVertexPointerKey
{
	xtVector3d *vert;
	xtSegment *vbseg; // segment hold vert;
};

inline bool xtSegmentVertexPointerKeyComp(xtSegmentVertexPointerKey *key0, xtSegmentVertexPointerKey *key1)
{
	return (key0->vert-key1->vert)?true:false;
}

class xtOrderSegSuquence
{
public:
	~xtOrderSegSuquence();
	void OrderSequence(std::vector<xtSegment *> &segs) ;
	void OrderSequenceSlow( std::vector<xtSegment *> &segs );

private:
	std::vector<xtSegmentVertexPointerKey *> seq;
	std::vector<xtVector3d *> verts;
	std::vector<int> sequence;
};

struct xtPntSlotKeyComp
{
	bool operator()(xtVector3d *left, xtVector3d *right) 
	{
		return (left-right)<0;
	}
};

typedef std::map<xtVector3d *, int, xtPntSlotKeyComp> xtPntSlotMap;

class xtSegmentPointer2Index
{
public:
	void IndexPointer(std::vector<xtSegment *> &segps, 
		std::vector<xtVector3d *> &verts,
		std::vector<std::tuple<int,int>> &segis);
private:

};
// order
// new_split, vertexJ, vertexJ
struct xtPntRobustSlotKeyComp
{
	bool operator()( const  xtVertexRobust *left, const xtVertexRobust *right )
	{
		if ( left->vtype==right->vtype && right->vtype==NEW_SPLIT ) {
			return (left->v-right->v)>0;
		}
		if ( (left->vtype==ORI_VERTEXI||left->vtype==ORI_VERTEXJ) &&right->vtype==NEW_SPLIT ) {
			return true;
		}
		if ( left->vtype==NEW_SPLIT && (right->vtype==ORI_VERTEXI||right->vtype==ORI_VERTEXJ) ){
			return false;
		}
		if ( left->vtype!=NEW_SPLIT&&right->vtype!=NEW_SPLIT 
			&&left->vtype!=right->vtype ) {
			return left->vtype==ORI_VERTEXI;
		}
		if ( left->vtype==ORI_VERTEXI ) {
			return left->idx<right->idx;
		}
		if ( left->vtype==ORI_VERTEXJ ) {
			return left->idx<right->idx;
		}
		
	}
};

typedef std::map<xtVertexRobust *, int, xtPntRobustSlotKeyComp> xtPntRobustSlotMap;

class xtSegmentRobustPointer2Index
{
public:
	void IndexPointer(std::vector<xtSegmentRobust *> &segps, 
		std::vector<xtVertexRobust *> &verts,
		std::vector<std::tuple<int,int>> &segis);
private:

};

enum xtSurfaceCat
{
	XTSURFACEI,
	XTSURFACEJ,	
};



struct xtGeometrySurfaceDataS;
class xtCollisionEntity
{
	friend class xtSplitBuilder;
	

public:
	~xtCollisionEntity() {
		DestroyMem();
	}

	void InitializeCollisionEntity(xtGeometrySurfaceDataS *surf, std::vector<xtCollidePair> &pairs, xtSurfaceCat surfcat);
	void AddSplitSegmentToFace(const int fi, xtSegment *seg);
	void AddSplitSegmentRBToFace(const int fi, xtSegmentRobust *segrb);


private:
	void DestroyMem();
	xtSurfaceSlot *GetFaceSlotExit(int fi);
	xtSegmentSlot *GetOrCreateSeg(int i0, int i1);
	xtSegmentSlot *SearchSeg(int i0, int i1);

	std::vector<xtSurfaceSlot *> surfslot;
	std::vector<xtSegmentSlot *> segslot;
};

namespace osg
{
	class Geode;
};

namespace xtOctreeDisplayUtility
{
	osg::Geode * RenderSplitSegments(xtSplitBuilder *, xtColor color, float linewidth);
	osg::Geode *RenderSplitSegmentsWithCyliner(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
	osg::Geode *RednerSplitPntsAsSphere(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
	osg::Geode *RenderRaySegment(xtSplitBuilder *sb);
	osg::Geode *RenderPlanarTriSplitSegs(xtSplitBuilder *sb);
};

//typedef std::tuple<xtVector3d, xtVector3d> xtRaySegment;
// the above Eigen and stl conflicts
struct xtRaySegment
{
	xtVector3d start;
	xtVector3d end;
	xtVector3d oriend;
};

struct xtPlanarTriSegData
{
	std::vector<xtTriPnt2> verts2d;
	std::vector<xtSeg2WithMarker> segmarkerlist;
};

struct xtPlanarTriSegData3d
{
	std::vector<xtVector3d> verts;
	std::vector<xtSeg2WithMarker> segmarkerlist;
};

class xtCollisionEngine;
class xtSplitBuilder
{
	friend class xtInterferecenTest;
	// need add the namespace pay attention to it!
	friend osg::Geode * xtOctreeDisplayUtility::RenderSplitSegments(xtSplitBuilder *, xtColor color, float linewidth);
	friend osg::Geode * xtOctreeDisplayUtility::RenderSplitSegmentsWithCyliner(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
	friend osg::Geode * xtOctreeDisplayUtility::RednerSplitPntsAsSphere(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
	friend osg::Geode * xtOctreeDisplayUtility::RenderRaySegment(xtSplitBuilder *sb);
	friend osg::Geode * xtOctreeDisplayUtility::RenderPlanarTriSplitSegs(xtSplitBuilder *sb);
	typedef std::map<xtSegmentFaceK, xtVector3d *, xtSegFaceComp> xtSFMap;
	typedef std::map<xtFaceFaceKey, xtSegment *, xtFaceFaceKeyComp> xtFFMap;
public:
	xtSplitBuilder(void);
	~xtSplitBuilder(void);

	void SetCE(xtCollisionEngine *ce) ;

	void Split();

private:
	void ConstructSplitSegments();
	void ConstructSplitSegmentsRobust();
	void SplitPnt(xtCollisionEntity *psI, xtCollisionEntity *psJ, xtSFMap &sfmap, xtGeometrySurfaceDataS *surfI, xtGeometrySurfaceDataS *surfJ);
	void TessellateCollidedFace(xtCollisionEntity *ps, xtGeometrySurfaceDataS *surf );
	void TessellateCollidedFaceRobust( xtCollisionEntity *ps, xtCollisionEngine *ce, const int type);
	void FilterAdjacentPnts();
	void InitializeCollisionEntity();
	void DestroyMem();

private:
	static void ConcatenationPolyLine(std::vector<std::tuple<int,int>> &segonsurfindices, const int numofverts);

private:

	xtCollisionEngine *mCE;

	//======================================================================================
	// detail split triangle
	
	xtSFMap mSFMI;
	xtSFMap mSFMJ;
	xtFFMap mFFM;
	// This shared split points between surface I and surface J is in the world framwork
	// So Need a inverse tran
	std::vector<xtVector3d *> mSharedSplitPoints;
	std::vector<xtSegment *>  mSharedSplitSegList;

	xtCollisionEntity *mPSI; // PS mean partial collision surface
	xtCollisionEntity *mPSJ;

	//======================================================================================
	// debug the segment and triangle after the normal projection
	std::vector<xtPlanarTriSegData> mDebugPlannarTriSeg;
	std::vector<xtPlanarTriSegData3d> mDebugPlanarTriSeg3d;
	//======================================================================================
	// only for debug
#if XT_DEBUG_PERCE_POINT
	std::vector<xtRaySegment> mDebugedge;
#endif

};

//stl tuple
//http://msdn.microsoft.com/en-us/library/bb982771.aspx

