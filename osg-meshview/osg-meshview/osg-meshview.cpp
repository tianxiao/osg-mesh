// osg-meshview.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <osg/Array>
#include <osg/Geode>
#include <osg/Vec3>
#include <osg/MatrixTransform>
#include <osg/PolygonStipple>
#include <osg/TriangleFunctor>
#include <osg/io_utils>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgViewer/Viewer>

#include <osgGA/TrackballManipulator>
#include <osg/Math>
#include <iostream>

#include "txFemSurf.h"

osg::Node *createScene()
{
	osg::Geode *geode = new osg::Geode();

    osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
    shared_colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));

    // same trick for shared normal.
    osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
    shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));

    // create QUADS
    {
        // create Geometry object to store all the vertices and lines primitive.
        osg::Geometry* polyGeom = new osg::Geometry();
        
        // note, anticlockwise ordering.
        osg::Vec3 myCoords[] =
        {
            osg::Vec3(0.0247182, 0.0f, -0.156548),
            osg::Vec3(0.0247182, 0.0f, -0.00823939),
            osg::Vec3(-0.160668, 0.0f, -0.0453167),
            osg::Vec3(-0.222464, 0.0f, -0.13183),

            osg::Vec3(0.238942, 0.0f, -0.251302),
            osg::Vec3(0.333696, 0.0f, 0.0329576),
            osg::Vec3(0.164788, 0.0f, -0.0453167),
            osg::Vec3(0.13595,  0.0f, -0.255421)
        };
        
        int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);
        
        osg::Vec3Array* vertices = new osg::Vec3Array(numCoords,myCoords);
       
        // pass the created vertex array to the points geometry object.
        polyGeom->setVertexArray(vertices);
        
        // use the shared color array.
        polyGeom->setColorArray(shared_colors.get());
        polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
        

        // use the shared normal array.
        polyGeom->setNormalArray(shared_normals.get());
        polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
        

        // This time we simply use primitive, and hardwire the number of coords to use 
        // since we know up front,
        polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,numCoords));
        
        
        //printTriangles("Quads",*polyGeom);

        // add the points geometry to the geode.
        geode->addDrawable(polyGeom);
    }

	return geode;

}


int _tmain(int argc, _TCHAR* argv[])
{
	txFemSurf femsurf;
	femsurf.LoadFemFile("arm.fem");

	osg::Group *root = new osg::Group;
	root->addChild( createScene() );
	root->addChild( femsurf.CreateMesh() );

	osgViewer::Viewer viewer;

	viewer.setSceneData( root );

	viewer.setUpViewInWindow(400,400,640,480);

	return viewer.run();

	return 0;
}

