#include "StdAfx.h"
#include "xtOctreeDisplayUtil.h"
#include "xtOctreeAdapter.h"
#include "xtGeometrySurfaceData.h"
#include "../buildpyramidGeometry/osgcommon.h"

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
	cpnode->addChild(RenderCollideList(surfaceI,cpI,xtColor(1.0,.0,.0,1.),false));
	cpnode->addChild(RenderCollideList(surfaceJ,cpJ,xtColor(.0,1.,0.,1.),false));
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
		normals->push_back(osg::Vec3(normal.x(),normal.y(),normal.z()));
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
