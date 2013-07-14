#include "StdAfx.h"
#include "xtObjOctreeScene.h"

#include "xtObjScene.h"
#include "xtTinyIMPL.h"
#include "xtOctreeAdapter.h"
#include "xtOctreeDisplayUtil.h"

xtObjOctreeScene::xtObjOctreeScene(void)
{
}


xtObjOctreeScene::~xtObjOctreeScene(void)
{
	delete mObjscene;
	mObjscene = NULL;
}

void xtObjOctreeScene::LoadObjData( const char *basepath, const char *filename )
{
	mObjscene = new xtObjScene;
	mObjscene->LoadObj(basepath, filename);
}

void xtObjOctreeScene::BuildOctree(double resolution)
{
	BuildAdapterData(resolution);
}

void xtObjOctreeScene::BuildAdapterData(double resolution)
{
	std::vector<int> vertsoffsetlist;
	std::vector<shape_t> &shapes = (mObjscene->mRawObjData->mShapes);
	vertsoffsetlist.reserve(shapes.size());
	vertsoffsetlist.push_back(0);
	int offset = 0;
	for ( size_t i=0; i<shapes.size(); ++i ) {
		offset += shapes[i].mesh.positions.size();
		vertsoffsetlist.push_back(offset);
	}

	mOctreeDataA = new xtOctreeAdapter;
	mOctreeDataA->ReserveSize( vertsoffsetlist[vertsoffsetlist.size()-1] );

	for ( size_t i=0; i<shapes.size(); ++i ) {
		shape_t &shape = shapes[i];
		for ( size_t j=0; j<shape.mesh.positions.size()/3; ++j ) {
			double p[3];
			for ( int k=0; k<3; k++ ) {
				p[k] = (double) shape.mesh.positions[3*j+k];
			}
			mOctreeDataA->AddPoint( p );
		}
	}

	mLargestDepth = mOctreeDataA->AdaptDataForOctree( resolution );

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

	std::vector<xtPnt3Adapter<double>> &afverts = mOctreeDataA->mPnts;

	// Insert obj's triangle
	for ( size_t i=0; i<shapes.size(); ++i ) {
		shape_t &shape = shapes[i];
		const int vertextoffset = vertsoffsetlist[i]/3;
		for ( size_t i=0; i<shape.mesh.indices.size()/3; ++i ) {
			const int a = shape.mesh.indices[i*3] + vertextoffset;
			const int b = shape.mesh.indices[i*3+1] + vertextoffset;
			const int c = shape.mesh.indices[i*3+2] + vertextoffset;
			mOctree->InsertTriangle(
				&(afverts[a].x),
				&(afverts[b].x),
				&(afverts[c].x)
				);
		}
	}

}

void xtObjOctreeScene::GetDumpLevelNodeList(int level,std::vector<xtOctreeNode<int> *> &dumpnodelist) 
{
	mOctree->DumpLevel(level, dumpnodelist);
}


osg::Geode *xtObjOctreeScene::CreatemOctreeScene(std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame/*=true*/, bool debugon/*=true*/)
{
	return xtOctreeDisplayUtility::CreatemOctreeScene( this->mOctreeDataA, this->mCubeCenter, mDumpLevelNodelist, isWireFrame, debugon);
}