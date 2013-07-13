#include "StdAfx.h"
#include "xtOctreeScene.h"


xtOctreeScene::xtOctreeScene(void)
{
	init();
}


xtOctreeScene::~xtOctreeScene(void)
{
	DestroyMem();
}

void xtOctreeScene::DestroyMem()
{
	delete this->octree;
	delete this->octreePoints;
}

osg::Geode* xtOctreeScene::CreateOctreeScene()
{
	return CreateOctreeSceneS(this->octreePoints,points.size(),this->leafboxList);
}

osg::Geode*  CreateOctreeSceneS(OctreePoint *octreePoints, const int nPnts,std::vector<osg::Box *> &leafboxList)
{
	osg::Geode* geode = new osg::Geode();
	
	// Create points
	if ( true ) 
	{
		osg::Geometry *randPntsGeom = new osg::Geometry();

		osg::Vec3Array *randVertsArray = new osg::Vec3Array;

		for ( int i=0; i<nPnts; ++i ) {
			Vec3 pnt = octreePoints[i].getPosition();
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

	
	{
		osg::StateSet *state2 = new osg::StateSet();
		state2->setAttribute( new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ));
		// The following code i try to transparent. failed!
		//state2->setMode(GL_BLEND,osg::StateAttribute::ON);
		//state2->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		//state2->setren
		for ( size_t i=0; i<leafboxList.size(); ++i ) {
			geode->addDrawable(new osg::ShapeDrawable(leafboxList[i]));
		}
		geode->setStateSet(state2);
	}

	return geode;
}

void xtOctreeScene::FillLeafBox(std::vector<osg::Box *> &leafboxList, Octree *root)
//void xtOctreeScene::FillLeafBox(Octree *root)
{
	if ( root->isLeafNode() ) {
		Vec3 origin = root->GetOctreeCenter();
		Vec3 extension = root->GetHalfExtension();
		osg::Vec3 or(origin.x,origin.y,origin.z);
		osg::Box *pleafbox = new osg::Box(or,2*extension.x,2*extension.y,2*extension.z);
		leafboxList.push_back(pleafbox);
		return;
	} else {
		Octree **childlist = root->GetChildren();
		for ( int i=0; i<8; ++i ) {
			FillLeafBox(leafboxList,childlist[i]);
		}
	}
}
