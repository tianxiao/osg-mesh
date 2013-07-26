#include "StdAfx.h"
#include "xtOctreeDisplayUtil.h"
#include "xtOctreeAdapter.h"
#include "xtGeometrySurfaceData.h"
#include "../buildpyramidGeometry/osgcommon.h"
#include "xtSplitBuilder.h"

xtOctreeDisplayUtil::xtOctreeDisplayUtil(void)
{
}


xtOctreeDisplayUtil::~xtOctreeDisplayUtil(void)
{
}


osg::Geode *xtOctreeDisplayUtility::CreatemOctreeScene( xtOctreeAdapter *mOctreeDataA, xtPnt3<int> &mCubeCenter, std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame/*=true*/, bool debugon )
{
	osg::Geode *geode = new osg::Geode;

	if ( debugon ) {
		osg::Geometry *randPntsGeom = new osg::Geometry();

		osg::Vec3Array *randVertsArray = new osg::Vec3Array;

		std::vector<xtPnt3Adapter<double>> &adpPnts = mOctreeDataA->GetPnts();

		unsigned int nPnts = adpPnts.size();
		for ( size_t i=0; i<nPnts; ++i ) {
			xtPnt3Adapter<double> pnt = adpPnts[i];
			randVertsArray->push_back(osg::Vec3(pnt.x,pnt.y,pnt.z));
		}

		randPntsGeom->setVertexArray(randVertsArray);

		osg::Vec4Array *colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(1.0f,.0f,.0f,1.0f));
		randPntsGeom->setColorArray(colors);
		randPntsGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
		osg::StateSet *pointstate = randPntsGeom->getOrCreateStateSet();
		pointstate->setAttribute(new osg::Point(5.0f),osg::StateAttribute::ON);
		pointstate->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		randPntsGeom->setStateSet(pointstate);

		randPntsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,nPnts));

		geode->addDrawable(randPntsGeom);
	}

	for ( size_t i=0; i<mDumpLevelNodelist.size(); ++i ) {
		xtPnt3<int> center = mCubeCenter + (mDumpLevelNodelist[i]->mLB + mDumpLevelNodelist[i]->mRT).Half();
		xtPnt3<int> extend = (mDumpLevelNodelist[i]->mRT - mDumpLevelNodelist[i]->mLB);//.Half();
		osg::Vec3 osgcenter(center.x,center.y,center.z);
		osg::Vec3 osgextend(extend.x,extend.y,extend.z);
		osg::Box *levelbox = new osg::Box(osgcenter,osgextend.x(),osgextend.y(),osgextend.z());
		geode->addDrawable(new osg::ShapeDrawable(levelbox));
	}

	if ( isWireFrame ) {
		osg::StateSet *state2 = new osg::StateSet();
		state2->setAttribute( new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ));
		geode->setStateSet(state2);
	}

	
	return geode;
};

osg::Geode *xtOctreeDisplayUtility::CreateOctreeSection( double v, int axis, xtOctreeAdapter *mOctreeDataA, xtPnt3<int> &mCubeCenter, std::vector<xtOctreeNode<int> *> &mDumpLevelNodelist, bool isWireFrame/*=true*/, bool debugon/*=true*/)
{
	osg::Geode *geode = new osg::Geode;

	osg::Geometry *rectanglemesh = new osg::Geometry;

	osg::Vec3Array *quadverts = new osg::Vec3Array;
	quadverts->reserve( mDumpLevelNodelist.size()*4 );

	for ( size_t i=0; i<mDumpLevelNodelist.size(); ++i ) {
		xtPnt3<int> center = mCubeCenter + (mDumpLevelNodelist[i]->mLB + mDumpLevelNodelist[i]->mRT).Half();
		xtPnt3<int> extend = (mDumpLevelNodelist[i]->mRT - mDumpLevelNodelist[i]->mLB);//.Half();
		
		xtPnt3<int> worldLB = mCubeCenter + mDumpLevelNodelist[i]->mLB;
		xtPnt3<int> worldRT = mCubeCenter + mDumpLevelNodelist[i]->mRT;

		worldLB[axis] = v;
		worldRT[axis] = v;

		if ( axis==0 ) {
			quadverts->push_back( osg::Vec3(worldLB.x, worldLB.y, worldLB.z));
			quadverts->push_back( osg::Vec3(worldLB.x, worldRT.y, worldLB.z) );
			quadverts->push_back( osg::Vec3(worldLB.x, worldRT.y, worldRT.z) );
			quadverts->push_back( osg::Vec3(worldLB.x, worldLB.y, worldRT.z) );
		} else if ( axis==1 ) {
			quadverts->push_back( osg::Vec3(worldLB.x, worldLB.y, worldLB.z));
			quadverts->push_back( osg::Vec3(worldRT.x, worldRT.y, worldLB.z) );
			quadverts->push_back( osg::Vec3(worldRT.x, worldRT.y, worldRT.z) );
			quadverts->push_back( osg::Vec3(worldLB.x, worldLB.y, worldRT.z) );
		} else if ( axis==2 ) {
			quadverts->push_back( osg::Vec3(worldLB.x, worldLB.y, worldLB.z));
			quadverts->push_back( osg::Vec3(worldRT.x, worldLB.y, worldLB.z) );
			quadverts->push_back( osg::Vec3(worldRT.x, worldRT.y, worldRT.z) );
			quadverts->push_back( osg::Vec3(worldLB.x, worldRT.y, worldRT.z) );
		}

	}	

	{
		osg::Vec4Array *colors = new osg::Vec4Array;
		colors->push_back( osg::Vec4(0.0f, 0.0f, 1.0f,1.0f));

		osg::Vec3Array *normals = new osg::Vec3Array;
		if ( axis==0 ) {
			normals->push_back( osg::Vec3(1.0, 0, 0 ));
		} else if ( axis==1 ) {
			normals->push_back( osg::Vec3(0.0, 1.0, 0 ));
		} else if ( axis==2 ) {
			normals->push_back( osg::Vec3(0.0, 0.0, 1.0 ));
		}
		

		rectanglemesh->setVertexArray( quadverts );
		rectanglemesh->setColorArray( colors );
		rectanglemesh->setColorBinding( osg::Geometry::BIND_OVERALL);
		rectanglemesh->setNormalArray( normals );
		rectanglemesh->setNormalBinding( osg::Geometry::BIND_OVERALL );

		rectanglemesh->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, quadverts->size()) );

	}

	if ( isWireFrame ) {
		osg::StateSet *state2 = new osg::StateSet();
		state2->setAttribute( new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ));
		geode->setStateSet(state2);
	}

	geode->addDrawable( rectanglemesh );

	return geode;

}

osg::Geode *xtOctreeDisplayUtility::DisplayxtGeometrySurfaceDataS(xtGeometrySurfaceDataS *surface, bool isWireFrame)
{
	osg::Geode *geode = new osg::Geode;

	osg::Geometry *surfacemesh = new osg::Geometry;

	osg::Vec3Array *verts = new osg::Vec3Array;
	verts->reserve( surface->indices.size()*3 );
	osg::Vec3Array *normals = new osg::Vec3Array;
	normals->reserve( surface->indices.size()*3 );

	for ( size_t i=0; i<surface->indices.size(); ++i ) {
		xtIndexTria3 tria = surface->indices[i];
		/**** for Index debug
		if ( tria.a[0]>=surface->verts.size() ||
			tria.a[1]>=surface->verts.size() ||
			tria.a[2]>=surface->verts.size()) {
			printf("%d\n",i);
		}
		****/
		xtVector3d pa = surface->verts[tria.a[0]];
		xtVector3d pb = surface->verts[tria.a[1]];
		xtVector3d pc = surface->verts[tria.a[2]];
		verts->push_back(osg::Vec3((float)pa.x(), (float)pa.y(), (float)pa.z()));
		verts->push_back(osg::Vec3((float)pb.x(), (float)pb.y(), (float)pb.z()));
		verts->push_back(osg::Vec3((float)pc.x(), (float)pc.y(), (float)pc.z()));
		xtVector3d pba = pb - pa;
		xtVector3d pca = pc - pa;
		xtVector3d normal = pba.cross(pca);
		osg::Vec3 norm(normal.x(), normal.y(), normal.z());
		normals->push_back(norm);//normals->push_back(norm);normals->push_back(norm);
	}

	osg::Vec4Array *colors = new osg::Vec4Array;
	colors->push_back( osg::Vec4(0.0f, 0.0f, 1.0f,1.0f));

	surfacemesh->setVertexArray(verts);
	surfacemesh->setColorArray(colors);
	surfacemesh->setColorBinding(osg::Geometry::BIND_OVERALL);
	surfacemesh->setNormalArray(normals);
	surfacemesh->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	surfacemesh->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES,0, verts->size() ));

	if ( isWireFrame ) {
		osg::StateSet *state2 = new osg::StateSet();
		state2->setAttribute( new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ));
		geode->setStateSet(state2);
	}

	geode->addDrawable( surfacemesh );



	return geode;
}

osg::Node *xtOctreeDisplayUtility::DisplayxtGeometrySurfaceDataWithTranRot(xtGeometrySurfaceDataS *surface, bool isWireFrame/*=true*/)
{
	osg::Geode *geode = DisplayxtGeometrySurfaceDataS(surface,isWireFrame);

	//osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
	osg::MatrixTransform *trans = new osg::MatrixTransform;
	trans->setMatrix(osg::Matrix::translate(surface->tran.x(),surface->tran.y(),surface->tran.z()));
	trans->addChild(geode);

	return trans;
}

osg::Node *xtOctreeDisplayUtility::RenderCollided(xtGeometrySurfaceDataS *surfaceI, xtGeometrySurfaceDataS *surfaceJ, std::vector<xtCollidePair> &pairs)
{
	std::vector<int> cpI, cpJ;
	cpI.reserve(pairs.size());
	cpJ.reserve(pairs.size());
	for ( size_t i=0; i<pairs.size(); ++i ) {
		cpI.push_back(pairs[i].i);
		cpJ.push_back(pairs[i].j);
	}

	osg::Group *cpnode = new osg::Group;
	osg::Geode *transparent = RenderCollideList(surfaceI,cpI,xtColor(0.5,0.5,0.0,1.),false);
	if ( true ) {
		osg::StateSet *state2 = new osg::StateSet();
		state2->setMode( GL_BLEND, osg::StateAttribute::ON );
		state2->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

		// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
		state2->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

		// Conversely, disable writing to depth buffer so that
		// a transparent polygon will allow polygons behind it to shine thru.
		// OSG renders transparent polygons after opaque ones.
		osg::Depth* depth = new osg::Depth;
		depth->setWriteMask( false );
		state2->setAttributeAndModes( depth, osg::StateAttribute::ON );
 
		// Disable conflicting modes.
		state2->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		transparent->setStateSet(state2);
	}
	cpnode->addChild(transparent);
	cpnode->addChild(RenderCollideList(surfaceJ,cpJ,xtColor(0.5,0.0,0.5,1.),false));
	cpnode->addChild(RenderCollideList(surfaceI,cpI,xtColor(0.0,0.0,0.0,1.)));
	cpnode->addChild(RenderCollideList(surfaceJ,cpJ,xtColor(0.0,0.0,0.0,1.)));
	
	
	cpnode->setName("Collision Pair");

	return cpnode;
}

osg::Geode *xtOctreeDisplayUtility::RenderCollideList(xtGeometrySurfaceDataS *surface, std::vector<int> &indices, xtColor color, bool isWireFrame)
{
	osg::Geode *geode = new osg::Geode;

	osg::Geometry *partialmesh = new osg::Geometry;

	osg::Vec3Array *verts = new osg::Vec3Array;
	verts->reserve( indices.size()*3 );
	osg::Vec3Array *normals = new osg::Vec3Array;
	normals->reserve( indices.size() );

	for ( size_t i=0; i<indices.size(); ++i ) {
		xtIndexTria3 &tria = surface->indices[indices[i]];
		xtPointsTria pointtria(tria,surface);
		verts->push_back(osg::Vec3((float)pointtria.pa.x(),(float)pointtria.pa.y(),(float)pointtria.pa.z()));
		verts->push_back(osg::Vec3((float)pointtria.pb.x(),(float)pointtria.pb.y(),(float)pointtria.pb.z()));
		verts->push_back(osg::Vec3((float)pointtria.pc.x(),(float)pointtria.pc.y(),(float)pointtria.pc.z()));
		xtVector3d normal = pointtria.Normal();
		normals->push_back(osg::Vec3((float)normal.x(),(float)normal.y(),(float)normal.z()));
	}

	osg::Vec4Array *colors = new osg::Vec4Array;
	colors->push_back( osg::Vec4(color.r,color.b,color.g,color.alpha));

	partialmesh->setVertexArray(verts);
	partialmesh->setColorArray(colors);
	partialmesh->setColorBinding(osg::Geometry::BIND_OVERALL);
	partialmesh->setNormalArray(normals);
	partialmesh->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	partialmesh->addPrimitiveSet(new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES,0,verts->size() ));

	if ( isWireFrame ) {
		osg::StateSet *state2 = new osg::StateSet();
		state2->setAttribute( new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ));
		geode->setStateSet(state2);
	}

	geode->addDrawable(partialmesh);

	return geode;
}

osg::Geode *xtOctreeDisplayUtility::RenderTriangle(xtTriangle *tri, xtColor color, bool isWireFrame/*=true*/)
{
	osg::Geode *geode = new osg::Geode;

	osg::Geometry *trishape = new osg::Geometry;

	osg::Vec3Array *verts = new osg::Vec3Array;
	verts->reserve( 3 );
	osg::Vec3Array *normals = new osg::Vec3Array;
	normals->reserve( 1 );

	verts->push_back( osg::Vec3((float)tri->pa.x(),(float)tri->pa.y(),(float)tri->pa.z()));
	verts->push_back( osg::Vec3((float)tri->pb.x(),(float)tri->pb.y(),(float)tri->pb.z()));
	verts->push_back( osg::Vec3((float)tri->pc.x(),(float)tri->pc.y(),(float)tri->pc.z()));

	xtVector3d norm = tri->TriNormal();

	osg::Vec4Array *colors = new osg::Vec4Array;
	colors->push_back( osg::Vec4(color.r,color.b,color.g,color.alpha));

	normals->push_back(osg::Vec3((float)norm.x(),(float)norm.y(),(float)norm.z()));

	trishape->setVertexArray(verts);
	trishape->setColorArray(colors);
	trishape->setColorBinding(osg::Geometry::BIND_OVERALL);
	trishape->setNormalArray(normals);
	trishape->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	trishape->addPrimitiveSet(new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES,0,3 ));

	if ( isWireFrame ) {
		osg::StateSet *state2 = new osg::StateSet();
		state2->setAttribute( new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ));
		geode->setStateSet(state2);
	}

	geode->addDrawable(trishape);

	return geode;
}

osg::Geode *xtOctreeDisplayUtility::RenderSplitSegments(xtSplitBuilder *splitBuilder, xtColor color, float linewidthva)
{
	osg::Geode *geode = new osg::Geode;

	osg::Geometry *splitline = new osg::Geometry;

	osg::StateSet *stateset = new osg::StateSet;
	osg::LineWidth *linewidth = new osg::LineWidth;
	linewidth->setWidth(linewidthva);
	stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	splitline->setStateSet(stateset);

    // set the colors as before, plus using the above
    osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(color.r,color.g,color.b,color.alpha));
    splitline->setColorArray(colors);
    splitline->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec3Array *verts = new osg::Vec3Array;
	for ( size_t i=0; i<splitBuilder->mSharedSplitSegList.size(); ++i ) {
		xtVector3d &p0 = *(splitBuilder->mSharedSplitSegList[i]->seg0);
		xtVector3d &p1 = *(splitBuilder->mSharedSplitSegList[i]->seg1);
		verts->push_back( osg::Vec3( (float)(p0.x()),(float)(p0.y()),(float)(p0.z())) );
		verts->push_back( osg::Vec3( (float)(p1.x()),(float)(p1.y()),(float)(p1.z())) );
	}

	splitline->setVertexArray( verts );
	splitline->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES,0,verts->size()) );

	geode->addDrawable(splitline);
	return geode;
}

osg::Geode *xtOctreeDisplayUtility::RenderSplitSegmentsWithCyliner(xtSplitBuilder *splitBuilder, xtColor color, float radius/*=4.0*/)
{
	osg::Geode *geode = new osg::Geode;

	for ( size_t i=0; i<splitBuilder->mSharedSplitSegList.size(); ++i ) {
		xtVector3d &p0 = *(splitBuilder->mSharedSplitSegList[i]->seg0);
		xtVector3d &p1 = *(splitBuilder->mSharedSplitSegList[i]->seg1);
		geode->addDrawable( new osg::ShapeDrawable( CreateCyliner(p0,p1,radius) ) );
	}

	// Set the color of the cylinder that extends between the two points.
	osg::Material *pMaterial = new osg::Material;
	osg::Vec4 CylinderColor((float)color.r,(float)color.g,(float)color.b,(float)color.alpha);
	pMaterial->setDiffuse( osg::Material::FRONT, CylinderColor);
	geode->getOrCreateStateSet()->setAttribute( pMaterial, osg::StateAttribute::OVERRIDE );


	return geode;
}

osg::Geode *xtOctreeDisplayUtility::RenderRaySegment(xtSplitBuilder *sb)
{
	const float radiussp = 0.002;
	const float radisucy = radiussp/2.;
	const xtColor color(0.0,1.0,0.0,1.0);

	osg::Geode *geode = new osg::Geode;

	for ( size_t i=0; i<sb->mDebugedge.size(); ++i ) {
		xtVector3d &startPnt = sb->mDebugedge[i].start;
		xtVector3d &endPnt   = sb->mDebugedge[i].end;
		xtVector3d &oriPnt   = sb->mDebugedge[i].oriend;
		osg::Vec3 sp((float)startPnt.x(),(float)startPnt.y(),(float)startPnt.z());
		osg::Vec3 ep((float)endPnt.x(),(float)endPnt.y(),(float)endPnt.z());
		osg::Sphere *sphere = new osg::Sphere( sp , radiussp );
		geode->addDrawable( new osg::ShapeDrawable(sphere) );
		geode->addDrawable( new osg::ShapeDrawable( CreateCyliner(startPnt,endPnt,radisucy)) );
		geode->addDrawable( new osg::ShapeDrawable( CreateCyliner(endPnt, oriPnt, radisucy/2.)) );
	}

	osg::Material *pMaterial = new osg::Material;
	osg::Vec4 CylinderColor((float)color.r,(float)color.g,(float)color.b,(float)color.alpha);
	pMaterial->setDiffuse( osg::Material::FRONT, CylinderColor);
	geode->getOrCreateStateSet()->setAttribute( pMaterial, osg::StateAttribute::OVERRIDE );

	return geode;
}


osg::Geode *xtOctreeDisplayUtility::RednerSplitPntsAsSphere(xtSplitBuilder *splitBuilder, xtColor color, float radius/*=4.0*/)
{
	osg::Geode *geode = new osg::Geode;

	for ( size_t i=0; i<splitBuilder->mSharedSplitPoints.size(); ++i ) {
		xtVector3d &center = *(splitBuilder->mSharedSplitPoints[i]);
		osg::Sphere *sphere = new osg::Sphere(osg::Vec3(center.x(),center.y(),center.z()),radius);
		geode->addDrawable( new osg::ShapeDrawable( sphere ) );
	}

	osg::Material *pMaterial = new osg::Material;
	osg::Vec4 CylinderColor((float)color.r,(float)color.g,(float)color.b,(float)color.alpha);
	pMaterial->setDiffuse( osg::Material::FRONT, CylinderColor);
	geode->getOrCreateStateSet()->setAttribute( pMaterial, osg::StateAttribute::OVERRIDE );

	return geode;

}

/**
// http://www.thjsmith.com/40/cylinder-between-two-points-opengl-c
osg::ref_ptr geode = new osg::Geode;
osg::Vec3 center;
float height;
osg::ref_ptr cylinder;
osg::ref_ptr cylinderDrawable;
osg::ref_ptr pMaterial;

height = (StartPoint- EndPoint).length();
center = osg::Vec3( (StartPoint.x() + EndPoint.x()) / 2, (StartPoint.y() + EndPoint.y()) / 2, (StartPoint.z() + EndPoint.z()) / 2);

// This is the default direction for the cylinders to face in OpenGL
osg::Vec3 z = osg::Vec3(0,0,1);

// Get diff between two points you want cylinder along
osg::Vec3 p = (StartPoint ¨C EndPoint);

// Get CROSS product (the axis of rotation)
osg::Vec3 t = z ^ p;

// Get angle. length is magnitude of the vector
double angle = acos( (z * p) / p.length());

// Create a cylinder between the two points with the given radius
cylinder = new osg::Cylinder(center,radius,height);
cylinder->setRotation(osg::Quat(angle, osg::Vec3(t.x(), t.y(), t.z())));

cylinderDrawable = new osg::ShapeDrawable(cylinder );
geode->addDrawable(cylinderDrawable);

// Set the color of the cylinder that extends between the two points.
pMaterial = new osg::Material;
pMaterial->setDiffuse( osg::Material::FRONT, CylinderColor);
geode->getOrCreateStateSet()->setAttribute( pMaterial, osg::StateAttribute::OVERRIDE );

// Add the cylinder between the two points to an existing group
pAddToThisGroup->addChild(geode);
*/
osg::Cylinder *xtOctreeDisplayUtility::CreateCyliner(xtVector3d &start, xtVector3d &end, double radius)
{
	osg::Cylinder *cylin;// = new osg::Cylinder;

	xtVector3d dir = end-start;
	double height = dir.norm();
	xtVector3d center = (start+end)/2.0;

	xtVector3d zdir(0.0,0.0,1.0);
	xtVector3d quaterAxis = dir.cross(zdir)/height;

	double angle = acos(dir.dot(zdir)/height);

	cylin = new osg::Cylinder(osg::Vec3((float)center.x(),(float)center.y(),(float)center.z()), (float)radius, (float)height);
	
	cylin->setRotation(osg::Quat( -angle, osg::Vec3((float)quaterAxis.x(), (float)quaterAxis.y(), (float)quaterAxis.z()) ) );
	

	return cylin;
}





















