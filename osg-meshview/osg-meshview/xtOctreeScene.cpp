#include "StdAfx.h"
#include "xtOctreeScene.h"


xtOctreeScene::xtOctreeScene(void)
{
	init();
}


xtOctreeScene::~xtOctreeScene(void)
{
}

osg::Geode* xtOctreeScene::CreateOctreeScene()
{
	osg::Geode* geode = new osg::Geode();
	
	// Create points
	{
		osg::Geometry *randPntsGeom = new osg::Geometry();

		osg::Vec3Array *randVertsArray = new osg::Vec3Array;

		for ( size_t i=0; i<points.size(); ++i ) {
			randVertsArray->push_back(osg::Vec3(points[i].x,points[i].y,points[i].z));
		}

		randPntsGeom->setVertexArray(randVertsArray);

		osg::Vec4Array *colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(.0f,.0f,.0f,1.0f));
		randPntsGeom->setColorArray(colors);
		randPntsGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

		
		randPntsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,points.size()));

		geode->addDrawable(randPntsGeom);
	}

	return geode;
}
