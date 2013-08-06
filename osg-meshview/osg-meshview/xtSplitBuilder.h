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

struct xtSplitVertex;
struct xtSegmentSlot
{
	int startIdx, endIdx;

	//====================================
	std::vector<xtVector3d *> pointOnSeg;
	// like the xtSurfaceSlot change the geometry point to the topology point
	std::vector<xtSplitVertex *> vertsOnSeg;
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

enum xtVertexWBOType
{
	ON_BOUNDARY,
	ON_SURFACE,
	ON_SURFACE_V,  // ot
	ON_V,
};

// But after I add the absolute point 
enum xtAbosoluteVertexType
{
	// ON_SURFACE is degenerate case in the intersection stage
	// while ON_SPLIT is the normal edge-tri intersection point
	ON_SURFACE_I,
	ON_SRUFACE_J,
	ON_SPLIT,
};

// try use the interface 
struct xtAbosoluteVertex
{
	xtAbosoluteVertexType type;
	xtVector3d *splitpnt;
	int idx;

	int vIdInPool;

	void InitVert( xtAbosoluteVertexType type, xtVector3d *splitpnt, int idx ) 
	{
		this->type = type;
		this->splitpnt = splitpnt;
		this->idx = idx;
	}

	bool operator==(xtAbosoluteVertex &v)
	{
		return type==v.type && splitpnt==v.splitpnt && idx==v.idx;
	}
};

class xtAbosoluteVertexPool
{
	friend class xtCPMesh;
public:
	~xtAbosoluteVertexPool() 
	{
		for ( size_t i=0; i<mAbverts.size(); ++i ) {
			delete mAbverts[i];
			mAbverts[i] = NULL;
		}
	};

	xtAbosoluteVertex *AddVert( xtAbosoluteVertexType type, xtVector3d *splitpnt, int idx )
	{
		xtAbosoluteVertex *rlt = FindVert( type, splitpnt, idx );
		if ( !rlt ) {
			rlt = new xtAbosoluteVertex;
			rlt->InitVert( type, splitpnt, idx );
			mAbverts.push_back(rlt);
		}
		return rlt;
	};

	void NormalizeIndexInPool()
	{
		for ( size_t i=0; i<mAbverts.size(); ++i ) {
			mAbverts[i]->vIdInPool = i;
		}
	};

private:
	xtAbosoluteVertex *FindVert(xtAbosoluteVertexType type, xtVector3d *splitpnt, int idx)
	{
		xtAbosoluteVertex *rlt = NULL;
		for ( size_t i=0; i<mAbverts.size(); ++i ) {
			xtAbosoluteVertex *c = mAbverts[i];
			if ( c->type == type &&
				c->splitpnt == splitpnt &&
				c->idx == idx ) {
				rlt = mAbverts[i];
			}
		}
		return rlt;
	}

private:
	std::vector<xtAbosoluteVertex *> mAbverts;
};

//=======================Experimental Codes=============================
// Next TODO!
// create a factory method hold all those points and expose an API
// for easy get and use those different point
class xtAbstractVertex
{
public:
	xtAbstractVertex() {};
	virtual ~xtAbstractVertex() =0{};
};

class xtMeshVertex :public xtAbstractVertex
{
public:
	xtMeshVertex() {};
	~xtMeshVertex() {};

private:
	xtAbosoluteVertexType type;  // on surface I or on surface J
	int idx;
};

class xtSplitPVertex : public xtAbstractVertex
{
public:
	xtSplitPVertex() {};
	~xtSplitPVertex() {};
private:
	xtVector3d *splitpnt;
};
//======================================================================


// This WBO is not self contained 
// It is at the surface slot context
// and use the surface data.

struct xtVertexWBO
{
	// Add the point on triangle segment field for convenient 
	// tessellation
	xtVertexWBOType type;
	xtVector3d *v;
	int idx;
	// abv is for unifying all the new generated point during the 
	// intersection.
	xtAbosoluteVertex *abv;

	void SetWBO(xtVertexWBOType type, xtVector3d *v, int idx ) 
	{
		this->type = type;
		this->v = v;
		this->idx =idx;
	};

	bool operator==(const xtVertexWBO &v) {
		return type==v.type&&this->v==v.v&&idx==v.idx;
	}
};

inline void SetWBOAbosoluteVertex( xtVertexWBO &wbo, xtAbosoluteVertexType type, xtVector3d *splitpnt, int idx, xtAbosoluteVertexPool &abpool)
{
	wbo.abv = abpool.AddVert( type, splitpnt, idx );
};

struct xtSegmentWBO
{
	xtVertexWBO v[2];
	xtVertexWBO *pv[2];
};

struct FindWBOComp
{
	FindWBOComp(xtVertexWBO *c):self(c){};
	bool operator()(xtVertexWBO *v)
	{
		return *v==*self;
	};

private:
	xtVertexWBO *self;
};

class xtSegmentWBOPool
{
public:
	~xtSegmentWBOPool() 
	{
		for ( size_t i=0; i<verts.size(); ++i ) {
			delete verts[i];
			verts[i] = NULL;
		}
	}

	xtVertexWBO *AddVert(xtVertexWBOType type, xtVector3d *v, int idx )
	{
		xtVertexWBO *rlt = FindVert( type, v, idx );
		if ( !rlt ) {
			rlt = new xtVertexWBO;
			rlt->SetWBO( type, v, idx );
		}
		return rlt;
	}

private:

	xtVertexWBO *FindVert( xtVertexWBOType type, xtVector3d *v, int idx )
	{
		xtVertexWBO wbo;
		wbo.SetWBO(type, v, idx);
		return FindVert( &wbo );
	}

	xtVertexWBO *FindVert(xtVertexWBO *x)
	{
		FindWBOComp findcomp(x);
		xtVertexWBO *rlt = NULL;
		std::vector<xtVertexWBO *>::iterator findit = 
			std::find_if( verts.begin(), verts.end(), findcomp );
		if ( findit!=verts.end() ) {
			return *findit;
		}
		return rlt;
	}

private:
	std::vector<xtVertexWBO *> verts;
};


inline xtSegmentWBO *CreateSeg(
	xtVertexWBOType type0, xtVector3d *v0, int idx0,
	xtVertexWBOType type1, xtVector3d *v1, int idx1,
	xtSegmentWBOPool &wbopool
	)
{
	xtSegmentWBO *seg = new xtSegmentWBO;
	seg->pv[0] = wbopool.AddVert( type0, v0, idx0 );
	seg->pv[1] = wbopool.AddVert( type1, v1, idx1 );
	return seg;
};

struct xtSurfaceSlot
{
	int idx;
	xtSegmentSlot *segs[3];
	std::vector<int> cflist;

	//====================================
	std::vector<xtVector3d *> pointsOnSurf;
	// modified pointsOnSurf
	std::vector<xtSplitVertex *> vertsOnSurf;
	std::vector<xtVector3d *> pointsOnSurfVerbos;  // same as pointsOnSurf in case miss
	std::vector<xtSegment *> segsonsurf;
	//====================================
	// Handle the point is to close or site on the triangle's original vertex
	std::vector<xtSegmentRobust *> segrobust;

	//====================================
	std::vector<xtSegmentWBO *> segwbo;

	// cache the 3~n nodes the 0~2 nodes is the triangle's nodes
	std::vector<xtAbosoluteVertex *> aftertrinodeslist;
	// output split triangle
	std::vector<xtIndexTria3 > tris;  
	// pre 3 is the tirangle idx should be reference to the surface data
	// for 3- is the local segment data should reference the the pointsOnSurfVerbos
};

static void OrderxtSegmentWBOList( std::vector<xtSegmentWBO *> &segwbo );

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

namespace osg
{
	class Geode;
	class Group;
};
class xtSplitBuilder;

namespace xtOctreeDisplayUtility
{
	osg::Geode *RenderSplitSegments(xtSplitBuilder *, xtColor color, float linewidth);
	osg::Geode *RenderSplitSegmentsWithCyliner(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
	osg::Geode *RednerSplitPntsAsSphere(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
	osg::Geode *RenderRaySegment(xtSplitBuilder *sb);
	osg::Geode *RenderPlanarTriSplitSegs(xtSplitBuilder *sb);
	osg::Group *RenderPlanarTris(xtSplitBuilder *sb);
	osg::Geode *RenderSplitSegmentsWBODebug(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
};


struct xtGeometrySurfaceDataS;
class xtCollisionEntity
{
	friend class xtCPMesh;
	friend class xtSplitBuilder;
	friend osg::Geode * xtOctreeDisplayUtility::RenderSplitSegmentsWBODebug(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
	// friends cannot propagate, A is B's friends B is C's friends, A and C may not be friends. can not access each other.
public:
	~xtCollisionEntity() {
		DestroyMem();
	}

	std::vector<xtSurfaceSlot *> &AccessSurfaceSlotList() { return surfslot; };

	void InitializeCollisionEntity(xtGeometrySurfaceDataS *surf, std::vector<xtCollidePair> &pairs, xtSurfaceCat surfcat);
	void AddSplitSegmentToFace(const int fi, xtSegment *seg);
	void AddSplitSegmentRBToFace(const int fi, xtSegmentRobust *segrb);
	void AddSegWBOToFace(const int fi, xtSegmentWBO *seg);


private:
	void DestroyMem();
	xtSurfaceSlot *GetFaceSlotExit(int fi);
	xtSegmentSlot *GetOrCreateSeg(int i0, int i1);
	xtSegmentSlot *SearchSeg(int i0, int i1);

	std::vector<xtSurfaceSlot *> surfslot;
	std::vector<xtSegmentSlot *> segslot;
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

struct xtPlanarTri
{
	std::vector<xtIndexTria3> tris;
	std::vector<xtVector3d> verts;
};

struct xtSplitVertex
{
	// actually the point and the following three can be combined to a union
	// or use the vritual funciton to bind
	xtVector3d point;
	bool IsDegenerate;
	int idx;
	xtSurfaceCat ftype;
};

// describe a degenerate case
struct xtDegeneratePoint
{
	int idx;          // 0 or 1
	int edge[2];
	int ofaceId;
	double t;
	xtVector3d dir;
};


class xtCollisionEngine;
class xtCPMesh;
class xtSplitBuilder
{
	friend class xtInterferecenTest;
	// need add the namespace pay attention to it!
	friend osg::Geode * xtOctreeDisplayUtility::RenderSplitSegments(xtSplitBuilder *, xtColor color, float linewidth);
	friend osg::Geode * xtOctreeDisplayUtility::RenderSplitSegmentsWithCyliner(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
	friend osg::Geode * xtOctreeDisplayUtility::RednerSplitPntsAsSphere(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
	friend osg::Geode * xtOctreeDisplayUtility::RenderRaySegment(xtSplitBuilder *sb);
	friend osg::Geode * xtOctreeDisplayUtility::RenderPlanarTriSplitSegs(xtSplitBuilder *sb);
	friend osg::Group * xtOctreeDisplayUtility::RenderPlanarTris(xtSplitBuilder *sb);
	friend osg::Geode * xtOctreeDisplayUtility::RenderSplitSegmentsWBODebug(xtSplitBuilder *splitBuilder, xtColor color, float linewidth/*=4.0*/);
	typedef std::map<xtSegmentFaceK, xtVector3d *, xtSegFaceComp> xtSFMap;
	typedef std::map<xtSegmentFaceK, xtSplitVertex *, xtSegFaceComp> xtSFVMap;
	typedef std::map<xtFaceFaceKey, xtSegment *, xtFaceFaceKeyComp> xtFFMap;
	typedef std::map<int, xtDegeneratePoint* > xtDePointMap;
public:
	xtSplitBuilder(void);
	~xtSplitBuilder(void);

	void SetCE(xtCollisionEngine *ce) ;

	void Split();

private:
	void ConstructSplitSegments();
	void ConstructSplitSegmentsRobust();
	void ConstructSplitSegmentsWithEndPoint();
	void SplitPnt(xtCollisionEntity *psI, xtCollisionEntity *psJ, xtSFMap &sfmap, xtGeometrySurfaceDataS *surfI, xtGeometrySurfaceDataS *surfJ);
	void ConstructSplitVerts(xtCollisionEntity *psI, xtCollisionEntity *psJ, xtSFVMap &sfvmap, xtGeometrySurfaceDataS *surfI, xtGeometrySurfaceDataS *surfJ);
	void TessellateCollidedFace(xtCollisionEntity *ps, xtGeometrySurfaceDataS *surf );
	void TessellateCollidedFaceRobust( xtCollisionEntity *ps, xtCollisionEngine *ce, const int type);
	void TessellateFaceWithWBO( xtCollisionEntity *ps, xtGeometrySurfaceDataS *surf0, xtGeometrySurfaceDataS *surf1 );

	void BuildMeshConnectivity();

	void InitializeCollisionEntity();
	void DestroyMem();

private:
	static void ConcatenationPolyLine(std::vector<std::tuple<int,int>> &segonsurfindices, const int numofverts);
	static void FilterAdjacentPnts(std::vector<std::tuple<int,int>> &segonsurfindices, std::vector<xtVertexRobust *> &segonsurfverts );
	static void FilterAdjacentVerts2d(std::vector<xtTriPnt2> &verts2d, std::vector<xtSeg2WithMarker> &segmarkerlist);

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

	// Add new shared split points add the degenerate case in it 
	// like the intersection point is sit on the surface or ? to close to the 
	// element surface;
	std::vector<xtSplitVertex *> mSharedSplitVerts;
	xtSFVMap mSFMVI;
	xtSFVMap mSFMVJ;

	// like the about store the shared field in the interface
	// This pool store the shared edge-triangle surface 
	// For unify the verts
	xtAbosoluteVertexPool mAbpool;


	// Add degenerate case
	//xtDePointMap mDPI;
	//xtDePointMap mDPJ;
	//std::vector<xtDegeneratePoint *> mDegenerateI;
	//std::vector<xtDegeneratePoint *> mDegenerateJ;


	xtCollisionEntity *mPSI; // PS mean partial collision surface
	xtCollisionEntity *mPSJ;

	//======================================================================================
	// debug the segment and triangle after the normal projection
	std::vector<xtPlanarTriSegData> mDebugPlannarTriSeg;
	std::vector<xtPlanarTriSegData3d> mDebugPlanarTriSeg3d;
	std::vector<xtPlanarTri> mDebugPlanarTris;
	//======================================================================================
	// only for debug
#if XT_DEBUG_PERCE_POINT
	std::vector<xtRaySegment> mDebugedge;
#endif

};

//stl tuple
//http://msdn.microsoft.com/en-us/library/bb982771.aspx

