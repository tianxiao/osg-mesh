#pragma once
#include "xtVec3d.h"
#include <vector>

class xtIndexTria3
{
public:
	xtIndexTria3(int a, int b, int c) {
		this->a[0] = a;
		this->a[1] = b;
		this->a[2] = c;
	}

	int a[3];
};

class xtIndexCquad4
{
public:
	xtIndexCquad4(int a, int b, int c, int d) {
		this->a[0] = a;
		this->a[1] = b;
		this->a[2] = c;
		this->a[3] = d;
	}

	int a[4];
};

struct xtNormalFaceList
{
	std::vector<int> facelist;
};

struct xtBBox
{
	xtVec3df center;
	xtVec3df extend;
};

namespace osg
{
	class Geometry;
	class Geode;
};

class txFemSurf
{
	friend class xtFemsurfOctreeScene;
public:
	txFemSurf(void);

	void LoadFemFile(char *file);

	void Draw();

	void RenderSurf2();

	~txFemSurf(void);

	// osg render function
	osg::Geode* CreateMesh();

private:
	//void CalculateBBox();
	void FillVC3C4(std::vector<xtVec3df> &tempverts,
		std::vector<int> &tempctria3, std::vector<int> &tempcquad4);
	void CalculateFaceNormal();
	void RequireNormal() ;
	void CalculateBBox();

	void BuildRendData();
	void DestroyRenderData();

	void RenderSurf(bool addWireframe, bool defaultt=true );


private:
	std::vector<xtVec3df> verts;
	
	std::vector<xtVec3df> vertsnormal;
	std::vector<xtNormalFaceList> normalfacelist;

	std::vector<xtIndexTria3> ctria3index;
	std::vector<xtVec3df> ctria3normal;
	std::vector<xtIndexCquad4> cquad4index;
	std::vector<xtVec3df> cquad4normal;
	std::vector<xtVec3df> facenormal;

	xtBBox bbox;


	// drawing element
	float* pVertList;
	float* pNormList;
	float* pColorList;
	int nbFaces_;
};

