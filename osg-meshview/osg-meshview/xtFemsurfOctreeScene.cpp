#include "StdAfx.h"
#include "xtFemsurfOctreeScene.h"

#include "txFemSurf.h"
#include "xtWinGUIUtil.h"

#include "xtOctreeScene.h"


xtFemsurfOctreeScene::xtFemsurfOctreeScene(void)
{
}


xtFemsurfOctreeScene::~xtFemsurfOctreeScene(void)
{
	DestroyMem();
}

void xtFemsurfOctreeScene::DestroyMem()
{
	delete this->femSurf;
}

void xtFemsurfOctreeScene::LoadFemSurf(/*char *filename*/)
{
	femSurf = new txFemSurf;

	char *filename = OpenDialog();
	if ( !filename ) {
		exit(0);
	}
	femSurf->LoadFemFile(filename);

	BuildOctree();
}


void xtFemsurfOctreeScene::BuildOctree()
{
	xtBBox bbox = femSurf->bbox;
	this->octree = new Octree(Vec3(bbox.center.v[0],bbox.center.v[1],bbox.center.v[2]),
		Vec3(bbox.extend.v[0]+10,bbox.extend.v[1]+10,bbox.extend.v[2]+10));

	const int nPnts = femSurf->verts.size();
	this->nPnts = nPnts;
	this->octreePoints = new OctreePoint[nPnts];
	for ( int i=0; i<nPnts; ++i ) {
		Vec3 pnt(femSurf->verts[i].v[0],femSurf->verts[i].v[1],femSurf->verts[i].v[2]);
		this->octreePoints[i].setPosition(pnt);
		this->octree->insert(&(this->octreePoints[i]));
	}
	printf("Inserted points to octree\n"); //fflush(std::stdout);

	xtOctreeScene::FillLeafBox(this->leafBoxlist,this->octree);
}

osg::Geode* xtFemsurfOctreeScene::CreateScene()
{
	return CreateOctreeSceneS(this->octreePoints,this->nPnts,this->leafBoxlist);
}
