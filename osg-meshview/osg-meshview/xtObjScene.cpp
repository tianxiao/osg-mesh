#include "StdAfx.h"
#include "xtObjScene.h"
#include <string>
#include <iostream>
#include "./tinyobjloader-master/tiny_obj_loader.h"

#include "xtTinyIMPL.h"



xtObjScene::xtObjScene(void)
{
}


xtObjScene::~xtObjScene(void)
{
	delete this->mRawObjData;
}

bool xtObjScene::LoadObj(const char *basepath,const char *filename)
{
  mRawObjData = new xtObjShapeImpl;

  std::cout << "Loading " << filename << std::endl;

  //std::vector<tinyobj::shape_t> shapes;
  std::string err = tinyobj::LoadObj(mRawObjData->mShapes, filename, basepath);

  if (!err.empty()) {
    std::cerr << err << std::endl;
    return false;
  }

  return true;
}

osg::Geode *xtObjScene::CreateObjScene()
{

	osg::Geode* geode = new osg::Geode();
	for ( size_t i=0; i<mRawObjData->mShapes.size(); ++i ) {
		geode->addDrawable( CreatObjScene( static_cast<void *>( &(mRawObjData->mShapes[i]) ) ) );
	}

	return geode;
}

osg::Geometry* xtObjScene::CreatObjScene(void *shapev)
{
	shape_t *shape = static_cast<shape_t *>(shapev);

	
	osg::Geometry* trisGeom = new osg::Geometry();

	osg::Vec3Array *verts = new osg::Vec3Array;
	osg::Vec3Array *norms = new osg::Vec3Array;
	osg::Vec3Array *colos = new osg::Vec3Array;

	for ( size_t i=0; i<shape->mesh.indices.size()/3; ++i ) {

		//int a,b,c;
		const int a = shape->mesh.indices[3*i]; 
		const int b = shape->mesh.indices[3*i+1];
		const int c = shape->mesh.indices[3*i+2];
		osg::Vec3 pa(
			shape->mesh.positions[3*a],
			shape->mesh.positions[3*a+1],
			shape->mesh.positions[3*a+2]
			);
		osg::Vec3 pb(
			shape->mesh.positions[3*b],
			shape->mesh.positions[3*b+1],
			shape->mesh.positions[3*b+2]
			);
		osg::Vec3 pc(
			shape->mesh.positions[3*c],
			shape->mesh.positions[3*c+1],
			shape->mesh.positions[3*c+2]
			);
		verts->push_back( pa );
		verts->push_back( pb );
		verts->push_back( pc );
		if ( shape->mesh.normals.size() ) {
			// Bind per vertex, Bind per normal;
			osg::Vec3 na, nb, nc;
			na.set( shape->mesh.normals[3*a], shape->mesh.normals[3*a+1], shape->mesh.normals[3*a+2] );
			na.set( shape->mesh.normals[3*b], shape->mesh.normals[3*b+1], shape->mesh.normals[3*b+2] );
			na.set( shape->mesh.normals[3*c], shape->mesh.normals[3*c+1], shape->mesh.normals[3*c+2] );
			norms->push_back(na);
			norms->push_back(nb);
			norms->push_back(nc);
		} else {
				osg::Vec3 normalperface;
				normalperface = ((pc-pa)^(pb-pa));
				if ( normalperface.length2() < 0.000001 ) {
					normalperface.set(1.0,0.0,0.0);
				} else {
					normalperface.normalize();//.normalize();
				}
				
		norms->push_back( normalperface );
		norms->push_back( normalperface );
		norms->push_back( normalperface );
			};
	}

	

	osg::Vec4Array *colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f,.0f,.0f,1.0f));
	{
		trisGeom->setVertexArray(verts);
		
		trisGeom->setColorArray(colors);
		trisGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

		trisGeom->setNormalArray( norms );
		trisGeom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

		trisGeom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,verts->size()) );
	
		//geode->addDrawable( trisGeom );
	}


	return trisGeom;
}


