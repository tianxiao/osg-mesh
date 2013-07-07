// buildpyramidGeometry.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

#include <osgGA/TerrainManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <osg/Material>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/Camera>
#include <osg/io_utils>
#include <osg/ShapeDrawable>

#include <osgText/Text>

#include <sstream>

#include "xtPickHandler.h"
#include "xtCreateHUD.h"



osgGA::KeySwitchMatrixManipulator *createKeySwitchMatrixManipulator()
{
	    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

        keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
        keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
        keyswitchManipulator->addMatrixManipulator( '3', "Drive", new osgGA::DriveManipulator() );

        unsigned int num = keyswitchManipulator->getNumMatrixManipulators();
        keyswitchManipulator->addMatrixManipulator( '4', "Terrain", new osgGA::TerrainManipulator() );

        std::string pathfile;
        char keyForAnimationPath = '5';
        //while (arguments.read("-p",pathfile))
        //{
        //    osgGA::AnimationPathManipulator* apm = new osgGA::AnimationPathManipulator(pathfile);
        //    if (apm || !apm->valid()) 
        //    {
        //        num = keyswitchManipulator->getNumMatrixManipulators();
        //        keyswitchManipulator->addMatrixManipulator( keyForAnimationPath, "Path", apm );
        //        ++keyForAnimationPath;
        //    }
        //}

        keyswitchManipulator->selectMatrixManipulator(num);

		return keyswitchManipulator.get();
}

osg::Group *createPrimitiveShape()
{
	// Create a test scene with a camera that has a relative reference frame.
    osg::Group* group = new osg::Group();

    osg::Geode* sphere = new osg::Geode();
    sphere->setName("Sphere");
    sphere->addDrawable(new osg::ShapeDrawable(new osg::Sphere()));

    osg::Geode* cube = new osg::Geode();
    cube->setName("Cube");
    cube->addDrawable(new osg::ShapeDrawable(new osg::Box()));

    osg::Camera* camera = new osg::Camera();
    camera->setRenderOrder(osg::Camera::POST_RENDER);
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    camera->setReferenceFrame(osg::Transform::RELATIVE_RF);
    camera->setViewMatrix(osg::Matrix::translate(-2, 0, 0));

    osg::MatrixTransform* xform = new osg::MatrixTransform(osg::Matrix::translate(1, 1, 1));
    xform->addChild(camera);

    group->addChild(sphere);
    group->addChild(xform);
    camera->addChild(cube);

	return group;
}


int _tmain(int argc, _TCHAR* argv[])
{
	 osg::ref_ptr<osgText::Text> updateText = new osgText::Text;

	osgViewer::Viewer viewer;
	//viewer.setCameraManipulator(createKeySwitchMatrixManipulator());
	viewer.addEventHandler(new PickHandler(updateText.get()));
	osg::Group *root = new osg::Group;

	root->addChild(createHUD(updateText.get()));
	root->addChild(createPrimitiveShape());

	viewer.setSceneData( root );

	

	viewer.setUpViewInWindow(20,20,1028,960);


	return viewer.run();
}

