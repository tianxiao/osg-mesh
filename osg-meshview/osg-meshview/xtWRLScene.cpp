#include "StdAfx.h"
#include "xtWRLScene.h"
#include <assert.h>
#include "../buildpyramidGeometry/osgcommon.h"
#include "xtWRLData.h"

xtWRLScene::xtWRLScene(void)
{
}


xtWRLScene::~xtWRLScene(void)
{
}

// from
// http://trac.openscenegraph.org/projects/osg//wiki/Support/Tutorials/Textures
osg::Geode *CreateGeo(xtWRLDataS *data) 
{
	std::vector<Vec3> &mVertices = data->mVertices;
	std::vector<xtIndexTria3> &mTris = data->mTris;
	std::vector<xtIndexCquad4> &mQuads = data->mQuads;
	std::vector<Vec2> &mTextureCoords = data->mTextureCoords;

	osg::Geode *temp = new osg::Geode;
	
	osg::Geometry *facemesh = new osg::Geometry;
	temp->addDrawable( facemesh );

	// Specify the vertices
	osg::Vec3Array *faceverts = new osg::Vec3Array;
	faceverts->reserve( data->mVertices.size() );
	for ( size_t i=0; i<mVertices.size(); ++i ) {
		Vec3 &temp = mVertices[i];
		faceverts->push_back( osg::Vec3(temp.x, temp.y, temp.z) );
	}
	facemesh->setVertexArray( faceverts );

	osg::DrawElementsUInt *facequads = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS,0 );
	facequads->reserve( data->mQuads.size()*4 );
	for ( size_t i=0; i<mQuads.size(); ++i ) {
		xtIndexCquad4 &temp = mQuads[i];
		for ( int i=0; i<4; ++i ) {
			facequads->push_back(temp.a[i]);
		}
	}
	facemesh->addPrimitiveSet(facequads);

	osg::DrawElementsUInt *facetrias = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
	facetrias->reserve( data->mTris.size()*3 );
	for ( size_t i=0; i<mTris.size(); ++i ) {
		xtIndexTria3 &tria = mTris[i];
		for ( int i=0; i<3; ++i ) {
			facetrias->push_back(tria.a[i]);
		}
	}
	facemesh->addPrimitiveSet(facetrias);

	// trunk the redundancy texture coordinate
	osg::Vec2Array *facetexcoords = new osg::Vec2Array(data->mVertices.size());
	assert(data->mTextureCoords.size()>=data->mVertices.size());
	for ( size_t i=0; i<mVertices.size(); ++i ) {
		(*facetexcoords)[i].set(mTextureCoords[i].x, mTextureCoords[i].y);
	}
	facemesh->setTexCoordArray(0,facetexcoords);

	return temp;
}
