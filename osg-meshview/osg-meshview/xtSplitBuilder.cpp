#include "StdAfx.h"
#include "xtSplitBuilder.h"
#include "xtCollisionEngine.h"
#include "xtGeometrySurfaceData.h"
#include "xtRayTriOverlay.h"
#include <algorithm>

void xtCollisionEntity::DestroyMem()
{
	for ( size_t i=0; i<surfslot.size(); ++i ) {
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
	ConstructSplitSegments();
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
					dir.normalize();
					if ( IntersectTriangleTemplate(startPntJ, dir,pa,pb,pc,&t,&u,&v) ) {
						xtVector3d *newsplitPnt = new xtVector3d(startPntJ+t*dir);
						mSharedSplitPoints.push_back(newsplitPnt);
						currssI->pointsOnSurf.push_back(newsplitPnt);
						colledseg->pointOnSeg.push_back(newsplitPnt);
						sfmap[key]=newsplitPnt;
					}
				}
			}
		}
	}
}



void xtSplitBuilder::TessellateCollidedFace(xtCollisionEntity *ps, xtGeometrySurfaceDataS *surf)
{
	
}

bool counteriftrue( bool & istrue )
{
	return istrue;
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

			mFFM[ffkey] = newseg;
			
		} else if ( 3==numIntersectWI ) {
			// impossible if they are not lay int the same plane
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
