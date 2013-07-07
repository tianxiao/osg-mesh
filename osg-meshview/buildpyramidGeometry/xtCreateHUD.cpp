#include "StdAfx.h"
#include "xtCreateHUD.h"


xtCreateHUD::xtCreateHUD(void)
{
}


xtCreateHUD::~xtCreateHUD(void)
{
}

osg::Node *createHUD(osgText::Text *updateText)
{
	osg::Camera *hudCamera = new osg::Camera;
	hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	hudCamera->setProjectionMatrixAsOrtho2D(0,1280,0,1024);
	hudCamera->setViewMatrix(osg::Matrix::identity());
	hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);

	//std::string timesFont(

	osg::Vec3 position(150.f,800.0f,0.0f);
	osg::Vec3 delta(0.0f,-60.0f,0.0f);

	{
		osg::Geode *geode = new osg::Geode;
		osg::StateSet *stateset = geode->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		geode->setName("simple");
		hudCamera->addChild(geode);

		osgText::Text *text = new osgText::Text;
		geode->addDrawable( text );

		text->setText("Picking in head up Displays is simple!");
		text->setCharacterSize(10.f);
		text->setPosition(position);

		position+=delta;
	}

	for ( int i=0; i<5; ++i ) {
		osg::Vec3 dy(0.0f,-30.0f,0.0f);
		osg::Vec3 dx(120.0f,0.0f,0.0f);
		osg::Geode *geode = new osg::Geode;
		osg::StateSet *stateset = geode->getOrCreateStateSet();
		const char *opts[] = {"one", "Two", "Three", "January", "Feb", "2003"};
		osg::Geometry *quad = new osg::Geometry;
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		std::string name = "subOption";
		name+=" ";
		name+=std::string(opts[i]);
		geode->setName(name);
		osg::Vec3Array *vertices = new osg::Vec3Array(4);
		osg::Vec4Array *colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(0.4-0.1*i,0.1*i,0.2*i,1.0));
		quad->setColorArray(colors);
		quad->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
		(*vertices)[0] = position;
		(*vertices)[1] = position+dx;
		(*vertices)[2] = position+dx+dy;
		(*vertices)[3] = position+dy;
		quad->setVertexArray(vertices);
		quad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
		geode->addDrawable(quad);
		hudCamera->addChild(geode);

		position+=delta;
	}

	{
		osg::Geode *geode = new osg::Geode;
		osg::StateSet *stateset = geode->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		geode->setName("The text Label");
		geode->addDrawable(updateText);
		hudCamera->addChild(geode);

		updateText->setCharacterSize(10.f);
		updateText->setColor(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
		updateText->setText("");
		updateText->setPosition(position);
		updateText->setDataVariance(osg::Object::DYNAMIC);

		position += delta;
	}

	return hudCamera;
}
