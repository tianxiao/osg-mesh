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
		xtPntOverlapState pnttype;
		int resignedIdx;
		int vinpoolidx;
		
	public:
		VertexT():type( UN_TAGEDPNT ), resignedIdx(-1), pnttype(PNT_UNTAGGED), vinpoolidx(-1) {};

		xtIntersectionPnt Type() { return type; };
		void SetType(xtIntersectionPnt type) { this->type=type; }; 

		int GetReAssignedIdx() { return this->resignedIdx; };
		void SetReassingedIdx(int idx) { this->resignedIdx=idx; }; 

		xtPntOverlapState PntType() { return pnttype; };
		void SetPntType(xtPntOverlapState state ) { this->pnttype = state };
	
		int GetVinPoolIdx() { return this->vinpoolidx; };
		void SetVinPoolIdx( int idx ) { this->vinpoolidx=idx; };
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

inline void xtDumpMeshUtil( txMyOPMesh &mesh, char *filename ) 
{
	try
	{
		if ( !OpenMesh::IO::write_mesh(mesh, filename) )
		{
			std::cerr << "Cannot write mesh to file 'output.off'" << std::endl;
			//return 1;
		}
	} catch ( std::exception& x )
	{
		std::cerr << x.what() << std::endl;
		//return 1;
	}
}



// useful OpenMesh API
/**
* HalfedgeHandle 	find_halfedge (VertexHandle _start_vh, VertexHandle _end_vh) const 
* Find halfedge from _vh0 to _vh1. Returns invalid handle if not found. 
*/


/**
* About the openmesh's garbage_collection

From: http://www.cnblogs.com/vranger/archive/2013/01/31/2888153.html

今天重又用起OpenMesh，动态编辑网格，效率很高。但中间遇到一个问题，花了我半天时间才搞定。

我使用删除顶点函数如下（一）所示，mesh_.garbage_collection();也加了，但怎么都删不掉，总是报错。

 

（一）

//顶点删除设置
  for (v_h_itr=del_v_handle_set.begin();v_h_itr!=del_v_handle_set.end();v_h_itr++)
  {
   Mesh::VertexHandle _vf=(*v_h_itr);
   pDmesh->mesh_.delete_vertex(_vf,false);
  }

  //执行删除操作
  pDmesh->mesh_.garbage_collection();

完全按照OpenMesh官网上提供的例子，就是不行。查阅之前的代码，才发现delete几何元素之前，要先获取它的属性定义。

用完后释放。如下（二）所示，问题解决。

（二） 

//获取预先定义属性
  if (!pDmesh->mesh_.has_vertex_status())
   pDmesh->mesh_.request_vertex_status();
  if(!pDmesh->mesh_.has_face_status())
   pDmesh->mesh_.request_face_status();
  if(!pDmesh->mesh_.has_edge_status())
   pDmesh->mesh_.request_edge_status();
  //

  //顶点删除设置
  for (v_h_itr=del_v_handle_set.begin();v_h_itr!=del_v_handle_set.end();v_h_itr++)
  {
   Mesh::VertexHandle _vf=(*v_h_itr);
   pDmesh->mesh_.delete_vertex(_vf,false);
  }

  //执行删除操作
  pDmesh->mesh_.garbage_collection();


  //释放预先定义属性
  if (pDmesh->mesh_.has_vertex_status())
   pDmesh->mesh_.release_vertex_status();
  if(pDmesh->mesh_.has_face_status())
   pDmesh->mesh_.release_face_status();
  if(pDmesh->mesh_.has_edge_status())
   pDmesh->mesh_.release_edge_status();

*/
