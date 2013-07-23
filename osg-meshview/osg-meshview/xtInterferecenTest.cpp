#include "StdAfx.h"
#include "xtInterferecenTest.h"
#include "xtGeometrySurfaceData.h"
#include "xtGTSParser.h"
#include "../buildpyramidGeometry/osgcommon.h"

xtInterferecenTest::xtInterferecenTest(void)
{
	this->mGtsparserI = new xtGTSParser;
	this->mGtsparserJ = new xtGTSParser;
	this->mSurfI = mGtsparserI->GetGTSSurface();
	this->mSurfJ = mGtsparserJ->GetGTSSurface();

	sceneRoot = new osg::Group;
}


xtInterferecenTest::~xtInterferecenTest(void)
{

	delete this->mGtsparserI;
	delete this->mGtsparserJ;

}

void xtInterferecenTest::LoadSurf(const char *fileI, const char *fileJ)
{
	this->mGtsparserI->LoadGTS(fileI);
	this->mGtsparserJ->LoadGTS(fileJ);
}

void xtInterferecenTest::SetTranRot(xtVector3d *tranI, xtMatrix3d *rotI, xtVector3d *tranJ, xtMatrix3d *rotJ)
{
	this->mGtsparserI->SetTranRot(tranI,rotI);
	this->mGtsparserJ->SetTranRot(tranJ,rotJ);
}

void xtInterferecenTest::SetUpScene()
{
	sceneRoot->addChild(mGtsparserI->CreateSceneWithTranRot());
	sceneRoot->addChild(mGtsparserJ->CreateSceneWithTranRot());
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
