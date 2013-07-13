// osg-meshview.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
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

#include <osgWidget/Util>
#include <osgWidget/WindowManager>
#include <osgWidget/Box>
#include <osgWidget/Label>

#include "../buildpyramidGeometry/xtCreateHUD.h"
#include "../buildpyramidGeometry/xtPickHandler.h"

#include "txFemSurf.h"
#include "xtOctreeScene.h"
#include "xtWinGUIUtil.h"
#include "xtFemsurfOctreeScene.h"

#include "mxtOctreeScene.h"


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

// For now this is just an example, but osgWidget::Menu will later be it's own Window.
// I just wanted to get this out there so that people could see it was possible.

const unsigned int MASK_2D = 0xF0000000;
const unsigned int MASK_3D = 0x0F000000;

struct ColorLabel: public osgWidget::Label {
    ColorLabel(const char* label):
    osgWidget::Label("", "") {
        setFont("fonts/Vera.ttf");
        setFontSize(14);
        setFontColor(1.0f, 1.0f, 1.0f, 1.0f);
        setColor(0.3f, 0.3f, 0.3f, 1.0f);
        addHeight(18.0f);
        setCanFill(true);
        setLabel(label);
        setEventMask(osgWidget::EVENT_MOUSE_PUSH | osgWidget::EVENT_MASK_MOUSE_MOVE);
    }

    bool mousePush(double, double, const osgWidget::WindowManager*) {
        return true;
    }

    bool mouseEnter(double, double, const osgWidget::WindowManager*) {
        setColor(0.6f, 0.6f, 0.6f, 1.0f);
        
        return true;
    }

    bool mouseLeave(double, double, const osgWidget::WindowManager*) {
        setColor(0.3f, 0.3f, 0.3f, 1.0f);
        
        return true;
    }
};

class ColorLabelMenu: public ColorLabel {
    osg::ref_ptr<osgWidget::Window> _window;

public:
    ColorLabelMenu(const char* label):
    ColorLabel(label) {
        _window = new osgWidget::Box(
            std::string("Menu_") + label,
            osgWidget::Box::VERTICAL,
            true
        );

        _window->addWidget(new ColorLabel("Open Some Stuff"));
        _window->addWidget(new ColorLabel("Do It Now"));
        _window->addWidget(new ColorLabel("Hello, How Are U?"));
        _window->addWidget(new ColorLabel("Hmmm..."));
        _window->addWidget(new ColorLabel("Option 5"));

        _window->resize();

        setColor(0.8f, 0.8f, 0.8f, 0.8f);
    }

    void managed(osgWidget::WindowManager* wm) {
        osgWidget::Label::managed(wm);

        wm->addChild(_window.get());

        _window->hide();
    }

    void positioned() {
        osgWidget::Label::positioned();

        _window->setOrigin(getX(), getHeight());
        _window->resize(getWidth());
    }

    bool mousePush(double, double, const osgWidget::WindowManager*) {
        if(!_window->isVisible()) _window->show();

        else _window->hide();

        return true;
    }

    bool mouseLeave(double, double, const osgWidget::WindowManager*) {
        if(!_window->isVisible()) setColor(0.8f, 0.8f, 0.8f, 0.8f);

        return true;
    }
};



osg::Geode* CreateFemGraph()
{
	//osg::ref_ptr<txFemSurf> ref_femsurf = new txFemSurf;
	txFemSurf *pfemsurf = new txFemSurf;
	char *filename = OpenDialog();
	if ( !filename ) {
		exit(0);
	}
	pfemsurf->LoadFemFile(filename);

	return pfemsurf->CreateMesh();
}


int _tmain(int argc, _TCHAR* argv[])
{

	osg::ref_ptr<osgText::Text> updateText = new osgText::Text;
	txFemSurf femsurf;

	osg::Group *root = new osg::Group;
	osgViewer::CompositeViewer comViewer;
	//root->addChild( createScene() );
	//root->addChild( femsurf.CreateMesh() );
	//root->addChild( CreateFemGraph() );

	//xtOctreeScene octreescene;
	//root->addChild( octreescene.CreateOctreeScene() );

	/*****
	xtFemsurfOctreeScene femOctScene;
	femOctScene.LoadFemSurf();
	root->addChild( femOctScene.CreateScene() );
	*****/


	mxtOctreeScene myOctreeScene;
	myOctreeScene.LodaFemSurfData();
	myOctreeScene.SetResolution(0.1);
	myOctreeScene.BuildOctree();

	std::vector<xtOctreeNode<int> *> dumplevel;
	myOctreeScene.GetDumpLevelNodeList(4,dumplevel);
	root->addChild( myOctreeScene.CreatemOctreeScene(dumplevel) );

	osg::Group *root3 = new osg::Group;
	{
	std::vector<xtOctreeNode<int> *> dumplevel;
	myOctreeScene.GetDumpLevelNodeList(3,dumplevel);
	root3->addChild( myOctreeScene.CreatemOctreeScene(dumplevel) );
	}
	osg::Group *root4 = new osg::Group;
	{
	std::vector<xtOctreeNode<int> *> dumplevel;
	myOctreeScene.GetDumpLevelNodeList(2,dumplevel);
	root3->addChild( myOctreeScene.CreatemOctreeScene(dumplevel) );
	}
	osg::Group *root5 = new osg::Group;
	{
	std::vector<xtOctreeNode<int> *> dumplevel;
	myOctreeScene.GetDumpLevelNodeList(1,dumplevel);
	root3->addChild( myOctreeScene.CreatemOctreeScene(dumplevel) );
	}
	osg::Group *root6 = new osg::Group;
	{
	std::vector<xtOctreeNode<int> *> dumplevel;
	myOctreeScene.GetDumpLevelNodeList(0,dumplevel);
	root3->addChild( myOctreeScene.CreatemOctreeScene(dumplevel) );
	}

	{
		// add original surface
		root3->addChild( myOctreeScene.GetFemSurfGeom() );
	}

	{
		osgViewer::View *viewer3 = new osgViewer::View;
	
		viewer3->setSceneData( root3 );
		viewer3->setUpViewInWindow(20,20,960,640);

		comViewer.addView(viewer3);
	}

	/****
	{
		osgViewer::View *viewer4 = new osgViewer::View;
	
		viewer4->setSceneData( root4 );
		viewer4->setUpViewInWindow(20,20,960,640);

		comViewer.addView(viewer4);
	}

	{
		osgViewer::View *view5 = new osgViewer::View;
	
		view5->setSceneData( root5 );
		view5->setUpViewInWindow(20,20,960,640);

		comViewer.addView(view5);
	}
	****/

	root->addChild( createHUD(updateText.get()) );

	osgViewer::Viewer viewer;
	
	viewer.setSceneData( root );
	viewer.setUpViewInWindow(200,200,960,640);
	viewer.addEventHandler(new PickHandler(updateText.get()));

	//return viewer.run();
	comViewer.addView(&viewer);

	return comViewer.run();

}

