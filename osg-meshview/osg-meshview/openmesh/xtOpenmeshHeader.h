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

typedef OpenMesh::TriMesh_ArrayKernelT<> txOPMesh;

struct xtTraits :public OpenMesh::DefaultTraits
{
	//typedef OpenMesh::Vec3d Point;
	VertexAttributes(OpenMesh::Attributes::Status);
	FaceAttributes(OpenMesh::Attributes::Status);
	EdgeAttributes(OpenMesh::Attributes::Status);
};
typedef OpenMesh::TriMesh_ArrayKernelT<xtTraits> txMyOPMesh;


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
