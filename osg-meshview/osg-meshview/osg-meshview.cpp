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
#include "xtObjScene.h"
#include "xtObjOctreeScene.h"

#include "xtWRLMParser.h"
#include "xtWRLScene.h"
#include "xtImagPro.h"

#include "linearmathcom.h"

#include "xtGTSParser.h"
#include "xtInterferecenTest.h"

#include "xtTriTriCollideScene.h"

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

int testOctreeBuildFromFem()
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
	int largestdepth = myOctreeScene.GetLargestDepth();

	std::vector<xtOctreeNode<int> *> dumplevel;
	myOctreeScene.GetDumpLevelNodeList(8,dumplevel);
	root->addChild( myOctreeScene.CreatemOctreeScene(dumplevel,false) );

	osg::Group *root3 = new osg::Group;
	{
	std::vector<xtOctreeNode<int> *> dumplevel;
	myOctreeScene.GetDumpLevelNodeList(3,dumplevel);
	root3->addChild( myOctreeScene.CreatemOctreeScene(dumplevel,false) );
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
	myOctreeScene.GetDumpLevelNodeList(4,dumplevel);
	osg::Geode *geode = myOctreeScene.CreatemOctreeScene(dumplevel,false);
	root3->addChild( geode );
	root5->addChild( geode );
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
	

	root->addChild( createHUD(updateText.get()) );

	osgViewer::Viewer viewer;
	
	viewer.setSceneData( root );
	viewer.setUpViewInWindow(200,200,960,640);
	viewer.addEventHandler(new PickHandler(updateText.get()));

	//return viewer.run();
	comViewer.addView(&viewer);

	return comViewer.run();
}

int testObjScene()
{
	osg::Group *root = new osg::Group;

	xtObjScene objscene;
	//char *basepath = "..\\scene\\";
	//char *filename = "..\\scene\\cornell_box.obj";
	//char *basepath = "..\\scene\\scene\\";
	//char *filename = "..\\scene\\bunny.obj";
	char *basepath = "..\\scene\\";
	char *filename = "..\\scene\\buddha.obj";
	objscene.LoadObj(basepath,filename);
	root->addChild( objscene.CreateObjScene() );

	osgViewer::Viewer viewer;
	
	viewer.setSceneData( root );
	viewer.setUpViewInWindow(200,200,960,640);

	return viewer.run();
}

int testObjOctreeScene()
{
	osg::Group *root = new osg::Group;

	xtObjScene objscene;

	//char *basepath = "..\\scene\\";
	//char *filename = "..\\scene\\cornell_box.obj";
	char *basepath = "E:\\gitProject\\osg-mesh\\osg-meshview\\osg-meshview\\scene";
	char *filename = "E:\\gitProject\\osg-mesh\\osg-meshview\\osg-meshview\\scene\\bunny.obj";
	//char *basepath = "..\\scene\\scene\\";
	//char *filename = "..\\scene\\buddha.obj";
	xtObjOctreeScene objoctreescene;
	objoctreescene.LoadObjData(basepath, filename);
	objoctreescene.BuildOctree(0.0001);
	int largestdepth = objoctreescene.GetLargestDepth();

	int dumpLevel = largestdepth-3;
	std::vector<xtOctreeNode<int> *> dumpnodelist;
	objoctreescene.GetDumpLevelNodeList(dumpLevel,dumpnodelist);
	root->addChild( objoctreescene.CreatemOctreeScene(dumpnodelist,false) );

	osgViewer::View *octreeView = new osgViewer::View;
	
	octreeView->setSceneData( root );
	octreeView->setUpViewInWindow(200,200,960,640);

	osgViewer::CompositeViewer comViewer;

	osgViewer::View *sectionView = new osgViewer::View;
	osg::Group *rootSection = new osg::Group;
	rootSection->addChild( objoctreescene.SectionAxisX( 0 ) );
	rootSection->addChild( objoctreescene.SectionAxisX( 500 ) );
	rootSection->addChild( objoctreescene.SectionAxisX( -500 ) );
	rootSection->addChild( objoctreescene.SectionAxisY( 0 ) );
	rootSection->addChild( objoctreescene.SectionAxisZ( 0 ) );
	sectionView->setSceneData( rootSection );
	sectionView->setUpViewInWindow(400,400,960,640);
	comViewer.addView( sectionView );

	comViewer.addView( octreeView );

	return comViewer.run();
}

int testFaceObjScene()
{
	osg::Group *root = new osg::Group;

	xtObjScene objscene;
	char *face0 = "sub91.fem";
	char *face1 = "sub98.fem";

	//===========================================
	txFemSurf *pfemsurf = new txFemSurf;
	pfemsurf->LoadFemFile(face0);

	root->addChild( pfemsurf->CreateMesh() );

	txFemSurf *pfemsurf2 = new txFemSurf;
	pfemsurf2->LoadFemFile(face1);

	root->addChild( pfemsurf2->CreateMesh() );
	//===========================================

	osgViewer::Viewer viewer;
	
	viewer.setSceneData( root );
	viewer.setUpViewInWindow(200,200,960,640);

	return viewer.run();
}

static inline int TestWRLParser()
{
	xtWRLMParser wrlparser;
	wrlparser.LoadWRLFile("sub98.wrl");
	wrlparser.DumpToFemFile("sub98.fem");
	DumImage(wrlparser.GetPicData(), "sub98.ppm");

	return 0;
}

static inline int TestWRLParserTextureDisplay()
{
	osg::Group *root = new osg::Group;

	xtWRLMParser wrlparser;
	wrlparser.LoadWRLFile("sub98.wrl");

	osg::Texture2D *facesub98 = new osg::Texture2D;
	// Protect form being optimized away as static state
	facesub98->setDataVariance(osg::Object::DYNAMIC);

	// load an image by reading a file
	osg::Image *imageface98 = osgDB::readImageFile("sub98.bmp");
		//"sub98_session1_frame1.png");
	if ( !imageface98 ) {
		std::cout << " couldn't find texture, quiting." << std::endl;
		return -1;
	}
	// Assign the texture to the image 
	facesub98->setImage(imageface98);

	// Create a new StateSet with default settings
	osg::StateSet *stateOne = new osg::StateSet;

	stateOne->setTextureAttributeAndModes(
		0, facesub98, osg::StateAttribute::ON);

	osg::Geode *facegeod = CreateGeo(wrlparser.GetData());
	facegeod->setStateSet( stateOne );
	//===========================================
	root->addChild( facegeod );
	//===========================================

	osgViewer::Viewer viewer;
	
	viewer.setSceneData( root );
	viewer.setUpViewInWindow(200,200,960,640);

	return viewer.run();
}


static inline int TestGTSParser()
{
	xtGTSParser gtsparser;
	//const char* filename = "D:\\model3d\\gts\\head.gts";
	const char* filename = "D:\\model3d\\gts\\epcot.gts";
	const char* conefilename = "D:\\model3d\\gts\\cone.gts";
	gtsparser.LoadGTS(filename);

	osgViewer::CompositeViewer comViewer;

	osg::Group *gtsroot = new osg::Group;
	gtsroot->addChild(gtsparser.CreateScene());
	osgViewer::View *viewGts = new osgViewer::View;
	viewGts->setUpViewInWindow(200,200,640,480);
	viewGts->setSceneData(gtsroot);

	comViewer.addView(viewGts);

	return comViewer.run();


}

static inline int TestGTSCollision()
{
	xtGTSParser gtsparser;
	xtGTSParser conegtsparser;
	//const char* filename = "D:\\model3d\\gts\\head.gts";
	const char* filename = "D:\\model3d\\gts\\sphere5.gts";
	const char* conefilename = "D:\\model3d\\gts\\sphere5.gts";
	gtsparser.LoadGTS(filename);
	conegtsparser.SetTranRot(new xtVector3d(0.7,0,0),NULL);
	conegtsparser.LoadGTS(conefilename);

	osgViewer::CompositeViewer comViewer;

	osg::Group *gtsroot = new osg::Group;
	gtsroot->addChild(gtsparser.CreateScene());
	gtsroot->addChild(conegtsparser.CreateSceneWithTranRot());
	osgViewer::View *viewGts = new osgViewer::View;
	viewGts->setUpViewInWindow(200,200,640,480);
	viewGts->setSceneData(gtsroot);

	comViewer.addView(viewGts);

	return comViewer.run();
}

static inline int TestGTSCollisionMoreCompact()
{
	xtInterferecenTest interference;
	const char* filename = "D:\\model3d\\gts\\sphere5.gts";
	interference.LoadSurf(filename,filename);
	interference.SetTranRot(new xtVector3d(0.7,0,0),NULL,NULL,NULL);

	return interference.RunScene();
}

static inline int TestGTSCollisionMoreCompactLittleModel()
{
	xtInterferecenTest interference;
	const char* filename = "D:\\model3d\\gts\\testdata\\dataxy.gts";
	const char* xzfile   = "D:\\model3d\\gts\\testdata\\dataxz.gts";
	interference.LoadSurf(filename,xzfile);
	interference.SetTranRot(new xtVector3d(0.0,0,0),NULL,NULL,NULL);

	return interference.RunScene();
}

static inline int TestTriTriIntersection()
{
	xtTriTriCollideScene tritris;
	tritris.Initial();

	return tritris.RunRender();
}


int _tmain(int argc, _TCHAR* argv[])
{
	//return testObjScene();
	//return testObjOctreeScene();
	//return TestWRLParser();
	//return testFaceObjScene();
	//return TestWRLParserTextureDisplay();
	//return TestGTSParser();
	//return TestGTSCollision();
	//return TestGTSCollisionMoreCompact();
	//return TestTriTriIntersection();
	return TestGTSCollisionMoreCompactLittleModel();

	//return 0;
}

