#include "StdAfx.h"
#include "xtInterferecenTest.h"
#include "xtGeometrySurfaceData.h"
#include "xtGTSParser.h"
#include "../buildpyramidGeometry/osgcommon.h"
#include "xtCollisionEngine.h"
#include "xtOctreeDisplayUtil.h"
#include "xtSplitBuilder.h"

xtInterferecenTest::xtInterferecenTest(void)
{
	this->mGtsparserI = new xtGTSParser;
	this->mGtsparserJ = new xtGTSParser;
	this->mSurfI = mGtsparserI->GetGTSSurface();
	this->mSurfJ = mGtsparserJ->GetGTSSurface();


	sceneRoot = new osg::Group;
	mSB = NULL;
}

xtInterferecenTest::~xtInterferecenTest(void)
{
	if ( mSB!=NULL ) {
		delete mSB;
	}

	delete mCE;

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

void xtInterferecenTest::SetTranRot(xtVector3d *tranI, xtMatrix3d *rotI, xtVector3d *tranJ, xtMatrix3d *rotJ)
{
	this->mGtsparserI->SetTranRot(tranI,rotI);
	this->mGtsparserJ->SetTranRot(tranJ,rotJ);
}

void xtInterferecenTest::SetUpScene()
{
	//sceneRoot->addChild(mGtsparserI->CreateSceneWithTranRot());
	//sceneRoot->addChild(mGtsparserJ->CreateSceneWithTranRot());

	if ( mCE->Collide() ) {
		std::vector<xtCollidePair> &pairs = mCE->GetColliedPairs();
		sceneRoot->addChild( xtOctreeDisplayUtility::RenderCollided(mSurfI,mSurfJ,pairs) );
		mSB = new xtSplitBuilder;
		mSB->SetCE(mCE); mSB->Split();
		//sceneRoot->addChild( xtOctreeDisplayUtility::RenderSplitSegments(mSB,xtColor(0.0,1.0,1.0,1.0),5.0f) );
		sceneRoot->addChild( xtOctreeDisplayUtility::RenderSplitSegmentsWithCyliner(mSB,xtColor(0.0,1.0,1.0,1.0),0.001f) );
		sceneRoot->addChild( xtOctreeDisplayUtility::RednerSplitPntsAsSphere(mSB,xtColor(1.0,0.0,0.0,1.0),0.002f) );
#if XT_DEBUG_PERCE_POINT
		sceneRoot->addChild( xtOctreeDisplayUtility::RenderRaySegment(mSB) );
#endif
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
