#include "StdAfx.h"
#include "mxtOctreeScene.h"

#include "txFemSurf.h"
#include "xtWinGUIUtil.h"
#include "xtOctreeAdapter.h"

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

}

osg::Geode *mxtOctreeScene::GetFemSurfGeom()
{
	return femsurf->CreateMesh();
}

void mxtOctreeScene::GetDumpLevelNodeList(int level,std::vector<xtOctreeNode<int> *> &dumpnodelist) 
{
	mOctree->DumpLevel(level,dumpnodelist);
}


osg::Geode *mxtOctreeScene::CreatemOctreeScene(std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame/*=true*/, bool debugon )
{
	osg::Geode *geode = new osg::Geode;

	if ( debugon ) {
		osg::Geometry *randPntsGeom = new osg::Geometry();

		osg::Vec3Array *randVertsArray = new osg::Vec3Array;

		unsigned int nPnts = mOctreeDataA->mPnts.size();
		for ( size_t i=0; i<nPnts; ++i ) {
			xtPnt3Adapter<double> pnt = mOctreeDataA->mPnts[i];
			randVertsArray->push_back(osg::Vec3(pnt.x,pnt.y,pnt.z));
		}

		randPntsGeom->setVertexArray(randVertsArray);

		osg::Vec4Array *colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f,.0f,.0f,1.0f));
		randPntsGeom->setColorArray(colors);
		randPntsGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
		osg::StateSet *pointstate = randPntsGeom->getOrCreateStateSet();
		pointstate->setAttribute(new osg::Point(5.0f),osg::StateAttribute::ON);
		pointstate->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		randPntsGeom->setStateSet(pointstate);

		randPntsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,nPnts));

		geode->addDrawable(randPntsGeom);
	}

	for ( size_t i=0; i<mDumpLevelNodelist.size(); ++i ) {
		xtPnt3<int> center = mCubeCenter + (mDumpLevelNodelist[i]->mLB + mDumpLevelNodelist[i]->mRT).Half();
		xtPnt3<int> extend = (mDumpLevelNodelist[i]->mRT - mDumpLevelNodelist[i]->mLB);//.Half();
		osg::Vec3 osgcenter(center.x,center.y,center.z);
		osg::Vec3 osgextend(extend.x,extend.y,extend.z);
		osg::Box *levelbox = new osg::Box(osgcenter,osgextend.x(),osgextend.y(),osgextend.z());
		geode->addDrawable(new osg::ShapeDrawable(levelbox));
	}

	if ( isWireFrame ) {
		osg::StateSet *state2 = new osg::StateSet();
		state2->setAttribute( new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ));
		geode->setStateSet(state2);
	}

	
	return geode;
}
