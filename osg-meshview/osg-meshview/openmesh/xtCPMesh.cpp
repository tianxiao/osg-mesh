#include "StdAfx.h"
#include "xtCPMesh.h"
#include "xtOpenmeshHeader.h"
#include <assert.h>
#include "../xtGeometrySurfaceData.h"
#include "../xtSplitBuilder.h"
//#include "../xtCollisionEngine.h"

xtCPMesh::xtCPMesh(void)
{
}


xtCPMesh::~xtCPMesh(void)
{
	delete this->mData;
	this->mData = NULL;
}

void xtCPMesh::LoadRawMesh(xtGeometrySurfaceDataS *surf)
{
	this->mSurf = surf;
	this->mData = new xtOpenMeshW;
	BuildRawMesh();
}

void xtCPMesh::LoadRawData(xtCollisionEntity *ce, xtAbosoluteVertexPool *abpool)
{
	assert(ce);
	assert(abpool);
	this->mCE = ce;
	this->mAbpool = abpool;

	Remesh();
}

void xtBuildRawMesh(xtGeometrySurfaceDataS *surf, xtOpenMeshW &data)
{
	std::vector<txMyOPMesh::VHandle> vhandles;
	vhandles.reserve( surf->verts.size() );
	for ( size_t i=0; i<surf->verts.size(); ++i ) {
		xtVector3d v = surf->verts[i];
		v = surf->tran + surf->rot*v;
		vhandles.push_back( data.mesh.add_vertex( txMyOPMesh::Point((float)v.x(), (float)v.y(), (float)v.z()) ));
	}

	std::vector<txMyOPMesh::VHandle> vhandle3(3);
	for ( size_t i=0; i<surf->indices.size(); ++i ) {
		xtIndexTria3 &tria = surf->indices[i];
		for ( int vidx=0; vidx<3; ++vidx ) {
			vhandle3[vidx] = vhandles[tria.a[vidx]];
		}
		data.mesh.add_face( vhandle3 );
	}

}

void xtCPMesh::DumpMeshOff()
{
	// write mesh to output.obj
	try
	{
		if ( !OpenMesh::IO::write_mesh(mData->mesh, "output.off") )
		{
			std::cerr << "Cannot write mesh to file 'output.off'" << std::endl;
			//return 1;
		}
	} catch ( std::exception& x )
	{
		std::cerr << x.what() << std::endl;
		//return 1;
	}
}

void xtCPMesh::BuildRawMesh()
{
	xtBuildRawMesh(this->mSurf, *(this->mData) );
}

void xtCPMesh::Remesh()
{
	std::vector<txMyOPMesh::VHandle> vhandles;
	for ( size_t i=0; i<mAbpool->mAbverts.size(); ++i ) {
		xtAbosoluteVertex *v = mAbpool->mAbverts[i];
		// first ignore the special case 
		assert( v->type!=ON_SURFACE_I && v->type!=ON_SRUFACE_J ) ;
		xtVector3d *p = v->splitpnt;
		vhandles.push_back( mData->mesh.add_vertex( txMyOPMesh::Point( (float)p->x(), (float)p->y(), (float)p->z() ) ) );
	}

	std::vector<txMyOPMesh::VHandle> vhandle3; 
	std::vector<txMyOPMesh::VHandle> vhandletem3(3);
	vhandle3.reserve(3);
	std::vector<std::vector<txMyOPMesh::VHandle> > vhandle3list;
	for ( size_t i=0; i<mCE->surfslot.size(); ++i ) {
		xtSurfaceSlot *ss = mCE->surfslot[i];
		txMyOPMesh::FHandle fhandle = mData->mesh.face_handle(ss->idx);
		txMyOPMesh::FVIter fvit = mData->mesh.fv_begin(fhandle);
		txMyOPMesh::FVIter fvend = mData->mesh.fv_end(fhandle);
		for ( ;fvit!=fvend;++fvit ) {
			vhandle3.push_back(fvit);
		}
		vhandle3list.push_back(vhandle3);
		mData->mesh.delete_face( fhandle, false );
		vhandle3.clear();
	}

	//mData->mesh.garbage_collection();

	for ( size_t i=0; i<mCE->surfslot.size(); ++i ) {
		xtSurfaceSlot *ss = mCE->surfslot[i];	
		vhandle3 = vhandle3list[i];
		//for ( txMyOPMesh::VFIter vfit=mData->mesh.vf_begin(fhandle); vfit!=mData->mesh.vf_end();
		for ( size_t triidx=0; triidx<ss->tris.size(); ++triidx ) {
			xtIndexTria3 &tria = ss->tris[triidx];
			for ( int vidx=0; vidx<3; ++vidx ) {
				//xtAbosoluteVertex *abv = ss->aftertrinodeslist
				int idx=tria.a[vidx];
				if ( idx<3 ) {
					vhandletem3[vidx]=mData->mesh.vertex_handle( mSurf->indices[ss->idx].a[idx] );
				} else {
					idx=idx-3;
					vhandletem3[vidx]=vhandles[ss->aftertrinodeslist[idx]->vIdInPool];
				}
			}
			std::swap( vhandletem3[1], vhandletem3[2] );
			mData->mesh.add_face( vhandletem3 );
			printf( "Add face %d\t--%d \n" , ss->idx, triidx);
			//vhandletem3.clear();
		}
		vhandle3.clear();
	}

	mData->mesh.garbage_collection();

	DumpMeshOff();
}
