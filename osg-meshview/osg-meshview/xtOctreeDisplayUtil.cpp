#include "StdAfx.h"
#include "xtOctreeDisplayUtil.h"
#include "xtOctreeAdapter.h"

xtOctreeDisplayUtil::xtOctreeDisplayUtil(void)
{
}


xtOctreeDisplayUtil::~xtOctreeDisplayUtil(void)
{
}


osg::Geode *xtOctreeDisplayUtility::CreatemOctreeScene( xtOctreeAdapter *mOctreeDataA, xtPnt3<int> &mCubeCenter, std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame/*=true*/, bool debugon )
{
	osg::Geode *geode = new osg::Geode;

	if ( debugon ) {
		osg::Geometry *randPntsGeom = new osg::Geometry();

		osg::Vec3Array *randVertsArray = new osg::Vec3Array;

		std::vector<xtPnt3Adapter<double>> &adpPnts = mOctreeDataA->GetPnts();

		unsigned int nPnts = adpPnts.size();
		for ( size_t i=0; i<nPnts; ++i ) {
			xtPnt3Adapter<double> pnt = adpPnts[i];
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
};

