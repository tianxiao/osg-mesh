#include "StdAfx.h"
#include "xtInterferecenTest.h"
#include "xtGeometrySurfaceData.h"
#include "xtGTSParser.h"
#include "../buildpyramidGeometry/osgcommon.h"
#include "xtCollisionEngine.h"
#include "xtOctreeDisplayUtil.h"
#include "xtRenderUtil.h"
#include "xtSplitBuilder.h"
#include "./OpenMesh/xtCPMesh.h"

xtInterferecenTest::xtInterferecenTest(void)
{
	this->mGtsparserI = new xtGTSParser;
	this->mGtsparserJ = new xtGTSParser;
	this->mSurfI = mGtsparserI->GetGTSSurface();
	this->mSurfJ = mGtsparserJ->GetGTSSurface();
	this->mMeshI = new xtCPMesh;
	this->mMeshJ = new xtCPMesh;

	sceneRoot = new osg::Group;
	mSB = NULL;
}

xtInterferecenTest::~xtInterferecenTest(void)
{
	if ( mSB!=NULL ) {
		delete mSB;
	}

	delete mCE;

	delete this->mMeshI;
	delete this->mMeshJ;

	delete this->mGtsparserI;
	delete this->mGtsparserJ;

}

void xtInterferecenTest::LoadSurf(const char *fileI, const char *fileJ)
{
	this->mGtsparserI->LoadGTS(fileI);
	this->mGtsparserJ->LoadGTS(fileJ);

	mCE = new xtCollisionEngine;
	mCE->SetSurfIJ(mSurfI,mSurfJ);
}

void xtInterferecenTest::LoadRawMesh()
{
	mMeshI->LoadRawMesh( mSurfI );
	mMeshJ->LoadRawMesh( mSurfJ );
}

void xtInterferecenTest::SetTranRot(xtVector3d *tranI, xtMatrix3d *rotI, xtVector3d *tranJ, xtMatrix3d *rotJ)
{
	this->mGtsparserI->SetTranRot(tranI,rotI);
	this->mGtsparserJ->SetTranRot(tranJ,rotJ);

	LoadRawMesh();
}

void xtInterferecenTest::SetUpScene()
{
	//sceneRoot->addChild(mGtsparserI->CreateSceneWithTranRot());
	//sceneRoot->addChild(mGtsparserJ->CreateSceneWithTranRot());

	if ( mCE->Collide() ) {
		std::vector<xtCollidePair> &pairs = mCE->GetColliedPairs();
		//sceneRoot->addChild( xtOctreeDisplayUtility::RenderCollided(mSurfI,mSurfJ,pairs) );
		mSB = new xtSplitBuilder;
		mSB->SetCE(mCE); mSB->Split();

		UpdateConnectivity();

		Difference();

		sceneRoot->addChild( xtRenderUtil::RenderCPMesh( mMeshI ) );

		//sceneRoot->addChild( xtOctreeDisplayUtility::RenderSplitSegments(mSB,xtColor(0.0,1.0,1.0,1.0),5.0f) );
		//sceneRoot->addChild( xtOctreeDisplayUtility::RenderSplitSegmentsWithCyliner(mSB,xtColor(0.0,1.0,1.0,1.0),0.001f) );
		//sceneRoot->addChild( xtOctreeDisplayUtility::RednerSplitPntsAsSphere(mSB,xtColor(1.0,0.0,0.0,1.0),0.002f) );
#if XT_DEBUG_PLANAR_TRI_SET
		//sceneRoot->addChild( xtOctreeDisplayUtility::RkenderPlanarTriSplitSegs(mSB) );
#endif
#if XT_DEBUG_PERCE_POINT
		sceneRoot->addChild( xtOctreeDisplayUtility::RenderRaySegment(mSB) );
#endif
		sceneRoot->addChild( xtOctreeDisplayUtility::RenderPlanarTris(mSB) );
		//sceneRoot->addChild( xtOctreeDisplayUtility::RenderSplitSegmentsWBODebug(mSB,xtColor(0.0,0.0,1.0,1.),0.001f) );
	}



#if RTREE_DEBUG_RESULT
	// RTree results
	if ( mCE->tempIBoxList.size() ) {
		for ( int k=0; k<mCE->tempIBoxList.size(); ++k ) {
			const int index = k;
			const int idxJ = mCE->tempJlist[index];
			std::vector<int> &idxIlist = mCE->tempIBoxList[index];
			for ( size_t i=0; i<idxIlist.size(); ++i ) {
				xtIndexTria3 &tria = mSurfI->indices[idxIlist[i]];
				xtTriangle tri(mSurfI->verts[tria.a[0]],mSurfI->verts[tria.a[1]],mSurfI->verts[tria.a[2]]);
				tri.TranRot(mSurfI->tran,mSurfI->rot);
				sceneRoot->addChild( xtOctreeDisplayUtility::RenderTriangle(&tri,xtColor(1.0,.0,.0,1.),false) );
			}
			xtIndexTria3 &triaJ = mSurfJ->indices[idxJ];
			xtTriangle triJ(mSurfJ->verts[triaJ.a[0]],mSurfJ->verts[triaJ.a[1]],mSurfJ->verts[triaJ.a[2]]);
			triJ.TranRot(mSurfJ->tran,mSurfJ->rot);
			sceneRoot->addChild( xtOctreeDisplayUtility::RenderTriangle(&triJ,xtColor(0.0,1.0,1.0,1.0),false) );
		}
	}
#endif

}

void xtInterferecenTest::UpdateConnectivity()
{
	mMeshI->LoadRawData( mSB->mPSI, &(mSB->mAbpool) );
	mMeshJ->LoadRawData( mSB->mPSJ, &(mSB->mAbpool) );
}

void xtInterferecenTest::Difference()
{
	mMeshI->Difference( *mMeshJ );
	//mMeshJ->Difference( *mMeshI );
}

int xtInterferecenTest::RunScene()
{
	SetUpScene();

	osgViewer::CompositeViewer comViewer;

	osgViewer::View *viewGts = new osgViewer::View;
	viewGts->setUpViewInWindow(200,200,640,480);
	viewGts->setSceneData(sceneRoot);

	comViewer.addView(viewGts);

	return comViewer.run();

}
