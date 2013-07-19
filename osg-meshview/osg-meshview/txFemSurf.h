#pragma once
#include "xtVec3d.h"
#include <vector>
#include "xtPrimitive.h"

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
	friend class mxtOctreeScene;
public:
	txFemSurf(void);

	void LoadFemFile(char *file);

	void LoadFemFile(
		std::vector<xtVec3df> &verts,
		std::vector<xtIndexTria3> &ctria3index,
		std::vector<xtIndexCquad4> &cquad4index) {
			this->verts = verts;
			this->ctria3index = ctria3index;
			this->cquad4index = cquad4index;
	}

	void DumpToFem(char *filename);

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

