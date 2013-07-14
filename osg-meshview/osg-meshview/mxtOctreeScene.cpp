#include "StdAfx.h"
#include "mxtOctreeScene.h"

#include "txFemSurf.h"
#include "xtWinGUIUtil.h"
#include "xtOctreeAdapter.h"
#include "xtOctreeDisplayUtil.h"

mxtOctreeScene::mxtOctreeScene(void)
{
}


mxtOctreeScene::~mxtOctreeScene(void)
{
	delete mOctree;
	delete mOctreeDataA;
	delete femsurf;
}

void mxtOctreeScene::LodaFemSurfData()
{
	femsurf = new txFemSurf;

	char *filename = OpenDialog();
	if ( !filename ) {
		exit(0);
	}
	femsurf->LoadFemFile(filename);
}

// only push the points
void mxtOctreeScene::BuildOctree()
{
	// data adapter
	mOctreeDataA = new xtOctreeAdapter;
	mOctreeDataA->ReserveSize( femsurf->verts.size() );

	for ( size_t i=0; i<femsurf->verts.size(); ++i ) {
		double p[3];
		for ( int j=0; j<3; ++j ) {
			p[j] = (double) femsurf->verts[i].v[j];
		}
		mOctreeDataA->AddPoint( p );
	}

	mLargestDepth = mOctreeDataA->AdaptDataForOctree(this->mResolution);

	// build octree 
	xtPnt3Adapter<double> dcenter = mOctreeDataA->GetCenter();
	xtPnt3Adapter<double> dextend = mOctreeDataA->GetExtend();
	std::vector<xtPnt3Adapter<double> > &points = mOctreeDataA->GetPnts();

	int center[3] = { (int)dcenter.x, (int)dcenter.y, (int)dcenter.z };
	// for drawing the cube
	mCubeCenter.x = center[0];
	mCubeCenter.y = center[1];
	mCubeCenter.z = center[2];
	mOctree = new xtOctree<int>( center, mLargestDepth );

	for ( size_t i=0; i<points.size(); ++i ) {
		mOctree->InsertPoint(&(points[i].x));
	}

	// Insert triangle
	for ( size_t i=0; i<femsurf->ctria3index.size(); ++i ) {
		xtIndexTria3 currTria3 = femsurf->ctria3index[i];
		mOctree->InsertTriangle(
			&(points[currTria3.a[0]].x),
			&(points[currTria3.a[1]].x),
			&(points[currTria3.a[2]].x));
	}

	// Insert quad
	for ( size_t i=0; i<femsurf->cquad4index.size(); ++i ) {
		xtIndexCquad4 currquad4 = femsurf->cquad4index[i];
		xtIndexTria3 currTria31(currquad4.a[0],currquad4.a[1],currquad4.a[2]);
		xtIndexTria3 currTria32(currquad4.a[0],currquad4.a[2],currquad4.a[3]);

		mOctree->InsertTriangle(
			&(points[currTria31.a[0]].x),
			&(points[currTria31.a[1]].x),
			&(points[currTria31.a[2]].x));

		mOctree->InsertTriangle(
			&(points[currTria32.a[0]].x),
			&(points[currTria32.a[1]].x),
			&(points[currTria32.a[2]].x));
	}

}

osg::Geode *mxtOctreeScene::GetFemSurfGeom()
{
	return femsurf->CreateMesh();
}

void mxtOctreeScene::GetDumpLevelNodeList(int level,std::vector<xtOctreeNode<int> *> &dumpnodelist) 
{
	mOctree->DumpLevel(level,dumpnodelist);
}


osg::Geode *mxtOctreeScene::CreatemOctreeScene( std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame/*=true*/, bool debugon )
{
	return xtOctreeDisplayUtility::CreatemOctreeScene(this->mOctreeDataA, this->mCubeCenter, mDumpLevelNodelist, isWireFrame, debugon);
}
