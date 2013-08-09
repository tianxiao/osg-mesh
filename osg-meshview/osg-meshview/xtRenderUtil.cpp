#include "StdAfx.h"
#include "xtRenderUtil.h"
#include "../buildpyramidGeometry/osgcommon.h"
#include "./openmesh/xtCPMesh.h"
#include "./openmesh/xtOpenmeshHeader.h"
#include "linearmathcom.h"


osg::Group *xtRenderUtil::RenderCPMesh( xtCPMesh *mesh )
{
	osg::Group *group = new osg::Group;
	osg::Geode *geode = new osg::Geode;
	group->addChild( geode );

	std::vector<unsigned int> &faceidx = mesh->mFacepro;
	txMyOPMesh &m=mesh->mData->mesh;
	
	osg::Geometry *geo = new osg::Geometry;

	osg::Vec3Array *verts = new osg::Vec3Array;
	verts->reserve( faceidx.size()*3 );
	osg::Vec3Array *normal = new osg::Vec3Array;
	normal->reserve( faceidx.size()*3 );

	osg::Vec4Array *colors = new osg::Vec4Array;
	colors->reserve( faceidx.size()*3 );

	const float ccolorstep = 1.0/faceidx.size();
	float colorstep = ccolorstep;

	xtVector3f v[3];
	xtVector3f norm;
	int vcount=0;
	for ( size_t i=0; i<faceidx.size(); ++i ) {
		int fhandleidx = faceidx[i];
		txMyOPMesh::FaceHandle fhandle = m.face_handle(fhandleidx);
		txMyOPMesh::FVIter fvit=m.fv_begin( fhandle );
		for ( ; fvit!=m.fv_end(fhandle); ++fvit ) {
			txMyOPMesh::Point &p = m.point( fvit );
			v[vcount++] << p[0], p[1], p[2];
			verts->push_back( osg::Vec3( p[0], p[1], p[2] ) );
		}
		vcount=0;
		norm = ( v[1]-v[0] ).cross( v[2]-v[0] ); norm.normalize();
		normal->push_back( osg::Vec3(norm.x(), norm.y(), norm.z() ));
		normal->push_back( osg::Vec3(norm.x(), norm.y(), norm.z() ));
		normal->push_back( osg::Vec3(norm.x(), norm.y(), norm.z() ));
		colors->push_back( osg::Vec4(colorstep,0.0f,0.0f,1.0f) );
		colors->push_back( osg::Vec4(colorstep,0.0f,0.0f,1.0f) );
		colors->push_back( osg::Vec4(colorstep,0.0f,0.0f,1.0f) );
		colorstep = i*ccolorstep;
	}

	geo->setVertexArray( verts );
	geo->setColorArray( colors );
	geo->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
	geo->setNormalArray( normal );
	geo->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
	geo->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, verts->size() ));
	
	geode->addDrawable( geo );

	return group;
}
