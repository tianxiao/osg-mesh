#include "StdAfx.h"
#include "xtCPMesh.h"
#include "xtOpenmeshHeader.h"
#include <assert.h>
#include "../xtGeometrySurfaceData.h"
#include "../xtSplitBuilder.h"
#include <queue>
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

	AttachSSToMeshFaceTraitsOrder();

	Remesh();

	//TrackInterface();
	TagIstBoundaryHalfedge2();

	TagFace2();
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

// how to set the globale constant collective
OpenMesh::VPropHandleT<xtIntersectionPnt> vtype;
OpenMesh::FPropHandleT<xtIntersectFace> ftype;

/**
* Currently only consider the closed intersection boundary
* The open intersection is more tedious. e.g. if the start segment is not from the beginning
* The code should search from head and tail to get the whole segment
* And also there must be a counter to check if all the seperate parts have been collected!
*/
void xtCPMesh::AttachSSToMeshFaceTraitsOrder()
{
	mIstOrder.reserve( mAbpool->mAbverts.size() );

	for ( size_t i=0; i<mCE->surfslot.size(); ++i ) {
		xtSurfaceSlot *ss = mCE->surfslot[i];
		txMyOPMesh::FaceHandle fhandle = mData->mesh.face_handle( ss->idx );
		mData->mesh.data( fhandle ).SetSS( ss );
	}

	xtSurfaceSlot *startss = mCE->surfslot[0];
	const int startofstartofseg = startss->aftertrinodeslist[0]->vIdInPool;
	const int endidx = startss->aftertrinodeslist.size()-1;
	const int endofstartofseg   = startss->aftertrinodeslist[endidx]->vIdInPool;
	const int startssidx = startss->idx;
	int cursoridx = startssidx;
	xtSurfaceSlot *cursorss = startss;
	for ( size_t i=0; i<cursorss->aftertrinodeslist.size()-1; ++i ) {
		mIstOrder.push_back( cursorss->aftertrinodeslist[i]->vIdInPool );
	}
	int currstartvert = cursorss->aftertrinodeslist[0]->vIdInPool;
	int currendvert   = cursorss->aftertrinodeslist[cursorss->aftertrinodeslist.size()-1]->vIdInPool;
	std::vector<txMyOPMesh::FaceHandle> cirvertsfhandles;
	while ( true ) {
		cirvertsfhandles.clear();
		GetFaceVertexCircularFace( mData->mesh, mData->mesh.face_handle(cursoridx), cirvertsfhandles );
		for ( size_t fidx=0; fidx<cirvertsfhandles.size(); ++fidx ) {
			txMyOPMesh::FaceHandle fhandle = cirvertsfhandles[fidx];
			xtSurfaceSlot *ss = mData->mesh.data( fhandle ).SS();
			if ( !ss ) continue;
			int startvert = ss->aftertrinodeslist[0]->vIdInPool;
			int endvert   = ss->aftertrinodeslist[ss->aftertrinodeslist.size()-1]->vIdInPool;
			if ( currstartvert==startvert ) {
				continue;
			} else if ( currstartvert==endvert ) {
				continue;
			} else if ( currendvert==startvert ) {
				cursoridx     = ss->idx;
				if ( startssidx==cursoridx ) {
					goto OUT;
				}
				for ( size_t i=0; i<ss->aftertrinodeslist.size()-1; ++i ) { // keep the tail to push back in the next begin
					mIstOrder.push_back( ss->aftertrinodeslist[i]->vIdInPool );
				}
				currstartvert = startvert;
				currendvert   = endvert;
				break;
			} else if ( currendvert==endvert ) {
				cursoridx     = ss->idx;
				if ( startssidx==cursoridx ) {
					goto OUT;
				}
				for ( size_t i=ss->aftertrinodeslist.size()-1; i>0; --i ) { // keep the head to stored int the next 
					mIstOrder.push_back( ss->aftertrinodeslist[i]->vIdInPool );
				}
				currstartvert = endvert;
				currendvert   = startvert;
				break;
			}
		}
	}

OUT:
	; 
}

void xtCPMesh::Remesh()
{
	
	mData->mesh.add_property( vtype );
	mData->mesh.add_property( ftype );

	std::vector<txMyOPMesh::VHandle> vhandles;
	for ( size_t i=0; i<mAbpool->mAbverts.size(); ++i ) {
		xtAbosoluteVertex *v = mAbpool->mAbverts[i];
		// first ignore the special case 
		assert( v->type!=ON_SURFACE_I && v->type!=ON_SRUFACE_J ) ;
		xtVector3d *p = v->splitpnt;
		txMyOPMesh::VHandle vhandle = mData->mesh.add_vertex( txMyOPMesh::Point( (float)p->x(), (float)p->y(), (float)p->z() ) ) ;
		mData->mesh.property( vtype, vhandle ) = XT_BOUNDARY_SPLIT_PNT;
		mData->mesh.data( vhandle ).SetType( XT_BOUNDARY_SPLIT_PNT );
		vhandles.push_back( vhandle );
	}

	mVHandleidxs.reserve( vhandles.size() );
	for ( size_t i=0; i<vhandles.size(); ++i ) {
		mVHandleidxs.push_back( vhandles[i].idx() );
	}

	assert(vhandles.size());
	mNumIstPnts = vhandles.size();
	mIstStartPntIdx = vhandles[0].idx();

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
					// Originally I use the cache handle 
					// vhandle3list--vhandle3 but the idx may mis alligned
					vhandletem3[vidx]=mData->mesh.vertex_handle( mSurf->indices[ss->idx].a[idx] );
				} else {
					idx=idx-3;
					vhandletem3[vidx]=vhandles[ss->aftertrinodeslist[idx]->vIdInPool];
				}
			}
			// The ccw or cw may also be considered when feed into the OpenMesh API
			std::swap( vhandletem3[1], vhandletem3[2] );
			txMyOPMesh::FaceHandle fhandle = mData->mesh.add_face( vhandletem3 );
			mData->mesh.property( ftype, fhandle ) = XT_INTERSECT_FACE;
			printf( "Add face %d\t--%d \n" , ss->idx, triidx);
			//vhandletem3.clear();
		}
		vhandle3.clear();
	}

	mData->mesh.garbage_collection();

	DumpMeshOff();
}

/**
* This function should be called after the boundary point have been filled
* New mesh connectivity have been contructed.
*/
void xtCPMesh::TagIstBoundaryHalfedge() // actually this function tag the edge.
{
	assert( mIstOrder.size() );
	int vidx     = -1;
	int vidxnext = -1;
	const int numofedge = mIstOrder.size(); // actually the poly should be closed that is say there n points and n polylines
	for ( size_t i=0; i<mIstOrder.size(); ++i ) {
		vidx     = mIstOrder[i];
		vidxnext = mIstOrder[(i+1)%numofedge];
		txMyOPMesh::VertexHandle vhandle     = mData->mesh.vertex_handle( mVHandleidxs[vidx] );
		txMyOPMesh::VertexHandle vnexthandle = mData->mesh.vertex_handle( mVHandleidxs[vidxnext] );
		txMyOPMesh::VOHIter vohit=mData->mesh.voh_begin( vhandle );
		for ( ; vohit!=mData->mesh.voh_end( vhandle ); ++vohit ) {
			if ( mData->mesh.opposite_vh( vohit )==vnexthandle ) {
				txMyOPMesh::EdgeHandle eh = mData->mesh.edge_handle( vohit );
				mData->mesh.data( eh ).SetType( IST_EDGE ) ;
			}
		}
	}

	std::vector< txMyOPMesh::EdgeHandle> taggedeges;
	txMyOPMesh::EdgeIter eit = mData->mesh.edges_begin();
	for ( ; eit!=mData->mesh.edges_end(); ++eit ) {
		if ( mData->mesh.data( eit ).Type()==IST_EDGE ) {
			taggedeges.push_back( eit.handle() );
		}
	}

	//for ( size_t i=0; i<
}

void xtCPMesh::TagIstBoundaryHalfedge2()
{
	assert( mIstOrder.size() );
	int vidx     = -1;
	int vidxnext = -1;
	const int numofedge = mIstOrder.size(); // actually the poly should be closed that is say there n points and n polylines
	for ( size_t i=0; i<mIstOrder.size(); ++i ) {
		vidx     = mIstOrder[i];
		vidxnext = mIstOrder[(i+1)%numofedge];
		txMyOPMesh::VertexHandle vhandle     = mData->mesh.vertex_handle( mVHandleidxs[vidx] );
		txMyOPMesh::VertexHandle vnexthandle = mData->mesh.vertex_handle( mVHandleidxs[vidxnext] );
		txMyOPMesh::VOHIter vohit=mData->mesh.voh_begin( vhandle );
		for ( ; vohit!=mData->mesh.voh_end(vhandle); ++vohit ) {
			if ( mData->mesh.opposite_vh( vohit )==vnexthandle ) {
				mData->mesh.data( vohit ).SetType( ON_BOUNDARYO );
			}
		}
		txMyOPMesh::VIHIter vihit=mData->mesh.vih_begin( vhandle );
		for ( ; vihit!=mData->mesh.vih_end(vhandle); ++vihit ) {
			if ( mData->mesh.opposite_vh( vihit )==vnexthandle ) {
				mData->mesh.data( vihit ).SetType( ON_BOUNDARYO );
			}
		}
	}

	txMyOPMesh::HalfedgeIter heit = mData->mesh.halfedges_begin();
	for ( ; heit!=mData->mesh.halfedges_end(); ++heit ) {
		if ( mData->mesh.data( heit ).Type()==ON_BOUNDARYO ) 
			mHEBs.push_back( heit.handle().idx() );
	}
}

/**
* This trace have a fatal defect
* If the triangle is compirsed by two feature point then the intersection point will 
* missing due to the find short cut break;
*/
void xtCPMesh::TrackInterface()
{
	//mData->mesh.add_property( vtype );
	//std::vector<txMyOPMesh::HalfedgeHandle> istHalfEdgeList;
	const txMyOPMesh::VertexHandle startvh = mData->mesh.vertex_handle( mIstStartPntIdx );
	txMyOPMesh::VertexHandle cusurvh;
	//txMyOPMesh::VertexHandle backcursur;
	// Two criteria 
	// 1st check if the totoal iterator num is the mNumIstPnts;
	// 2nd check if the end pnt isn't start, or have no bit back 
	// here I use 2nd 

	bool siIstopen=false;
	bool isFind=false;
	int outvcount=0;
	mIstVertHandleIdxList.push_back( mIstStartPntIdx );
	cusurvh = startvh;
	while ( true ) { // cursurvh's idx() == -1; is not valid handle
		txMyOPMesh::VVIter vvit = mData->mesh.vv_begin( cusurvh );
		for ( ; vvit!=mData->mesh.vv_end( cusurvh ); ++vvit ) {
			if ( mData->mesh.data( vvit ).Type() == XT_BOUNDARY_SPLIT_PNT ) {
				if ( (mIstVertHandleIdxList.size()>1 && vvit.handle().idx()!=mIstVertHandleIdxList[mIstVertHandleIdxList.size()-2]) ||
					mIstVertHandleIdxList.size()==1) {
					cusurvh = vvit.handle();
					isFind = true;
					break;
				}
			}
		}
#if 1 // debug if there is circle in the inersection 
		for ( vvit=mData->mesh.vv_begin( cusurvh ); vvit!=mData->mesh.vv_end(cusurvh); ++vvit ) {
			if ( mData->mesh.data( vvit ).Type() == XT_BOUNDARY_SPLIT_PNT ) {
				outvcount++;
			}
		}
		if ( outvcount>2 ) {
			printf( "Ist Has Circle %d\t%d\n",cusurvh.idx(),outvcount );
		}
		outvcount=0;
#endif
		if ( !isFind ) {
			siIstopen = true;
			break;
		}

		if ( cusurvh == startvh ) {
			siIstopen = false;
			break;
		}
		mIstVertHandleIdxList.push_back( cusurvh.idx() );
		isFind = false;
		
	}
}

void xtCPMesh::TrackInterface2()
{
	
}

void xtCPMesh::TagFace()
{
	assert( mIstOrder.size()>2 ); // minimum 3 points can be a loop;
	int vidx     = -1;
	int vidxnext = -1;
	for ( size_t i=0; i<mIstOrder.size()-1; ++i ) {
		vidx     = mIstOrder[i];
		vidxnext = mIstOrder[i+1];
	}
	const int vidxstart     = mIstOrder[0];
	const int vidxstartnext = mIstOrder[1];

	const int randomfaceidx = 0; //  randomly pick a starting face, for face is atom
	/**
	* The following is not necessary the mesh's map can do the job 
	* The exit conditions is once the num of adjacent face is zero;
	* The tagged face is mark directly to the face
	* std::vector<txMyOPMesh::FaceHandle> adj;
	* std::queue<txMyOPMesh::FaceHandle> adjqueue;
	* adj.push_back( mData->mesh.face_handle(randomfaceidx) );
	*/
	int numadjs=1; // i put the random face idx in to the search queue.
	std::queue<txMyOPMesh::FaceHandle> adjqueue;
	txMyOPMesh::FaceHandle randomfacehandle = mData->mesh.face_handle( randomfaceidx );
	assert( randomfacehandle.idx()>-1 );
	adjqueue.push( randomfacehandle );
	bool isIstEdgeFace = false;
	bool isIstEdgeFaceCmp = false;
	while( !adjqueue.empty() ) {
		txMyOPMesh::FaceHandle fhandle = adjqueue.front();
		mData->mesh.data( fhandle ).SetType( RED );
		isIstEdgeFace = IsIstEdgeFace( fhandle.idx() );
		adjqueue.pop();
		/** using the face-face iterator
		txMyOPMesh::FFIter ffit = mData->mesh.ff_begin( fhandle );
		for ( ; ffit!=mData->mesh.ff_end(fhandle); ++ffit ) {
			if ( mData->mesh.data( ffit ).Type()==UN_TAGGEDFACE ) {
				isIstEdgeFaceCmp=IsIstEdgeFace(ffit.handle().idx());
				if ( isIstEdgeFace&&isIstEdgeFaceCmp ) {
					continue;
				} else {
					adjqueue.push( ffit.handle() );
				}
			}
		}
		**/

		// using the face-halfedge iterator
		txMyOPMesh::FHIter fhit=mData->mesh.fh_begin( fhandle );
		for ( ; fhit!=mData->mesh.fh_end( fhandle ); ++fhit ) {
			txMyOPMesh::FaceHandle oppfacehandle = mData->mesh.opposite_face_handle( fhit );
			txMyOPMesh::EdgeHandle seedgehandle  = mData->mesh.edge_handle( fhit );
			if ( mData->mesh.data( oppfacehandle ).Type()==UN_TAGGEDFACE ) {
				if ( mData->mesh.data( seedgehandle ).Type()==IST_EDGE ) {
					continue;
					//mData->mesh.data( oppfacehandle ).SetType( SPLIT_PNT );
				} else {
					adjqueue.push( oppfacehandle );
				}
			}
		}
	}

	// for debug
	DumpTaggedRed();
}

void xtCPMesh::TagFace2()
{
	assert( mIstOrder.size()>2 );

	const int randomfaceidx = 0;

	std::queue<txMyOPMesh::FaceHandle> adjqueue;
	txMyOPMesh::FaceHandle randomfacehandle = mData->mesh.face_handle( randomfaceidx );
	assert( randomfacehandle.idx()>-1 );
	adjqueue.push( randomfacehandle );

	while( !adjqueue.empty() ) {
		txMyOPMesh::FaceHandle fhandle = adjqueue.front();
		if ( mData->mesh.data( fhandle ).Type()==UN_TAGGEDFACE ) {
			goto ConditionLabel;
		} else {
			adjqueue.pop();
			continue;
		}
		ConditionLabel:
		mFacepro.push_back( fhandle.idx() );
		mData->mesh.data( fhandle ).SetType( RED );
		adjqueue.pop();

		// using the face-halfedge iterator
		txMyOPMesh::FHIter fhit=mData->mesh.fh_begin( fhandle );
		for ( ; fhit!=mData->mesh.fh_end( fhandle ); ++fhit ) {
			txMyOPMesh::FaceHandle oppfacehandle = mData->mesh.opposite_face_handle( fhit.current_halfedge_handle() );
			if ( 
				mData->mesh.data( oppfacehandle ).Type()==UN_TAGGEDFACE  &&
				mData->mesh.data( fhit.handle() ).Type()!=ON_BOUNDARYO 
				//mData->mesh.data( 
				) {
					//switch ( mData->mesh.data( fhit ).Type() ) {
					//case UN_TAGGED:
					//	printf( " %d \t UN_TAGGED\n", oppfacehandle.idx() );
					//	break;
					//case ON_BOUNDARYO:
					//	printf( " %d \t ON_BOUNDARYO\n", oppfacehandle.idx() );
					//	break;
					//}
				adjqueue.push( oppfacehandle );
			} else if ( mData->mesh.data( fhit ).Type()==ON_BOUNDARYO ) {
				mData->mesh.data( oppfacehandle ).SetType( GREEN );
				printf( "%%%%\n" );
			}
		}
	}

	// for debug
	DumpTaggedRed();

}

bool xtCPMesh::IsIstEdgeFace( int fhandleidx ) 
{
	txMyOPMesh::FaceHandle fhandle = mData->mesh.face_handle( fhandleidx );
	txMyOPMesh::FEIter feit = mData->mesh.fe_begin( fhandle );
	for ( ; feit!=mData->mesh.fe_end( fhandle); ++feit ) {
		if ( mData->mesh.data( feit ).Type()==IST_EDGE ) {
			return true;
		}
	}
	return false;
}

bool xtCPMesh::IsTwoFaceSeperatedByIstEdge(int f0, int f1)
{
	//txMyOPMesh::VertexHandle verhandle0 = mData->mesh.face_handle( f0 );
	//txMyOPMesh::VertexHandle verhandle1 = mData->mesh.face_handle( f1 );
	//txMyOPMesh::FVIter fvit0=mData->mesh.fv_begin( verhandle0 );
	//txMyOPMesh::FVIter fvit1=mData->mesh.fv_begin( verhandle1 );
	//for ( 
	return false;
}

void xtCPMesh::DumpTaggedRed()
{
	txMyOPMesh redmesh;
	redmesh=mData->mesh;
	
	txMyOPMesh::FaceIter fit = redmesh.faces_begin();
	for ( ; fit!=redmesh.faces_end(); ++fit ) {
		if ( redmesh.data( fit ).Type()!=RED ) {
			redmesh.delete_face( fit.handle(), true );
		}
	}
	redmesh.garbage_collection();


#if 0
	txMyOPMesh splitmesh;
	splitmesh = mData->mesh;

	fit=splitmesh.faces_begin();
	for ( ;fit!=splitmesh.faces_end(); ++fit ) {
		if ( splitmesh.data( fit ).Type()!=SPLIT_PNT ) 
			splitmesh.delete_face( fit.handle(), true );
	}
	splitmesh.garbage_collection();

	txMyOPMesh tsplitMesh;
	tsplitMesh = mData->mesh;
	//int vidx, vidxnext;
	//const int numofedge = mIstOrder.size();
	//for ( size_t i=0; i<mIstOrder.size(); ++i ) {
	//	vidx     = mIstOrder[i];
	//	vidxnext = mIstOrder[(i+1)%numofedge];

	//}

	/**
	* instead of using the edge I can use the halfedge and reference to the edge
	*/
	/**
	txMyOPMesh::EdgeIter eit = tsplitMesh.edges_begin();
	for ( ; eit!=tsplitMesh.edges_end(); ++eit ) {
		if ( tsplitMesh.data( eit ).Type()==IST_EDGE ) {
			txMyOPMesh::edgef feit=mData->mesh.
		}
	}
	**/
	/**
	txMyOPMesh::HalfedgeIter heit = tsplitMesh.halfedges_begin();
	for ( ; heit!=tsplitMesh.halfedges_end(); ++heit ) {
		if ( tsplitMesh.data( tsplitMesh.edge_handle( heit ) ).Type()==IST_EDGE ) {
			//tsplitMesh.data( tsplitMesh.face_handle( heit ) ).SetType(SPLIT_PNT);
			//tsplitMesh.opposite_face_handle( heit );
			//tsplitMesh.data( tsplitMesh.opposite_face_handle( heit ) ).SetType(SPLIT_PNT);
			tsplitMesh.data( tsplitMesh.face_handle( tsplitMesh.opposite_halfedge_handle( heit ) ) ).SetType(SPLIT_PNT);
			//tsplitMesh.data( tsplitMesh.face_handle(heit) ).SetType( SPLIT_PNT );
			//tsplitMesh.data( tsplitMesh.opposite_face_handle( heit ) ).SetType( SPLIT_PNT );
		}
	}
	**/
	/**
	fit=tsplitMesh.faces_begin();
	for ( ; fit!=tsplitMesh.faces_end(); ++fit ) {
		txMyOPMesh::FEIter feit=tsplitMesh.fe_begin( fit ) ;
		for ( ; feit!=tsplitMesh.fe_end(fit); ++feit ) {
			if ( tsplitMesh.data( feit ).Type()==IST_EDGE ) 
				tsplitMesh.data( fit ).SetType( SPLIT_PNT );
		}
	}
	**/
	fit=tsplitMesh.faces_begin();
	for ( ; fit!=tsplitMesh.faces_end(); ++fit ) {
		txMyOPMesh::FHIter fhit=tsplitMesh.fh_begin( fit );
		for ( ; fhit!=tsplitMesh.fh_end( fit ); ++fhit ) {
			if ( tsplitMesh.data( fhit ).Type()==ON_BOUNDARYO ) {
				tsplitMesh.data( fit ).SetType( SPLIT_PNT );
				break;
			}
		}
	}
	// why the number of the final face didn't comptatible with the 
	// totally halfedge, one given face may have several boundary halfedge.
	fit=tsplitMesh.faces_begin();
	for ( ; fit!=tsplitMesh.faces_end(); ++fit ) {
		if ( tsplitMesh.data( fit ).Type()!=SPLIT_PNT ) 
			tsplitMesh.delete_face( fit );
	}
	tsplitMesh.garbage_collection();
#endif

	// write mesh to output.obj
	try
	{
		if ( !OpenMesh::IO::write_mesh(redmesh, "redmeshoutput1.off") )
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
