#include "StdAfx.h"
#include "xtTriTriCollideScene.h"
#include "../buildpyramidGeometry/osgcommon.h"
#include "xtOctreeDisplayUtil.h"
#include "xtPrimitiveTestUtil.h"

bool TriTriCollide( xtTriangle *tri0, xtTriangle *tri1 ) 
{
	float v0[3] = {(float)tri0->pa.x(),(float)tri0->pa.y(),(float)tri0->pa.z()};
	float v1[3] = {(float)tri0->pb.x(),(float)tri0->pb.y(),(float)tri0->pb.z()};
	float v2[3] = {(float)tri0->pc.x(),(float)tri0->pc.y(),(float)tri0->pc.z()};

	float u0[3] = {(float)tri1->pa.x(),(float)tri1->pa.y(),(float)tri1->pa.z()};
	float u1[3] = {(float)tri1->pb.x(),(float)tri1->pb.y(),(float)tri1->pb.z()};
	float u2[3] = {(float)tri1->pc.x(),(float)tri1->pc.y(),(float)tri1->pc.z()};

	return (1==NoDivTriTriIsect(v0,v1,v2,u0,u1,u2))?true:false;
}


xtTriTriCollideScene::xtTriTriCollideScene(void)
{
	this->sceneRoot = new osg::Group;
}


xtTriTriCollideScene::~xtTriTriCollideScene(void)
{
}

void xtTriTriCollideScene::Initial() 
{
	xtVector3d tri0[3] = {
		xtVector3d(0,0,0),
		xtVector3d(2,0,0),
		xtVector3d(1,2,0),
	};

	xtVector3d tri1[3] = {
		xtVector3d(1,1,-1),
		xtVector3d(1,-1,0),
		xtVector3d(1,1.,1),
	};

	xtVector3d tri2[3] = {
		xtVector3d(0.,0.,0.0),
		xtVector3d(-2,0,0),
		xtVector3d(-1,-2,0),
	};

	for ( int i=0; i<3; ++i ) {
		(&(mTri0.pa))[i] = tri0[i];
		(&(mTri1.pa))[i] = tri1[i];
		(&(mTri2.pa))[i] = tri2[i];
	}
}


void xtTriTriCollideScene::SetUpScene()
{
	sceneRoot->addChild( xtOctreeDisplayUtility::RenderTriangle(&mTri0,xtColor(1.0,0.0,0.0,1.0),false) );
	sceneRoot->addChild( xtOctreeDisplayUtility::RenderTriangle(&mTri1,xtColor(0.0,0.0,1.0,1.0),false) );
	sceneRoot->addChild( xtOctreeDisplayUtility::RenderTriangle(&mTri2,xtColor(1.0,1.0,0.0,1.0),false) );

	if ( TriTriCollide(&mTri0,&mTri1) ) {
		printf(" 0>><<1 Two Triangle Collide\n");
	}

	if ( TriTriCollide(&mTri1,&mTri2) ) {
		printf(" 1>><<2 Two Triangle Collide\n");
	}

}

int xtTriTriCollideScene::RunRender()
{
	SetUpScene();

	osgViewer::CompositeViewer comViewer;

	osgViewer::View *viewGts = new osgViewer::View;
	viewGts->setUpViewInWindow(200,200,640,480);
	viewGts->setSceneData(sceneRoot);

	comViewer.addView(viewGts);

	return comViewer.run();

}


