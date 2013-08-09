#include "StdAfx.h"
#include "xtAwesomeGuiTest.h"
#include "xtAwesomGui.h"


xtAwesomeGuiTest::xtAwesomeGuiTest(void)
{
}


xtAwesomeGuiTest::~xtAwesomeGuiTest(void)
{
}

//extern Awesomium::WebCore *g_webCore;

void RunAwesomGui()
{
	xtAwesomiumImage::g_webCore = WebCore::Initialize( WebConfig() );

	//osg::ArgumentParser arguments( &argc, argv);

	//std::string url( "http://www.baidu.com");
	std::string url( "file:///E:/gitProject/osg-mesh/osg-meshview/osg-meshview/js2d/drawcoincident.html");

	int w=800, h=600;

	osg::ref_ptr<xtAwesomiumImage> image = new xtAwesomiumImage;
	image->LoadURL( url, w, h );

	osg::ref_ptr<osgViewer::InteractiveImageHandler> handler = 
		new osgViewer::InteractiveImageHandler( image.get() );
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage( image.get() );
	texture->setResizeNonPowerOfTwoHint( false );

	osg::ref_ptr<osg::Geometry> quad = 
		osg::createTexturedQuadGeometry( osg::Vec3(), osg::X_AXIS, -osg::Z_AXIS );
	quad->setEventCallback( handler.get() );
	quad->setCullCallback( handler.get() );

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( quad.get() );
	geode->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get() );
	geode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	geode->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

	osg::ref_ptr<osg::MatrixTransform> scene = new osg::MatrixTransform;

	scene->addChild( geode.get() );

	osgViewer::Viewer viwer;
	viwer.setSceneData( scene.get() );
	viwer.setUpViewInWindow( 200,200, 1000,800 );

	viwer.setCameraManipulator( new osgGA::TrackballManipulator );
	viwer.realize();

	while ( !viwer.done() ) {
		xtAwesomiumImage::g_webCore->Update();
		viwer.frame();
	}
	image = NULL;

}