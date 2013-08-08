#pragma once
class xtOpenmeshHeader
{
public:
	xtOpenmeshHeader(void);
	~xtOpenmeshHeader(void);
};

//#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
// using the trimesh connetivity to get more efficiency
#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <vector>
#include <algorithm>
#include "xtVertFaceEnum.h"

typedef OpenMesh::TriMesh_ArrayKernelT<> txOPMesh;

struct xtSurfaceSlot;
struct xtTraits :public OpenMesh::DefaultTraits
{
	//typedef OpenMesh::Vec3d Point;
	VertexAttributes(OpenMesh::Attributes::Status);
	FaceAttributes(OpenMesh::Attributes::Status);
	EdgeAttributes(OpenMesh::Attributes::Status);

	VertexTraits
	{
	private:
		xtIntersectionPnt type;
		
	public:
		VertexT():type( UN_TAGEDPNT ) {};

		xtIntersectionPnt Type() { return type; };
		void SetType(xtIntersectionPnt type) { this->type=type; }; 

	};

	FaceTraits
	{
	private:
		xtSurfaceSlot *ss;
		xtOPFaceMarker type;

	public:
		FaceT():ss(NULL), type(UN_TAGGEDFACE){};

		xtSurfaceSlot *SS() { return ss; };
		void SetSS( xtSurfaceSlot *ss ) { this->ss = ss; };

		xtOPFaceMarker Type() { return type; };
		void SetType( xtOPFaceMarker type ) { this->type = type; };

	};

	HalfedgeTraits 
	{
	private:
		xtHalfEdgeMarker type;

	public:
		HalfedgeT():type(UN_TAGGED) {};

		xtHalfEdgeMarker Type() { return type; };
		void SetType( xtHalfEdgeMarker type ) { this->type = type; };
	};

	EdgeTraits
	{
	private:
		xtEdgeMarker type;

	public:
		EdgeT():type(UN_TAGGED_EDGE) {};

		xtEdgeMarker Type() { return type; };
		void SetType( xtEdgeMarker type ) { this->type = type; };
	};


};
typedef OpenMesh::TriMesh_ArrayKernelT<xtTraits> txMyOPMesh;

inline void GetFaceVertexCircularFace(txMyOPMesh &mesh, txMyOPMesh::FaceHandle fhandle, std::vector<txMyOPMesh::FaceHandle> &cirvertsfhandles)
{
	txMyOPMesh::FVIter fvit=mesh.fv_begin( fhandle );
	for ( ; fvit!=mesh.fv_end( fhandle ); ++fvit ) {
		txMyOPMesh::VFIter vfit=mesh.vf_begin( fvit );
		for ( ; vfit!=mesh.vf_end( fvit ); ++vfit ) {
			if ( vfit.handle()==fhandle ) continue;
			std::vector<txMyOPMesh::FaceHandle>::iterator fid = std::find( cirvertsfhandles.begin(), cirvertsfhandles.end(), vfit.handle() );
			if ( fid==cirvertsfhandles.end() ) {
				cirvertsfhandles.push_back( vfit.handle() );
			}
		}
	}
}


struct xtOpenMeshW
{
	txMyOPMesh mesh;
};

enum xtOPVertexType
{
	XT_ORIGINAL,
	XT_SPLIT_POINT,
};

struct xtOPVertexHolder
{
	xtOPVertexType type;
	void *datum;
};

enum xtOPFaceType
{
	ORIGINAL_FACE,
	TESSELLATE_FACE,
};

enum xtUnionType
{
	ON_SURFACEU,
	IN_SURFACEOPPOSITE,
};

struct xtOPFaceHolder
{
	xtOPFaceType ft;
	xtUnionType  ut;
};



// useful OpenMesh API
/**
* HalfedgeHandle 	find_halfedge (VertexHandle _start_vh, VertexHandle _end_vh) const 
* Find halfedge from _vh0 to _vh1. Returns invalid handle if not found. 
*/
