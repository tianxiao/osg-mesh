#include "StdAfx.h"
#include "xtSplitBuilder.h"
#include "xtCollisionEngine.h"
#include "xtGeometrySurfaceData.h"
#include "xtRayTriOverlay.h"
#include <algorithm>
#include <list>
#include <queue>
#include "xtLog.h"
#include "./trianglev2/xtTrianglePLSG.h"

void xtCollisionEntity::DestroyMem()
{
	for ( size_t i=0; i<surfslot.size(); ++i ) {
		// TODO!!!
		for ( size_t svidx=0; svidx<surfslot[i]->pointsOnSurfVerbos.size(); ++svidx ) {
			delete surfslot[i]->pointsOnSurfVerbos[svidx];
			surfslot[i]->pointsOnSurfVerbos[svidx] = NULL;
		}
		for ( size_t segwboi=0; segwboi<surfslot[i]->segwbo.size(); ++segwboi ) {
			delete surfslot[i]->segwbo[segwboi];
			surfslot[i]->segwbo[segwboi] = NULL;
		}
		
		delete surfslot[i];
		surfslot[i] = NULL;
	}

	for ( size_t i=0; i<segslot.size(); ++i ) {
		delete segslot[i];
		segslot[i] = NULL;
	}
}

void xtCollisionEntity::InitializeCollisionEntity(xtGeometrySurfaceDataS *surf, std::vector<xtCollidePair> &pairs, xtSurfaceCat surfcat)
{
	for ( size_t i=0; i<pairs.size(); ++i ) {
		int idx;
		if ( XTSURFACEI==surfcat ) {
			idx = pairs[i].i;
		} else if ( XTSURFACEJ==surfcat ) {
			idx = pairs[i].j;
		} else {
			assert(false);
		}

		xtSurfaceSlot *temss = GetFaceSlotExit(idx);
		if ( temss ) {
			if ( XTSURFACEI==surfcat ) {
				temss->cflist.push_back(pairs[i].j);
			} else if ( XTSURFACEJ==surfcat ) {
				temss->cflist.push_back(pairs[i].i);
			} else {
				assert(false);
			}
			continue;
		}

		xtIndexTria3 &tria = surf->indices[idx];
		xtSurfaceSlot *ss = new xtSurfaceSlot;
		ss->idx = idx;
		ss->segs[0] = GetOrCreateSeg(tria.a[0], tria.a[1]);
		ss->segs[1] = GetOrCreateSeg(tria.a[1], tria.a[2]);
		ss->segs[2] = GetOrCreateSeg(tria.a[2], tria.a[0]);
		if ( XTSURFACEI==surfcat ) {
			ss->cflist.push_back(pairs[i].j);
		} else if ( XTSURFACEJ==surfcat ) {
			ss->cflist.push_back(pairs[i].i);
		} else {
			assert(false);
		}
		
		surfslot.push_back(ss);
	}
}

void xtCollisionEntity::AddSplitSegmentToFace(const int fi, xtSegment *seg)
{
	xtSurfaceSlot *ss = GetFaceSlotExit(fi);
	assert(ss);
	ss->segsonsurf.push_back(seg);
}

void xtCollisionEntity::AddSplitSegmentRBToFace(const int fi, xtSegmentRobust *segrb)
{
	xtSurfaceSlot *ss = GetFaceSlotExit(fi);
	assert(ss);
	ss->segrobust.push_back(segrb);
}

void xtCollisionEntity::AddSegWBOToFace(const int fi, xtSegmentWBO *seg)
{
	xtSurfaceSlot *ss = GetFaceSlotExit(fi);
	assert(ss);
	ss->segwbo.push_back(seg);
}


xtSurfaceSlot *xtCollisionEntity::GetFaceSlotExit(int fi)
{
	for ( size_t i=0; i<surfslot.size(); ++i ) {
		if ( surfslot[i]->idx==fi ) {
			return surfslot[i];
		}
	}
	return NULL;
}

xtSegmentSlot *xtCollisionEntity::GetOrCreateSeg(int i0, int i1)
{
	xtSegmentSlot *temp = SearchSeg(i0,i1);
	if ( temp ) {
		return temp;
	} else { // not exist create new one
		temp = new xtSegmentSlot;
		temp->startIdx = i0;
		temp->endIdx = i1;
		segslot.push_back(temp);
		return temp;
	}
}

xtSegmentSlot *xtCollisionEntity::SearchSeg(int i0, int i1)
{
	xtSegmentSlot *temp = NULL;
	for ( size_t i=0; i<segslot.size(); ++i ) {
		if ( ( segslot[i]->startIdx==i0&&segslot[i]->endIdx==i1 ||
			segslot[i]->startIdx==i1&&segslot[i]->endIdx==i0 ) ) {
			return segslot[i];
		}
	}

	return temp;
}


xtSplitBuilder::xtSplitBuilder(void)
{

}


xtSplitBuilder::~xtSplitBuilder(void)
{
	DestroyMem();
}

void xtSplitBuilder::SetCE(xtCollisionEngine *ce) 
{ 
	this->mCE = ce; 
	InitializeCollisionEntity();
}

void xtSplitBuilder::Split()
{
	SplitPnt(mPSI,mPSJ,mSFMI,mCE->mSurfI,mCE->mSurfJ);
	SplitPnt(mPSJ,mPSI,mSFMJ,mCE->mSurfJ,mCE->mSurfI);
	int methodid = 2;
	if ( 1==methodid ) {
		ConstructSplitSegmentsRobust();
		TessellateCollidedFaceRobust(mPSI,mCE,0);
		//TessellateCollidedFaceRobust(mPSJ,mCE,1);
	} else if ( 0==methodid ) {
		ConstructSplitSegments();	
		TessellateCollidedFace(mPSI,mCE->mSurfI);
		TessellateCollidedFace(mPSJ,mCE->mSurfJ);
	} else if ( 2==methodid ) {
		ConstructSplitSegmentsWithEndPoint();
		TessellateFaceWithWBO(mPSI, mCE->mSurfI, mCE->mSurfJ);
	};
	
}

void xtSplitBuilder::SplitPnt(xtCollisionEntity *psI, xtCollisionEntity *psJ, xtSFMap &sfmap, xtGeometrySurfaceDataS *surfI, xtGeometrySurfaceDataS *surfJ)
{
	for ( size_t i=0; i<psI->surfslot.size(); ++i ) {
		xtSurfaceSlot *currssI = psI->surfslot[i];
		for ( size_t fidx=0; fidx<currssI->cflist.size(); ++fidx ) {
			const int collidessIdx = currssI->cflist[fidx];
			xtSurfaceSlot *collidess = psJ->GetFaceSlotExit(collidessIdx);
			for ( int eidx=0; eidx<3; ++eidx ) {
				xtSegmentSlot *colledseg = collidess->segs[eidx];
				const int startPntIdx = colledseg->startIdx;
				const int endPntIdx = colledseg->endIdx;
				// check start end collidessIdx as a key;
				xtSegmentFaceK key = { startPntIdx, endPntIdx, currssI->idx };
				xtSegmentFaceK keyinv = { endPntIdx, startPntIdx, currssI->idx };
				xtSFMap::iterator findkey = sfmap.find(key);
				if ( findkey!=sfmap.end() ) {
					xtVector3d *splitPnt = findkey->second;
					currssI->pointsOnSurf.push_back(splitPnt);
					colledseg->pointOnSeg.push_back(splitPnt);
				} else if ( (findkey=sfmap.find(keyinv))!=sfmap.end() ) {
					xtVector3d *splitPnt = findkey->second;
					currssI->pointsOnSurf.push_back(splitPnt);
					colledseg->pointOnSeg.push_back(splitPnt);
				} else { // calculate the new point and insert it to the sfmap
					xtVector3d startPntJ = GetWorldCoordinate(surfJ,startPntIdx);// surfJ->verts[startPntIdx];
					xtVector3d endPntJ   = GetWorldCoordinate(surfJ,endPntIdx); //surfJ->verts[endPntIdx];
					
					xtIndexTria3 &triaI = surfI->indices[currssI->idx];
					xtVector3d pa = GetWorldCoordinate(surfI,triaI.a[0]);//surfI->verts[triaI.a[0]];
					xtVector3d pb = GetWorldCoordinate(surfI,triaI.a[1]);//surfI->verts[triaI.a[1]];
					xtVector3d pc = GetWorldCoordinate(surfI,triaI.a[2]);//surfI->verts[triaI.a[2]];
					
					double t,u,v;
					xtVector3d dir = (endPntJ-startPntJ);
					const double dirlength = dir.norm();
					if ( dirlength < 0.00001 ) {
						printf("May De:%f\n",dirlength);
					}
					dir/=dirlength;
					//dir.normalize();
					//printf("Ray Direction x:%f,\ty:%f\tz:%f\n",dir.x(),dir.y(),dir.z());
					g_log_file << "++"<< eidx << "\t" <<  dir.x() << "\t" <<dir.y() << "\t" << dir.z() << '\n' ; 

					if ( IntersectTriangleTemplate(startPntJ, dir,pa,pb,pc,&t,&u,&v) ) {
						if ( dirlength>=t && t>=0) {
							xtVector3d *newsplitPnt = new xtVector3d(startPntJ+t*dir);
							mSharedSplitPoints.push_back(newsplitPnt);
							currssI->pointsOnSurf.push_back(newsplitPnt);
							colledseg->pointOnSeg.push_back(newsplitPnt);
							sfmap[key]=newsplitPnt;
							//printf("Point On Parameters %f\t%f\t%f\t\n",t,u,v);
							g_log_file << "--"<< eidx << "\t" << t << '\t' << u << '\t' << v << '\n';
#if XT_DEBUG_PERCE_POINT
							xtRaySegment rayseg = {startPntJ,*newsplitPnt,endPntJ};
							mDebugedge.push_back(rayseg);
#endif
						}
					}
				}
			}
		}
	}
}

xtOrderSegSuquence::~xtOrderSegSuquence()
{
	for ( size_t i=0; i<seq.size(); ++i ) {
		delete seq[i];
		seq[i] = NULL;
	} 
}

void xtOrderSegSuquence::OrderSequence(std::vector<xtSegment *> &segs) 
{
	xtSegmentVertexPointerKey *key;
	for ( size_t i=0; i<segs.size(); ++i ) {
		key = new xtSegmentVertexPointerKey;
		key->vert = segs[i]->seg0;
		key->vbseg = segs[i];
		seq.push_back(key);

		key = new xtSegmentVertexPointerKey;
		key->vert = segs[i]->seg1;
		key->vbseg = segs[i];
		seq.push_back(key);
	}
	std::sort(seq.begin(),seq.end(),xtSegmentVertexPointerKeyComp);

	const int seqsize = seq.size();
	std::vector<xtSegmentVertexPointerKey *> startend;
	std::vector<int> startendidx;
	int interseqcount = 0;
	for ( size_t i=0; i<seq.size()-1; ++i ) {
		xtSegmentVertexPointerKey  *right, *mid;
		mid = seq[i];
		right = seq[i+1];

		if ( right==mid ) {
			verts.push_back(mid->vert);
			sequence.push_back(interseqcount++);
			i++;
			continue;
		}

		if ( right!=mid ) {
			startend.push_back(mid);
			startendidx.push_back(i);
			continue;
		}
	}

	// concate the sequence;
	
}


void xtOrderSegSuquence::OrderSequenceSlow( std::vector<xtSegment *> &segs )
{
	std::vector<xtSegmentVertexPointerKey *> seq;
}


void xtSegmentPointer2Index::IndexPointer(std::vector<xtSegment *> &segs,
			std::vector<xtVector3d *> &verts,
		std::vector<std::tuple<int,int>> &segis)
{
	
	xtPntSlotMap vmap;
	int vertcount=0;
	for ( size_t i=0; i<segs.size(); ++i ) {
		std::tuple<int,int> intseg;
		xtSegment *seg = segs[i];
		xtPntSlotMap::iterator fsid = vmap.find( seg->seg0 );
		if ( fsid!=vmap.end() ) {
			std::get<0>(intseg) = fsid->second;
		} else {
			std::get<0>(intseg) = vertcount;
			vmap.insert( std::pair<xtVector3d*,int>(seg->seg0,vertcount++) );
		}
		xtPntSlotMap::iterator feid = vmap.find( seg->seg1 );
		if ( feid!=vmap.end() ) {
			std::get<1>( intseg ) = feid->second;
		} else {
			std::get<1>( intseg ) = vertcount;
			vmap.insert( std::pair<xtVector3d*,int>(seg->seg1,vertcount++) );
		}

		segis.push_back( intseg );
	}
	std::vector<int> idxcache;
	for ( xtPntSlotMap::iterator it=vmap.begin(); it!=vmap.end(); ++it ) {
		idxcache.push_back(it->second);
		verts.push_back(it->first);
	}

	std::sort( idxcache.begin(), idxcache.end() );

	for ( size_t i=0; i<idxcache.size()/2; ++i ) {
		std::swap( verts[i], verts[idxcache[i]] );
	}
}


void xtSegmentRobustPointer2Index::IndexPointer(std::vector<xtSegmentRobust *> &segps, 
	std::vector<xtVertexRobust *> &verts,
	std::vector<std::tuple<int,int>> &segis)
{
	xtPntRobustSlotMap rsmap;
	int vertcount = 0;
	for ( size_t i=0; i<segps.size(); ++i ) {
		std::tuple<int,int> intseg;
		xtSegmentRobust *seg = segps[i];

		xtPntRobustSlotMap::iterator fsit = rsmap.find( &(seg->v[0]) );
		if ( fsit!=rsmap.end() ) {
			std::get<0>( intseg ) = fsit->second;
		} else {
			std::get<0>( intseg ) = vertcount;
			rsmap.insert( std::pair< xtVertexRobust*, int>(&(seg->v[0]), vertcount++) );
		}

		xtPntRobustSlotMap::iterator feit = rsmap.find( &(seg->v[1]) );
		if ( feit!=rsmap.end() ) {
			std::get<1>( intseg ) = feit->second;
		} else {
			std::get<1>( intseg ) = vertcount;
			rsmap.insert( std::pair< xtVertexRobust*, int>( &(seg->v[1]), vertcount++) );
		}

		segis.push_back( intseg );
	}

	std::vector<int> idxcache;
	for ( xtPntRobustSlotMap::iterator it=rsmap.begin(); it!=rsmap.end(); ++it ) {
		idxcache.push_back(it->second);
		verts.push_back(it->first);
	}

	//std::sort( idxcache.begin(), idxcache.end() );

	for ( size_t i=0; i<idxcache.size()/2; ++i ) {
		std::swap( verts[i], verts[idxcache[i]] );
	}
}


void xtSplitBuilder::TessellateCollidedFace(xtCollisionEntity *ps, xtGeometrySurfaceDataS *surf)
{
	for ( size_t ssidx=0; ssidx<ps->surfslot.size(); ++ssidx ) {
		xtSurfaceSlot *ss = ps->surfslot[ssidx];
		
		std::vector<int> vertidxmap;

		xtIndexTria3 &triaI = surf->indices[ss->idx];
		xtVector3d pa = GetWorldCoordinate(surf,triaI.a[0]);//surfI->verts[triaI.a[0]];
		xtVector3d pb = GetWorldCoordinate(surf,triaI.a[1]);//surfI->verts[triaI.a[1]];
		xtVector3d pc = GetWorldCoordinate(surf,triaI.a[2]);//surfI->verts[triaI.a[2]];
		vertidxmap.push_back(triaI.a[0]);
		vertidxmap.push_back(triaI.a[1]);
		vertidxmap.push_back(triaI.a[2]);

		// construct local coordinate
		xtVector3d pba = pb - pa;
		xtVector3d pca = pc - pa;
		xtVector3d norm = pca.cross(pba);
		norm.normalize();
		xtVector3d xcoord = pca.cross(norm);
		xcoord.normalize();
		xtVector3d ycoord = norm.cross(xcoord);

		//xtMatrix3d rotm(
		//	xcoord.x(),ycoord.x(),norm.x(),
		//	xcoord.y(),ycoord.y(),norm.y(),
		//	xcoord.z(),ycoord.z(),norm.z());
		xtMatrix3d rotm;
		//rotm << xcoord.x(),ycoord.x(),norm.x(),
		//	xcoord.y(),ycoord.y(),norm.y(),
		//	xcoord.z(),ycoord.z(),norm.z() ;
		rotm << 
			xcoord.x(), xcoord.y(), xcoord.z(),
			ycoord.x(), ycoord.y(), ycoord.z(),
			norm.x(), norm.y(), norm.z();


		std::vector<xtVector3d *> &segonsurfverts = ss->pointsOnSurfVerbos;
		std::vector<std::tuple<int,int>> segonsurfindices;
		xtSegmentPointer2Index indexsegonsurf;
		indexsegonsurf.IndexPointer(ss->segsonsurf,segonsurfverts,segonsurfindices);
		
		std::vector<xtTriPnt2> verts2d;
		std::vector<xtSeg2WithMarker> segmarkerlist;
		std::vector<xtTriIndexO> outtris;

		// pack triangle boundary
		xtVector3d pa2d = rotm*pa;
		xtVector3d pb2d = rotm*pb;
		xtVector3d pc2d = rotm*pc;
		xtTriPnt2 a2d = { pa2d.x(), pa2d.y() };
		xtTriPnt2 b2d = { pb2d.x(), pb2d.y() };
		xtTriPnt2 c2d = { pc2d.x(), pc2d.y() };
		verts2d.push_back(a2d);
		verts2d.push_back(b2d);
		verts2d.push_back(c2d);
		xtSeg2WithMarker segmarker;
		segmarker.seg[0] = 0;
		segmarker.seg[1] = 1;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);
		segmarker.seg[0] = 1;
		segmarker.seg[1] = 2;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);
		segmarker.seg[0] = 2;
		segmarker.seg[1] = 0;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);

		// pack seg on triangle surf
		for ( size_t i=0; i<segonsurfverts.size(); ++i ) {
			xtVector3d p2d = rotm*(*(segonsurfverts[i]));
			xtTriPnt2 d2 = { p2d.x(), p2d.y() };
			verts2d.push_back(d2);
			vertidxmap.push_back(i);
		}
		xtSeg2WithMarker ontrisegmarker;
		ontrisegmarker.marker = 1;// one means seg on triangle surface
		for ( size_t i=0; i<segonsurfindices.size(); ++i ) {
			std::tuple<int,int> &segonsurf = segonsurfindices[i];
			ontrisegmarker.seg[0] = std::get<0>(segonsurf)+3;
			ontrisegmarker.seg[1] = std::get<1>(segonsurf)+3;
			segmarkerlist.push_back(ontrisegmarker);
		}



		xtTrianglePLSG splittriutil(verts2d, segmarkerlist, outtris);

		for ( size_t i=0; i<outtris.size(); ++i ) {
			xtIndexTria3 tria;
			for ( int fidx=0; fidx<3; ++fidx ) {
				tria.a[fidx] = outtris[i].idx[fidx];
			}
			ss->tris.push_back(tria);
		}
	}
}

void xtSplitBuilder::TessellateCollidedFaceRobust( xtCollisionEntity *ps, xtCollisionEngine *ce, const int type )
{
	xtGeometrySurfaceDataS *surf;
	if ( type==0 ) {
		surf = ce->mSurfI;
	} else if ( type==1 ) {
		surf = ce->mSurfJ;
	}

	for ( size_t ssidx=0; ssidx<ps->surfslot.size(); ++ssidx ) {
		xtSurfaceSlot *ss = ps->surfslot[ssidx];
		
		std::vector<int> vertidxmap;

		xtIndexTria3 &triaI = surf->indices[ss->idx];
		xtVector3d pa = GetWorldCoordinate(surf,triaI.a[0]);//surfI->verts[triaI.a[0]];
		xtVector3d pb = GetWorldCoordinate(surf,triaI.a[1]);//surfI->verts[triaI.a[1]];
		xtVector3d pc = GetWorldCoordinate(surf,triaI.a[2]);//surfI->verts[triaI.a[2]];
		vertidxmap.push_back(triaI.a[0]);
		vertidxmap.push_back(triaI.a[1]);
		vertidxmap.push_back(triaI.a[2]);

		// construct local coordinate
		xtVector3d pba = pb - pa;
		xtVector3d pca = pc - pa;
		xtVector3d norm = pca.cross(pba);
		norm.normalize();
		xtVector3d xcoord = pca.cross(norm);
		xcoord.normalize();
		xtVector3d ycoord = norm.cross(xcoord);

		//xtMatrix3d rotm(
		//	xcoord.x(),ycoord.x(),norm.x(),
		//	xcoord.y(),ycoord.y(),norm.y(),
		//	xcoord.z(),ycoord.z(),norm.z());
		xtMatrix3d rotminv;
		rotminv << xcoord.x(),ycoord.x(),norm.x(),
			xcoord.y(),ycoord.y(),norm.y(),
			xcoord.z(),ycoord.z(),norm.z() ;
		xtMatrix3d rotm;
		rotm << 
			xcoord.x(), xcoord.y(), xcoord.z(),
			ycoord.x(), ycoord.y(), ycoord.z(),
			norm.x(), norm.y(), norm.z();


		std::vector<xtVertexRobust *> segonsurfverts;
		std::vector<std::tuple<int,int>> segonsurfindices;
		xtSegmentRobustPointer2Index indexsegonsurf;
	 	indexsegonsurf.IndexPointer(ss->segrobust,segonsurfverts,segonsurfindices);
		
		std::vector<xtTriPnt2> verts2d;
		std::vector<xtSeg2WithMarker> segmarkerlist;
		std::vector<xtTriIndexO> outtris;

		// pack triangle boundary
		xtVector3d pa2d = rotm*pa;
		xtVector3d pb2d = rotm*pb;
		xtVector3d pc2d = rotm*pc;
		xtTriPnt2 a2d = { pa2d.x(), pa2d.y() };
		xtTriPnt2 b2d = { pb2d.x(), pb2d.y() };
		xtTriPnt2 c2d = { pc2d.x(), pc2d.y() };
		const double zheight = (pa2d.z()+pb2d.z()+pc2d.z())/3.;
		verts2d.push_back(a2d);
		verts2d.push_back(b2d);
		verts2d.push_back(c2d);
		xtSeg2WithMarker segmarker;
		segmarker.seg[0] = 0;
		segmarker.seg[1] = 1;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);
		segmarker.seg[0] = 1;
		segmarker.seg[1] = 2;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);
		segmarker.seg[0] = 2;
		segmarker.seg[1] = 0;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);

		// pack seg on triangle surf
		std::vector<int> newsplitidx;
		std::vector<int> idxlist;
		for ( size_t i=0; i<segonsurfverts.size(); ++i ) {
			xtVertexRobust *vert = segonsurfverts[i];
			if ( vert->vtype == NEW_SPLIT ) {
				newsplitidx.push_back(i);
				xtVector3d p2d = rotm*(*(vert->v));
				xtTriPnt2 d2 = {p2d.x(),p2d.y()};
				verts2d.push_back(d2);
			} else if ( type==0 ) {
				if ( vert->vtype == ORI_VERTEXI ) {
					if ( vert->idx==triaI.a[0] ) {
						idxlist.push_back(0);
					} else if ( vert->idx==triaI.a[1] ) {
						idxlist.push_back(1);
					} else if ( vert->idx==triaI.a[2] ) {
						idxlist.push_back(2);
					}
				} else if ( vert->vtype == ORI_VERTEXJ ) {
					xtVector3d p2d = rotm*GetWorldCoordinate(ce->mSurfJ, vert->idx);
					xtTriPnt2 d2 = {p2d.x(),p2d.y()};
					verts2d.push_back(d2);
					newsplitidx.push_back(i);
				}
			} else if ( type == 1 ) {
				if ( vert->vtype == ORI_VERTEXI ) {
					xtVector3d p2d = rotm*GetWorldCoordinate(ce->mSurfI, vert->idx);
					xtTriPnt2 d2 = {p2d.x(),p2d.y()};
					verts2d.push_back(d2);
					newsplitidx.push_back(i);
				} else if ( vert->vtype == ORI_VERTEXJ ) {
					if ( vert->idx==triaI.a[0] ) {
						idxlist.push_back(0);
					} else if ( vert->idx==triaI.a[1] ) {
						idxlist.push_back(1);
					} else if ( vert->idx==triaI.a[2] ) {
						idxlist.push_back(2);
					}
				}
			} 
		}


		ConcatenationPolyLine(segonsurfindices,segonsurfverts.size());

		xtSeg2WithMarker ontrisegmarker;
		ontrisegmarker.marker = 1;// one means seg on triangle surface
		int indxcounter=0;
		for ( size_t i=0; i<segonsurfindices.size(); ++i ) {
			std::tuple<int,int> &segonsurf = segonsurfindices[i];
			xtVertexRobust *vert0 = segonsurfverts[std::get<0>(segonsurf)];
			xtVertexRobust *vert1 = segonsurfverts[std::get<1>(segonsurf)];
			if ( type==0 ) {
				if ( vert0->vtype==NEW_SPLIT ) {
					std::vector<int>::iterator fit = std::find(newsplitidx.begin(),newsplitidx.end(),std::get<0>(segonsurf));
					const int newidx = std::distance(newsplitidx.begin(),fit);
					std::get<0>(segonsurf) = newidx;
					std::get<0>( segonsurf ) = newidx + 3;
				}
				if ( vert1->vtype==NEW_SPLIT ) {
					std::vector<int>::iterator fit = std::find( newsplitidx.begin(), newsplitidx.end(), std::get<1>(segonsurf) );
					const int newidx = std::distance( newsplitidx.begin(),fit );
					std::get<1>( segonsurf ) = newidx;
					std::get<1>( segonsurf ) = newidx + 3;
				}
				if ( vert0->vtype==ORI_VERTEXI ) {
					std::get<0>( segonsurf ) = idxlist[indxcounter++];
				}
				if ( vert1->vtype==ORI_VERTEXI ) {
					std::get<1>( segonsurf ) = idxlist[indxcounter++];
				}
				if ( vert0->vtype==ORI_VERTEXJ ) {
					std::vector<int>::iterator fit = std::find( newsplitidx.begin(), newsplitidx.end(), std::get<0>(segonsurf) );
					const int newidx = std::distance( newsplitidx.begin(),fit );
					std::get<0>( segonsurf )  = newidx;
					std::get<0>( segonsurf )  = newidx + 3 ;
				}
				if ( vert1->vtype==ORI_VERTEXJ ) {
					std::vector<int>::iterator fit = std::find( newsplitidx.begin(), newsplitidx.end(), std::get<1>(segonsurf) );
					const int newidx = std::distance( newsplitidx.begin(),fit );
					std::get<1>( segonsurf ) = newidx;
					std::get<1>( segonsurf ) = newidx + 3;
				}
			} else if ( type==1 ) {
				if ( vert0->vtype==NEW_SPLIT ) {
					std::vector<int>::iterator fit = std::find(newsplitidx.begin(),newsplitidx.end(),std::get<0>(segonsurf));
					const int newidx = std::distance( newsplitidx.begin(),fit );
					std::get<0>(segonsurf) = newidx;
					std::get<0>(segonsurf) = newidx + 3;
				}
				if ( vert1->vtype==NEW_SPLIT ) {
					std::vector<int>::iterator fit = std::find( newsplitidx.begin(), newsplitidx.end(), std::get<1>(segonsurf) );
					const int newidx = std::distance( newsplitidx.begin(), fit );
					std::get<1>( segonsurf ) = newidx;
					std::get<1>( segonsurf ) = newidx + 3;
				}
				if ( vert0->vtype==ORI_VERTEXI ) {
					std::vector<int>::iterator fit = std::find( newsplitidx.begin(), newsplitidx.end(), std::get<0>(segonsurf) );
					const int newidx = std::distance( newsplitidx.begin(),fit );
					std::get<0>( segonsurf )  = newidx;
					std::get<0>( segonsurf )  = newidx + 3;
				}
				if ( vert1->vtype==ORI_VERTEXI ) {
					std::vector<int>::iterator fit = std::find( newsplitidx.begin(), newsplitidx.end(), std::get<1>(segonsurf) );
					const int newidx = std::distance( newsplitidx.begin(),fit );
					std::get<1>( segonsurf ) = newidx;
					std::get<1>( segonsurf ) = newidx + 3;
				}
				if ( vert0->vtype==ORI_VERTEXJ ) {
					std::get<0>( segonsurf ) = idxlist[indxcounter++];
				}
				if ( vert1->vtype==ORI_VERTEXJ ) {
					std::get<1>( segonsurf ) = idxlist[indxcounter++];
				}
			}
			ontrisegmarker.marker = 1;
			ontrisegmarker.seg[0] = std::get<0>( segonsurf );
			ontrisegmarker.seg[1] = std::get<1>( segonsurf );
			segmarkerlist.push_back(ontrisegmarker);
		}



#if XT_DEBUG_PLANAR_TRI_SET
		xtPlanarTriSegData ptriseg;
		ptriseg.verts2d = verts2d;
		ptriseg.segmarkerlist = segmarkerlist;
		mDebugPlannarTriSeg.push_back(ptriseg);

		xtPlanarTriSegData3d ptriseg3d;
		for ( size_t i=0; i<verts2d.size(); ++i ) {
			xtVector3d v;
			v << verts2d[i].p[0] , verts2d[i].p[1], zheight;
			ptriseg3d.verts.push_back( rotminv*v );
		}
		ptriseg3d.segmarkerlist = segmarkerlist;
		mDebugPlanarTriSeg3d.push_back(ptriseg3d);
		
		if ( ssidx==1 ) {
			return;
		}
#endif
		//FilterAdjacentVerts2d(verts2d, segmarkerlist);

		xtTrianglePLSG splittriutil(verts2d, segmarkerlist, outtris);

		for ( size_t i=0; i<outtris.size(); ++i ) {
			xtIndexTria3 tria;
			for ( int fidx=0; fidx<3; ++fidx ) {
				tria.a[fidx] = outtris[i].idx[fidx];
			}
			ss->tris.push_back(tria);
		}
		xtPlanarTri planartris;
		planartris.tris = ss->tris;
		planartris.verts = ptriseg3d.verts;
		mDebugPlanarTris.push_back(planartris);
	}
	
	

}

struct xtState2
{
	int s[2];
};

void OrderxtSegmentWBOList( std::vector<xtSegmentWBO *> &segwbo )
{

	std::vector<xtState2> segstatelist;
	segstatelist.reserve(segwbo.size());
	for ( size_t i=0; i<segwbo.size(); ++i ) {
		xtState2 state = {-2,-2};
		segstatelist.push_back(state);
	}

	int vertexcount=0;
	int singlecount=0;
	int singlep[2];
	int signlepatseg[2];
	for ( int segi=0; segi<segwbo.size(); ++segi ) {
		xtSegmentWBO *seg = segwbo[segi];

		xtState2 &state = segstatelist[segi];
		int marker[2] = {-1,-1};
		int atidx[2] = {-1,-1};
		
		// if state has been checked bypass;
		//if ( state.s[0]>-2 ) continue;
		//if ( state.s[1]>-2 ) continue;

		for ( int sci=segi+1; sci<segwbo.size(); ++sci ) {
			for ( int endi=0; endi<2; ++endi ) {
				for ( int scii=0; scii<2; ++scii ) {
					if ( seg->v[endi]==segwbo[sci]->v[scii] ) {
						marker[endi]=scii;
						atidx[endi]=sci;
					}
				}
			}
		}
		for ( int endi=0; endi<2; ++endi ) {
			if ( marker[endi]>-1 && segstatelist[segi].s[endi]==-2 ) {
				segstatelist[segi].s[endi]=vertexcount;
				segstatelist[atidx[endi]].s[marker[endi]]=vertexcount;
				vertexcount++;
			} else if ( marker[endi]==-1 && segstatelist[segi].s[endi]==-2 ) {
				segstatelist[segi].s[endi]=vertexcount;
				vertexcount++;
				singlep[singlecount]      = endi;
				signlepatseg[singlecount] = segi;
				singlecount++;
			}
		}
	}
	
	assert(singlecount==2);
	int startsegidx = signlepatseg[0];
	bool isStart = singlep[0]==0;
	for ( size_t i=0; i<segwbo.size(); ++i ) {
		if ( !isStart ) {
			std::swap( segwbo[startsegidx]->v[0], segwbo[startsegidx]->v[1] );
			std::swap( segstatelist[startsegidx].s[0], segstatelist[startsegidx].s[1] );
		}
		std::swap( segwbo[i], segwbo[startsegidx] );
		std::swap( segstatelist[i], segstatelist[startsegidx] );

		int endpidx = segstatelist[i].s[1];

		if ( segwbo.size()-1==i ) break;

		for ( size_t si=i+1; si<segstatelist.size(); ++si ) {
			if ( segstatelist[si].s[0]==endpidx ) {
				startsegidx = si;
				isStart=true;
			} else if ( segstatelist[si].s[1]==endpidx ) {
				startsegidx = si;
				isStart=false;
			}
		}

	}

}

xtTriPnt2 GetPlanarPoint()
{
	xtTriPnt2 t;
	return t;
}

void xtSplitBuilder::TessellateFaceWithWBO( xtCollisionEntity *ps, xtGeometrySurfaceDataS *surf0, xtGeometrySurfaceDataS *surf1 )
{
	for ( size_t ssidx=0; ssidx<ps->surfslot.size(); ++ssidx ) {
		xtSurfaceSlot *ss = ps->surfslot[ssidx];

		std::vector<int> vertidxmap;

		xtIndexTria3 &triaI = surf0->indices[ss->idx];
		xtVector3d pa = GetWorldCoordinate(surf0,triaI.a[0]);//surfI->verts[triaI.a[0]];
		xtVector3d pb = GetWorldCoordinate(surf0,triaI.a[1]);//surfI->verts[triaI.a[1]];
		xtVector3d pc = GetWorldCoordinate(surf0,triaI.a[2]);//surfI->verts[triaI.a[2]];
		vertidxmap.push_back(triaI.a[0]);
		vertidxmap.push_back(triaI.a[1]);
		vertidxmap.push_back(triaI.a[2]);

		// construct local coordinate
		xtVector3d pba = pb - pa;
		xtVector3d pca = pc - pa;
		xtVector3d norm = pca.cross(pba);
		norm.normalize();
		xtVector3d xcoord = pca.cross(norm);
		xcoord.normalize();
		xtVector3d ycoord = norm.cross(xcoord);


		xtMatrix3d rotminv;
		rotminv << xcoord.x(),ycoord.x(),norm.x(),
			xcoord.y(),ycoord.y(),norm.y(),
			xcoord.z(),ycoord.z(),norm.z() ;
		xtMatrix3d rotm;
		rotm << 
			xcoord.x(), xcoord.y(), xcoord.z(),
			ycoord.x(), ycoord.y(), ycoord.z(),
			norm.x(), norm.y(), norm.z();


		std::vector<xtVertexRobust *> segonsurfverts;
		std::vector<std::tuple<int,int>> segonsurfindices;
		xtSegmentRobustPointer2Index indexsegonsurf;
	 	indexsegonsurf.IndexPointer(ss->segrobust,segonsurfverts,segonsurfindices);
		
		std::vector<xtTriPnt2> verts2d;
		std::vector<xtSeg2WithMarker> segmarkerlist;
		std::vector<xtTriIndexO> outtris;

		// 1) pack the geometry vertex
		// pack triangle boundary vertex
		xtVector3d pa2d = rotm*pa;
		xtVector3d pb2d = rotm*pb;
		xtVector3d pc2d = rotm*pc;
		xtTriPnt2 a2d = { pa2d.x(), pa2d.y() };
		xtTriPnt2 b2d = { pb2d.x(), pb2d.y() };
		xtTriPnt2 c2d = { pc2d.x(), pc2d.y() };
		const double zheight = (pa2d.z()+pb2d.z()+pc2d.z())/3.;
		verts2d.push_back(a2d);
		verts2d.push_back(b2d);
		verts2d.push_back(c2d);

		// pack split segment 
		OrderxtSegmentWBOList( ss->segwbo );

		// push back first boundary vertex
		xtVector3d boundaryPntStart = *(ss->segwbo[0]->v[0].v);
		boundaryPntStart=rotm*boundaryPntStart;
		xtTriPnt2 bPntStart = {boundaryPntStart.x(),boundaryPntStart.y()};
		verts2d.push_back(bPntStart);
		// treat the start and end as a special case
		for ( size_t i=1; i<ss->segwbo.size(); ++i ) {
			xtVector3d pnt;// = *(ss->segwbo[i]->v[0].v);
			if ( ss->segwbo[i]->v[0].type==ON_SURFACE ) {
				pnt = *(ss->segwbo[i]->v[0].v);
			} else if ( ss->segwbo[i]->v[0].type==ON_SURFACE_V ) {
				pnt = GetWorldCoordinate( surf1, ss->segwbo[i]->v[0].idx );
			} else if ( ss->segwbo[i]->v[0].type==ON_V ) {
				pnt = GetWorldCoordinate( surf0, ss->segwbo[i]->v[0].idx );
			} else if ( ss->segwbo[i]->v[0].type==ON_BOUNDARY ) {
				assert(false);
				// not handled in case there more complex configuration
			}
			pnt = rotm*pnt;
			xtTriPnt2 pnt2d = {pnt.x(),pnt.y()};
			verts2d.push_back(pnt2d);
		}
		xtVector3d boundaryPntEnd   = *(ss->segwbo[ss->segwbo.size()-1]->v[1].v);
		boundaryPntEnd = rotm*boundaryPntEnd;
		xtTriPnt2 bPntEnd = {boundaryPntEnd.x(),boundaryPntEnd.y()};
		verts2d.push_back(bPntEnd);

		// 2) pack the topology segment connectivity
		xtSeg2WithMarker segmarker;
		const int batedge0 = ss->segwbo[0]->v[0].idx;
		segmarker.seg[0] = batedge0;
		segmarker.seg[1] = 0+3;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);
		segmarker.seg[0] = 0+3;
		segmarker.seg[1] = batedge0+1;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);
		const int batedge1idx = ss->segwbo.size()-1;
		const int batedge1 = ss->segwbo[batedge1idx]->v[1].idx;
		segmarker.seg[0] = batedge1;
		segmarker.seg[1] = batedge1idx+3+1;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);
		segmarker.seg[0] = batedge1idx+3+1;
		segmarker.seg[1] = batedge1+1;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);
		const int wedgeidx = (0+1+2)-(batedge0+batedge1);
		segmarker.seg[0] = wedgeidx;
		segmarker.seg[1] = (wedgeidx+1)%3;
		segmarker.marker = 0;
		segmarkerlist.push_back(segmarker);

		for ( size_t i=0; i<ss->segwbo.size(); ++i ) {
			segmarker.seg[0] = 3+i;
			segmarker.seg[1] = 3+i+1;
			segmarker.marker = 1;
			segmarkerlist.push_back(segmarker);
		}

#if XT_DEBUG_PLANAR_TRI_SET
		xtPlanarTriSegData ptriseg;
		ptriseg.verts2d = verts2d;
		ptriseg.segmarkerlist = segmarkerlist;
		mDebugPlannarTriSeg.push_back(ptriseg);

		xtPlanarTriSegData3d ptriseg3d;
		for ( size_t i=0; i<verts2d.size(); ++i ) {
			xtVector3d v;
			v << verts2d[i].p[0] , verts2d[i].p[1], zheight;
			ptriseg3d.verts.push_back( rotminv*v );
		}
		ptriseg3d.segmarkerlist = segmarkerlist;
		mDebugPlanarTriSeg3d.push_back(ptriseg3d);
		
		if ( ssidx==2 ) {
			return;
		}
#endif
		//FilterAdjacentVerts2d(verts2d, segmarkerlist);
		printf("++Begin Tesselate Triangle %d:\n",ssidx);
		xtTrianglePLSG splittriutil(verts2d, segmarkerlist, outtris);
		printf("--Finis Tesselate Triangle %d:\n",ssidx);
		for ( size_t i=0; i<outtris.size(); ++i ) {
			xtIndexTria3 tria;
			for ( int fidx=0; fidx<3; ++fidx ) {
				tria.a[fidx] = outtris[i].idx[fidx];
			}
			ss->tris.push_back(tria);
		}
		xtPlanarTri planartris;
		planartris.tris = ss->tris;
		planartris.verts = ptriseg3d.verts;
		mDebugPlanarTris.push_back(planartris);


	}
}

bool counteriftrue( bool & istrue )
{
	return istrue;
}

void xtSplitBuilder::ConstructSplitSegmentsRobust()
{
	for ( size_t ki=0; ki<mCE->mCollide.size(); ++ki ) {
		const int fIidx = mCE->mCollide[ki].i;
		const int fJidx = mCE->mCollide[ki].j;
		xtFaceFaceKey ffkey = {fIidx,fJidx};
		xtFaceFaceKey ffkeyinv = {fJidx, fIidx};
		xtFFMap::iterator findkey = mFFM.find(ffkey);
		// bug
		//xtFFMap::iterator findkeyinv = mFFM.find(ffkeyinv);
		if ( findkey!=mFFM.end() /*|| findkeyinv!=mFFM.end() */) continue;
		
		// let J's tri 3 edge test the I's face
		// 0) may degenerate
		// 1) one case in J 
		// 2) generate the common split segment
		std::vector<bool> edgestateI; edgestateI.reserve(3);
		std::vector<xtVector3d *> spIlist; spIlist.reserve(3);
		std::vector<bool> edgestateJ; edgestateJ.reserve(3);
		std::vector<xtVector3d *> spJlist; spJlist.reserve(3);

		xtIndexTria3 triaI = mCE->mSurfI->indices[fIidx];
		xtIndexTria3 triaJ = mCE->mSurfJ->indices[fJidx];

		// check start end collidessIdx as a key;
		for ( int i=0; i<3; ++i ) {
			xtSegmentFaceK key = { triaJ.a[i], triaJ.a[(i+1)%3], fIidx };
			xtSegmentFaceK keyinv = { triaJ.a[(i+1)%3], triaJ.a[i], fIidx };
			xtSFMap::iterator findit;
			if ( (findit=mSFMI.find(key))!=mSFMI.end() ) {
				edgestateI.push_back(true);
				spIlist.push_back(findit->second);
			} else if ( (findit=mSFMI.find(keyinv))!=mSFMI.end() ) {
				edgestateI.push_back(true);
				spIlist.push_back(findit->second);
			} else {
				edgestateI.push_back(false);
				spIlist.push_back(NULL);
			}
		}

		for ( int i=0; i<3; ++i ) {
			xtSegmentFaceK key = { triaI.a[i], triaI.a[(i+1)%3], fJidx };
			xtSegmentFaceK keyinv = { triaI.a[(i+1)%3], triaI.a[i], fJidx };
			xtSFMap::iterator findit;
			if ( (findit=mSFMJ.find(key))!=mSFMJ.end() ) {
				edgestateJ.push_back(true);
				spJlist.push_back(findit->second);
			} else if ( (findit=mSFMJ.find(keyinv))!=mSFMJ.end() ) {
				edgestateJ.push_back(true);
				spJlist.push_back(findit->second);
			} else {
				edgestateJ.push_back(false);
				spJlist.push_back(NULL);
			}
		}

		//int numIntersectWI, numIntersectWJ;
		const int numIntersectWI = std::count(edgestateI.begin(),edgestateI.end(),true);
		const int numIntersectWJ = std::count(edgestateJ.begin(),edgestateJ.end(),true);
		if ( 0==numIntersectWI && 2==numIntersectWJ ) {
			//std::vector<bool>::iterator findfalse = std::find(edgestateI.begin(),edgestateI.end(),false);
			int falseIdxJ;
			for ( size_t i=0; i<edgestateJ.size(); ++i ) {
				if ( !edgestateJ[i] ) {
					falseIdxJ = i;
				}
			}
			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spJlist[(falseIdxJ+1)%3];
			newseg->seg1 = spJlist[(falseIdxJ+2)%3];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

			//======================================================
			xtSegmentRobust *newsegrb = new xtSegmentRobust;
			newsegrb->v[0].vtype = NEW_SPLIT;
			newsegrb->v[0].v = spJlist[(falseIdxJ+1)%3];
			newsegrb->v[1].vtype = NEW_SPLIT;
			newsegrb->v[1].v = spJlist[(falseIdxJ+2)%3];

			mPSI->AddSplitSegmentRBToFace(fIidx,newsegrb);
			mPSJ->AddSplitSegmentRBToFace(fJidx,newsegrb);

		} else if ( 1==numIntersectWI&&1==numIntersectWJ ) {
			std::vector<bool>::iterator iiiter = std::find(edgestateI.begin(), edgestateI.end(), true);
			const size_t iiidx = std::distance(edgestateI.begin(),iiiter);

			std::vector<bool>::iterator ijiter = std::find(edgestateJ.begin(), edgestateJ.end(), true);
			const size_t ijidx = std::distance(edgestateJ.begin(),ijiter);

			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[iiidx];
			newseg->seg1 = spJlist[ijidx];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

			//======================================================
			xtSegmentRobust *newsegrb = new xtSegmentRobust;
			newsegrb->v[0].vtype = NEW_SPLIT;
			newsegrb->v[0].v = spIlist[iiidx];
			newsegrb->v[1].vtype = NEW_SPLIT;
			newsegrb->v[1].v = spJlist[ijidx];

			mPSI->AddSplitSegmentRBToFace(fIidx,newsegrb);
			mPSJ->AddSplitSegmentRBToFace(fJidx,newsegrb);
			
		} else if ( 2==numIntersectWI && 0==numIntersectWJ ) {
			int falseIdx;
			for ( size_t i=0; i<edgestateI.size(); ++i ) {
				if ( !edgestateI[i] ) {
					falseIdx = i;
				}
			}
			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[(falseIdx+1)%3];
			newseg->seg1 = spIlist[(falseIdx+2)%3];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

			//======================================================
			xtSegmentRobust *newsegrb = new xtSegmentRobust;
			newsegrb->v[0].vtype = NEW_SPLIT;
			newsegrb->v[0].v = spIlist[(falseIdx+1)%3];
			newsegrb->v[1].vtype = NEW_SPLIT;
			newsegrb->v[1].v = spIlist[(falseIdx+2)%3];

			mPSI->AddSplitSegmentRBToFace(fIidx,newsegrb);
			mPSJ->AddSplitSegmentRBToFace(fJidx,newsegrb);
			
		} else if ( 3==numIntersectWI&&0==numIntersectWJ) {
			printf( "Co point I\n" );

		} else if ( 0==numIntersectWI&&3==numIntersectWJ) {
			printf( "Co point J\n" );

		} else if ( 2==numIntersectWI&&1==numIntersectWJ) {
			//printf( "I touch on\n" ); 
			// one point of I on triangle J
			// There at least two edge intersect with J degenerate case
			std::vector<bool>::iterator findIit = std::find(edgestateI.begin(),edgestateI.end(),false);
			const size_t falseIidx = std::distance(edgestateI.begin(),findIit);
			
			std::vector<bool>::iterator findJit = std::find(edgestateJ.begin(),edgestateJ.end(),true);
			const size_t trueJidx = std::distance(edgestateJ.begin(),findJit);

			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[(falseIidx+1)%3];
			newseg->seg1 = spJlist[trueJidx];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

			//======================================================
			xtSegmentRobust *newsegrb = new xtSegmentRobust;
			newsegrb->v[0].vtype = ORI_VERTEXI;
			newsegrb->v[0].idx = triaI.a[(falseIidx+2)%3];
			newsegrb->v[1].vtype = NEW_SPLIT;
			newsegrb->v[1].v = spJlist[trueJidx];

			mPSI->AddSplitSegmentRBToFace(fIidx,newsegrb);
			mPSJ->AddSplitSegmentRBToFace(fJidx,newsegrb);

		} else if ( 1==numIntersectWI&&2==numIntersectWJ) {
			//printf( "J touch on\n" );
			// one point of J on triangle area I
			// There at lease two edge intersect by this degenerate case
			// In this branch just one edge collision
			std::vector<bool>::iterator findIit = std::find(edgestateI.begin(),edgestateI.end(),true);
			const size_t trueIidx = std::distance(edgestateI.begin(),findIit);
			
			std::vector<bool>::iterator findJit = std::find(edgestateJ.begin(),edgestateJ.end(),false);
			const size_t falseJidx = std::distance(edgestateJ.begin(),findJit);

			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[trueIidx];
			newseg->seg1 = spJlist[(falseJidx+1)%3];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

			//======================================================
			xtSegmentRobust *newsegrb = new xtSegmentRobust;
			newsegrb->v[0].vtype = NEW_SPLIT;
			newsegrb->v[0].v = spIlist[trueIidx];
			newsegrb->v[1].vtype = ORI_VERTEXJ;
			newsegrb->v[1].idx = triaJ.a[(falseJidx+2)%3];

			mPSI->AddSplitSegmentRBToFace(fIidx,newsegrb);
			mPSJ->AddSplitSegmentRBToFace(fJidx,newsegrb);

		} else if ( 3==numIntersectWI ) {
			// impossible if they are not lay int the same plane
			assert(false);
		}  else {
			// unknow situation
			printf( "I\t%d, J\t%d\n",numIntersectWI,numIntersectWJ);
			assert(false);
		}


	}
}

void xtSplitBuilder::ConstructSplitSegmentsWithEndPoint()
{
	for ( size_t ki=0; ki<mCE->mCollide.size(); ++ki ) {
		const int fIidx = mCE->mCollide[ki].i;
		const int fJidx = mCE->mCollide[ki].j;
		xtFaceFaceKey ffkey = {fIidx,fJidx};
		xtFaceFaceKey ffkeyinv = {fJidx, fIidx};
		xtFFMap::iterator findkey = mFFM.find(ffkey);
		// bug
		//xtFFMap::iterator findkeyinv = mFFM.find(ffkeyinv);
		if ( findkey!=mFFM.end() /*|| findkeyinv!=mFFM.end() */) continue;
		
		// let J's tri 3 edge test the I's face
		// 0) may degenerate
		// 1) one case in J 
		// 2) generate the common split segment
		std::vector<bool> edgestateI; edgestateI.reserve(3);
		std::vector<xtVector3d *> spIlist; spIlist.reserve(3);
		std::vector<bool> edgestateJ; edgestateJ.reserve(3);
		std::vector<xtVector3d *> spJlist; spJlist.reserve(3);

		xtIndexTria3 triaI = mCE->mSurfI->indices[fIidx];
		xtIndexTria3 triaJ = mCE->mSurfJ->indices[fJidx];

		// check start end collidessIdx as a key;
		for ( int i=0; i<3; ++i ) {
			xtSegmentFaceK key = { triaJ.a[i], triaJ.a[(i+1)%3], fIidx };
			xtSegmentFaceK keyinv = { triaJ.a[(i+1)%3], triaJ.a[i], fIidx };
			xtSFMap::iterator findit;
			if ( (findit=mSFMI.find(key))!=mSFMI.end() ) {
				edgestateI.push_back(true);
				spIlist.push_back(findit->second);
			} else if ( (findit=mSFMI.find(keyinv))!=mSFMI.end() ) {
				edgestateI.push_back(true);
				spIlist.push_back(findit->second);
			} else {
				edgestateI.push_back(false);
				spIlist.push_back(NULL);
			}
		}

		for ( int i=0; i<3; ++i ) {
			xtSegmentFaceK key = { triaI.a[i], triaI.a[(i+1)%3], fJidx };
			xtSegmentFaceK keyinv = { triaI.a[(i+1)%3], triaI.a[i], fJidx };
			xtSFMap::iterator findit;
			if ( (findit=mSFMJ.find(key))!=mSFMJ.end() ) {
				edgestateJ.push_back(true);
				spJlist.push_back(findit->second);
			} else if ( (findit=mSFMJ.find(keyinv))!=mSFMJ.end() ) {
				edgestateJ.push_back(true);
				spJlist.push_back(findit->second);
			} else {
				edgestateJ.push_back(false);
				spJlist.push_back(NULL);
			}
		}

		//int numIntersectWI, numIntersectWJ;
		const int numIntersectWI = std::count(edgestateI.begin(),edgestateI.end(),true);
		const int numIntersectWJ = std::count(edgestateJ.begin(),edgestateJ.end(),true);
		if ( 0==numIntersectWI && 2==numIntersectWJ ) {
			//std::vector<bool>::iterator findfalse = std::find(edgestateI.begin(),edgestateI.end(),false);
			int falseIdxJ;
			for ( size_t i=0; i<edgestateJ.size(); ++i ) {
				if ( !edgestateJ[i] ) {
					falseIdxJ = i;
				}
			}
			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spJlist[(falseIdxJ+1)%3];
			newseg->seg1 = spJlist[(falseIdxJ+2)%3];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;


			//======================================================
			xtSegmentWBO *nsegwboi = new xtSegmentWBO;
			nsegwboi->v[0].type = ON_SURFACE;
			nsegwboi->v[0].v = spJlist[(falseIdxJ+1)%3];
			nsegwboi->v[0].idx = -1;
			nsegwboi->v[1].type = ON_SURFACE;
			nsegwboi->v[1].v = spJlist[(falseIdxJ+2)%3];
			nsegwboi->v[1].idx = -1;
			mPSJ->AddSegWBOToFace(fJidx, nsegwboi);

			xtSegmentWBO *nsegwboj = new xtSegmentWBO;
			nsegwboj->v[0].type = ON_BOUNDARY;
			nsegwboj->v[0].v = spJlist[(falseIdxJ+1)%3];
			nsegwboj->v[0].idx = (falseIdxJ+1)%3;
			nsegwboj->v[1].type = ON_BOUNDARY;
			nsegwboj->v[1].v = spJlist[(falseIdxJ+2)%3];
			nsegwboj->v[1].idx = (falseIdxJ+1)%3;
			mPSI->AddSegWBOToFace(fIidx, nsegwboj);

		} else if ( 1==numIntersectWI&&1==numIntersectWJ ) {
			std::vector<bool>::iterator iiiter = std::find(edgestateI.begin(), edgestateI.end(), true);
			const size_t iiidx = std::distance(edgestateI.begin(),iiiter);

			std::vector<bool>::iterator ijiter = std::find(edgestateJ.begin(), edgestateJ.end(), true);
			const size_t ijidx = std::distance(edgestateJ.begin(),ijiter);

			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[iiidx];
			newseg->seg1 = spJlist[ijidx];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;


			//======================================================
			xtSegmentWBO *nsegwboi = new xtSegmentWBO;
			nsegwboi->v[0].type = ON_BOUNDARY;
			nsegwboi->v[0].v = spIlist[iiidx];
			nsegwboi->v[0].idx = iiidx;
			nsegwboi->v[1].type = ON_SURFACE;
			nsegwboi->v[1].v = spJlist[ijidx];
			nsegwboi->v[1].idx = -1;
			mPSJ->AddSegWBOToFace(fJidx, nsegwboi);

			xtSegmentWBO *nsegwboj = new xtSegmentWBO;
			nsegwboj->v[0].type = ON_BOUNDARY;
			nsegwboj->v[0].v = spJlist[ijidx];
			nsegwboj->v[0].idx = ijidx;
			nsegwboj->v[1].type = ON_SURFACE;
			nsegwboj->v[1].v = spIlist[iiidx];
			nsegwboj->v[1].idx = -1;
			mPSI->AddSegWBOToFace(fIidx, nsegwboj);
			
		} else if ( 2==numIntersectWI && 0==numIntersectWJ ) {
			int falseIdx;
			for ( size_t i=0; i<edgestateI.size(); ++i ) {
				if ( !edgestateI[i] ) {
					falseIdx = i;
				}
			}
			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[(falseIdx+1)%3];
			newseg->seg1 = spIlist[(falseIdx+2)%3];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

			//======================================================
			xtSegmentWBO *nsegwboi = new xtSegmentWBO;
			nsegwboi->v[0].type = ON_BOUNDARY;
			nsegwboi->v[0].v = spIlist[(falseIdx+1)%3];
			nsegwboi->v[0].idx = (falseIdx+1)%3;
			nsegwboi->v[1].type = ON_BOUNDARY;
			nsegwboi->v[1].v = spIlist[(falseIdx+2)%3];
			nsegwboi->v[1].idx = (falseIdx+2)%3;
			mPSJ->AddSegWBOToFace(fJidx, nsegwboi);

			xtSegmentWBO *nsegwboj = new xtSegmentWBO;
			nsegwboj->v[0].type = ON_SURFACE;
			nsegwboj->v[0].v = spIlist[(falseIdx+1)%3];
			nsegwboj->v[0].idx = -1;
			nsegwboj->v[1].type = ON_SURFACE;
			nsegwboj->v[1].v = spIlist[(falseIdx+2)%3];
			nsegwboj->v[1].idx = -1;
			mPSI->AddSegWBOToFace(fIidx, nsegwboj);
			
		} else if ( 3==numIntersectWI&&0==numIntersectWJ) {
			printf( "Co point I\n" );

		} else if ( 0==numIntersectWI&&3==numIntersectWJ) {
			printf( "Co point J\n" );

		} else if ( 2==numIntersectWI&&1==numIntersectWJ) {
			//printf( "I touch on\n" ); 
			// one point of I on triangle J
			// There at least two edge intersect with J degenerate case
			std::vector<bool>::iterator findIit = std::find(edgestateI.begin(),edgestateI.end(),false);
			const size_t falseIidx = std::distance(edgestateI.begin(),findIit);
			
			std::vector<bool>::iterator findJit = std::find(edgestateJ.begin(),edgestateJ.end(),true);
			const size_t trueJidx = std::distance(edgestateJ.begin(),findJit);

			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[(falseIidx+1)%3];
			newseg->seg1 = spJlist[trueJidx];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

			//======================================================
			xtSegmentWBO *nsegwboi = new xtSegmentWBO;
			nsegwboi->v[0].type = ON_V;
			nsegwboi->v[0].v = NULL;
			nsegwboi->v[0].idx = (falseIidx+2)%3;
			nsegwboi->v[1].type = ON_SURFACE;
			nsegwboi->v[1].v = spJlist[trueJidx];
			nsegwboi->v[1].idx = -1;
			mPSJ->AddSegWBOToFace(fJidx, nsegwboi);

			xtSegmentWBO *nsegwboj = new xtSegmentWBO;
			nsegwboj->v[0].type = ON_SURFACE_V;
			nsegwboj->v[0].v = NULL;
			nsegwboj->v[0].idx = triaI.a[(falseIidx+2)%3];
			nsegwboj->v[1].type = ON_BOUNDARY;
			nsegwboj->v[1].v = spJlist[trueJidx];
			nsegwboj->v[1].idx = -1;
			mPSI->AddSegWBOToFace(fIidx, nsegwboj);

		} else if ( 1==numIntersectWI&&2==numIntersectWJ) {
			//printf( "J touch on\n" );
			// one point of J on triangle area I
			// There at lease two edge intersect by this degenerate case
			// In this branch just one edge collision
			std::vector<bool>::iterator findIit = std::find(edgestateI.begin(),edgestateI.end(),true);
			const size_t trueIidx = std::distance(edgestateI.begin(),findIit);
			
			std::vector<bool>::iterator findJit = std::find(edgestateJ.begin(),edgestateJ.end(),false);
			const size_t falseJidx = std::distance(edgestateJ.begin(),findJit);

			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[trueIidx];
			newseg->seg1 = spJlist[(falseJidx+1)%3];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

			//======================================================
			xtSegmentWBO *nsegwboi = new xtSegmentWBO;
			nsegwboi->v[0].type = ON_BOUNDARY;
			nsegwboi->v[0].v = spIlist[trueIidx];
			nsegwboi->v[0].idx = -1;
			nsegwboi->v[1].type = ON_SURFACE_V;
			nsegwboi->v[1].v = NULL;
			nsegwboi->v[1].idx = triaJ.a[(falseJidx+2)%3];
			mPSJ->AddSegWBOToFace(fJidx, nsegwboi);

			xtSegmentWBO *nsegwboj = new xtSegmentWBO;
			nsegwboj->v[0].type = ON_SURFACE;
			nsegwboj->v[0].v = spIlist[trueIidx];
			nsegwboj->v[0].idx = -1;
			nsegwboj->v[1].type = ON_V;
			nsegwboj->v[1].v = NULL;
			nsegwboj->v[1].idx = (falseJidx+2)%3;
			mPSI->AddSegWBOToFace(fIidx, nsegwboj);

		} else if ( 3==numIntersectWI ) {
			// impossible if they are not lay int the same plane
			assert(false);
		}  else {
			// unknow situation
			printf( "I\t%d, J\t%d\n",numIntersectWI,numIntersectWJ);
			assert(false);
		}


	}
}

void xtSplitBuilder::ConstructSplitSegments()
{
	for ( size_t ki=0; ki<mCE->mCollide.size(); ++ki ) {
		const int fIidx = mCE->mCollide[ki].i;
		const int fJidx = mCE->mCollide[ki].j;
		xtFaceFaceKey ffkey = {fIidx,fJidx};
		xtFaceFaceKey ffkeyinv = {fJidx, fIidx};
		xtFFMap::iterator findkey = mFFM.find(ffkey);
		// bug
		//xtFFMap::iterator findkeyinv = mFFM.find(ffkeyinv);
		if ( findkey!=mFFM.end() /*|| findkeyinv!=mFFM.end() */) continue;
		
		// let J's tri 3 edge test the I's face
		// 0) may degenerate
		// 1) one case in J 
		// 2) generate the common split segment
		std::vector<bool> edgestateI; edgestateI.reserve(3);
		std::vector<xtVector3d *> spIlist; spIlist.reserve(3);
		std::vector<bool> edgestateJ; edgestateJ.reserve(3);
		std::vector<xtVector3d *> spJlist; spJlist.reserve(3);

		xtIndexTria3 triaI = mCE->mSurfI->indices[fIidx];
		xtIndexTria3 triaJ = mCE->mSurfJ->indices[fJidx];

		// check start end collidessIdx as a key;
		for ( int i=0; i<3; ++i ) {
			xtSegmentFaceK key = { triaJ.a[i], triaJ.a[(i+1)%3], fIidx };
			xtSegmentFaceK keyinv = { triaJ.a[(i+1)%3], triaJ.a[i], fIidx };
			xtSFMap::iterator findit;
			if ( (findit=mSFMI.find(key))!=mSFMI.end() ) {
				edgestateI.push_back(true);
				spIlist.push_back(findit->second);
			} else if ( (findit=mSFMI.find(keyinv))!=mSFMI.end() ) {
				edgestateI.push_back(true);
				spIlist.push_back(findit->second);
			} else {
				edgestateI.push_back(false);
				spIlist.push_back(NULL);
			}
		}

		for ( int i=0; i<3; ++i ) {
			xtSegmentFaceK key = { triaI.a[i], triaI.a[(i+1)%3], fJidx };
			xtSegmentFaceK keyinv = { triaI.a[(i+1)%3], triaI.a[i], fJidx };
			xtSFMap::iterator findit;
			if ( (findit=mSFMJ.find(key))!=mSFMJ.end() ) {
				edgestateJ.push_back(true);
				spJlist.push_back(findit->second);
			} else if ( (findit=mSFMJ.find(keyinv))!=mSFMJ.end() ) {
				edgestateJ.push_back(true);
				spJlist.push_back(findit->second);
			} else {
				edgestateJ.push_back(false);
				spJlist.push_back(NULL);
			}
		}

		//int numIntersectWI, numIntersectWJ;
		const int numIntersectWI = std::count(edgestateI.begin(),edgestateI.end(),true);
		const int numIntersectWJ = std::count(edgestateJ.begin(),edgestateJ.end(),true);
		if ( 0==numIntersectWI && 2==numIntersectWJ ) {
			//std::vector<bool>::iterator findfalse = std::find(edgestateI.begin(),edgestateI.end(),false);
			int falseIdxJ;
			for ( size_t i=0; i<edgestateJ.size(); ++i ) {
				if ( !edgestateJ[i] ) {
					falseIdxJ = i;
				}
			}
			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spJlist[(falseIdxJ+1)%3];
			newseg->seg1 = spJlist[(falseIdxJ+2)%3];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

		} else if ( 1==numIntersectWI&&1==numIntersectWJ ) {
			std::vector<bool>::iterator iiiter = std::find(edgestateI.begin(), edgestateI.end(), true);
			const size_t iiidx = std::distance(edgestateI.begin(),iiiter);

			std::vector<bool>::iterator ijiter = std::find(edgestateJ.begin(), edgestateJ.end(), true);
			const size_t ijidx = std::distance(edgestateJ.begin(),ijiter);

			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[iiidx];
			newseg->seg1 = spJlist[ijidx];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;
			
		} else if ( 2==numIntersectWI && 0==numIntersectWJ ) {
			int falseIdx;
			for ( size_t i=0; i<edgestateI.size(); ++i ) {
				if ( !edgestateI[i] ) {
					falseIdx = i;
				}
			}
			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[(falseIdx+1)%3];
			newseg->seg1 = spIlist[(falseIdx+2)%3];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;
			
		} else if ( 3==numIntersectWI&&0==numIntersectWJ) {
			printf( "Co point I\n" );

		} else if ( 0==numIntersectWI&&3==numIntersectWJ) {
			printf( "Co point J\n" );

		} else if ( 2==numIntersectWI&&1==numIntersectWJ) {
			//printf( "I touch on\n" ); 
			// one point of I on triangle J
			// There at least two edge intersect with J degenerate case
			std::vector<bool>::iterator findIit = std::find(edgestateI.begin(),edgestateI.end(),false);
			const size_t falseIidx = std::distance(edgestateI.begin(),findIit);
			
			std::vector<bool>::iterator findJit = std::find(edgestateJ.begin(),edgestateJ.end(),true);
			const size_t trueJidx = std::distance(edgestateJ.begin(),findJit);

			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[(falseIidx+1)%3];
			newseg->seg1 = spJlist[trueJidx];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

		} else if ( 1==numIntersectWI&&2==numIntersectWJ) {
			//printf( "J touch on\n" );
			// one point of J on triangle area I
			// There at lease two edge intersect by this degenerate case
			// In this branch just one edge collision
			std::vector<bool>::iterator findIit = std::find(edgestateI.begin(),edgestateI.end(),true);
			const size_t trueIidx = std::distance(edgestateI.begin(),findIit);
			
			std::vector<bool>::iterator findJit = std::find(edgestateJ.begin(),edgestateJ.end(),false);
			const size_t falseJidx = std::distance(edgestateJ.begin(),findJit);

			xtSegment *newseg = new xtSegment;
			newseg->seg0 = spIlist[trueIidx];
			newseg->seg1 = spJlist[(falseJidx+1)%3];
			mSharedSplitSegList.push_back(newseg);

			mPSI->AddSplitSegmentToFace(fIidx,newseg);
			mPSJ->AddSplitSegmentToFace(fJidx,newseg);
			mFFM[ffkey] = newseg;

		} else if ( 3==numIntersectWI ) {
			// impossible if they are not lay int the same plane
			assert(false);
		}  else {
			// unknow situation
			printf( "I\t%d, J\t%d\n",numIntersectWI,numIntersectWJ);
			assert(false);
		}


	}
}

void xtSplitBuilder::InitializeCollisionEntity()
{
	mPSI = new xtCollisionEntity;
	mPSJ = new xtCollisionEntity;
	mPSI->InitializeCollisionEntity(mCE->mSurfI,mCE->mCollide,XTSURFACEI);
	mPSJ->InitializeCollisionEntity(mCE->mSurfJ,mCE->mCollide,XTSURFACEJ);
}

void xtSplitBuilder::DestroyMem()
{
	delete mPSI;
	mPSI = NULL;
	delete mPSJ;
	mPSJ = NULL;

	for ( size_t i=0; i<mSharedSplitSegList.size(); ++i ) {
		delete mSharedSplitSegList[i];
		mSharedSplitSegList[i] = NULL;
	}

	for ( size_t i=0; i<mSharedSplitPoints.size(); ++i ) {
		delete mSharedSplitPoints[i];
		mSharedSplitPoints[i] = NULL;
	}

}

void xtSplitBuilder::ConcatenationPolyLine(std::vector<std::tuple<int,int>> &segonsurfindices, const int numofverts)
{
	// concate the line
	//std::vector<std::tuple<int,int>> vertslot;
	//for ( size_t i=0; i<segonsurfverts.size(); ++i ) {
	//	std::tuple<int,int> t(-1,-1);
	//	vertslot.push_back(t);
	//}
	std::vector<std::vector<int>> vertslotv;
	for ( size_t i=0; i<numofverts; ++i ) {
		std::vector<int> v;
		v.reserve(2);
		vertslotv.push_back(v);
	}
	for ( size_t i=0; i<segonsurfindices.size(); ++i ) {
		std::tuple<int,int> seg = segonsurfindices[i];
		vertslotv[std::get<0>(seg)].push_back(i);
		vertslotv[std::get<1>(seg)].push_back(i);
	}
	// first is the seg second is the seg's start vertex idx
	std::vector<std::tuple<int, int>> startendsegidx;
	for ( size_t i=0; i<vertslotv.size(); ++i ) {
		if ( vertslotv[i].size()==1 ) {
			std::tuple<int, int> t(vertslotv[i][0],i);
			startendsegidx.push_back(t);
		} 
	}
	assert(startendsegidx.size());
	int startsegidx = std::get<0>(startendsegidx[0]);
	int startveridx = std::get<1>(startendsegidx[0]);
	if ( std::get<0>(segonsurfindices[startsegidx])!=startveridx ) {
		std::swap(
			std::get<0>(segonsurfindices[startsegidx]),
			std::get<1>(segonsurfindices[startsegidx]));
	}
	int endvertidx = std::get<1>(segonsurfindices[startsegidx]);
	std::vector<int> seqsegvec;
	seqsegvec.push_back(startsegidx);

	std::list<int> segpool;
	for ( size_t i=0; i<segonsurfindices.size(); ++i ) {
		segpool.push_back(i);
	}
	std::list<int>::iterator segfit = std::find( segpool.begin(), segpool.end(), startsegidx );
	segpool.erase(segfit);
	while( !segpool.empty() ) {
		// find endvertidx
		for ( std::list<int>::iterator it=segpool.begin(); it!=segpool.end(); ++it ) {
			std::tuple<int,int> &t = segonsurfindices[*it];
			if ( std::get<0>(t)==endvertidx ) {
				endvertidx = std::get<1>(t);
				seqsegvec.push_back(*it);
				segpool.erase(it);
				break;
			} else if ( std::get<1>(t)==endvertidx ) {
				std::swap( std::get<0>(t), std::get<1>(t) );
				endvertidx = std::get<1>(t);
				seqsegvec.push_back(*it);
				segpool.erase(it);
				break;
			}
		}
	}
	for ( size_t i=0; i<segonsurfindices.size()/2; ++i ) {
		std::swap( segonsurfindices[i], segonsurfindices[seqsegvec[i]] );
	}
}

void xtSplitBuilder::FilterAdjacentPnts(std::vector<std::tuple<int,int>> &segonsurfindices, std::vector<xtVertexRobust *> &segonsurfverts )
{
	
}

void xtSplitBuilder::FilterAdjacentVerts2d(std::vector<xtTriPnt2> &verts2d, std::vector<xtSeg2WithMarker> &segmarkerlist)
{
	std::vector<xtSeg2WithMarker> cacheback;
	const double precision = 0.000001;
	for ( size_t i=3; i<segmarkerlist.size(); ++i ) {
		xtSeg2WithMarker &segmarker = segmarkerlist[i];
		double squaredist = SquareDistance(verts2d[segmarker.seg[0]],verts2d[segmarker.seg[1]]);
		if ( squaredist>precision ) {
			cacheback.push_back(segmarker);
		} else {
			if ( i>0 && i<segmarkerlist.size()-1 ) {
				xtSeg2WithMarker &pre = segmarkerlist[i-1];
				xtSeg2WithMarker &pos = segmarkerlist[i+1];
				pos.seg[0] = pre.seg[1];
			} else if ( i==0 ) {
				//xtSeg2WithMarker &pos = segmarkerlist[i+1];
				//pos.seg[0] = segmarker.seg[0];
			} else if ( i==segmarkerlist.size()-1 ) {
				//xtSeg2WithMarker &pre = segmarkerlist[i-1];
				//pre.seg[1] = segmarker.seg[0];
			}
		}
	}
	segmarkerlist.erase(segmarkerlist.begin()+3,segmarkerlist.end());
	segmarkerlist.insert(segmarkerlist.begin()+3,cacheback.begin(),cacheback.end());
}

// Got distance from 
// Got index from iterator in the vector
// http://stackoverflow.com/questions/2152986/best-way-to-get-the-index-of-an-iterator
/**
I would prefer it - vec.begin() precisely for the opposite reason given by Naveen: 
so it wouldn't compile if you change the vector into a list. If you do this during 
every iteration, you could easily end up turning an O(n) algorithm into an O(n^2) 
algorithm.
Another option, if you don't jump around in the container during iteration, would be 
to keep the index as a second loop counter.
*/
// C++ STL Vectors: Get iterator from index?
// http://stackoverflow.com/questions/671423/c-stl-vectors-get-iterator-from-index
/**
vector<Type>::iterator nth = v.begin() + index;
*/