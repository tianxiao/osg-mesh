#include "StdAfx.h"
#include "xtCollisionEngine.h"
#include "RTree.h"
#include "xtGeometrySurfaceData.h"
#include "xtPrimitiveTestUtil.h"

typedef RTree<int, double, 3> xtRTree;

struct xtRtreeImp
{
	xtRTree tree;
};


xtCollisionEngine::xtCollisionEngine(void)
{
	mT = new xtRtreeImp;
}


xtCollisionEngine::~xtCollisionEngine(void)
{
	delete mT;
	mT = NULL;
}

void xtCollisionEngine::BuildRTree()
{
	BuildSurfBBOX();

	for ( size_t i=0; i<mIBBoxList.size(); ++i ) {
		mT->tree.Insert(&(mIBBoxList[i].min[0]), &(mIBBoxList[i].max[0]), i);
	}

	//
	
}

bool SearchCallBack(int id, void *arg)
{
	std::vector<int> *fidlist = static_cast<std::vector<int>*>(arg);
	fidlist->push_back(id);
	return true;
}

bool xtCollisionEngine::Collide()
{
	int nIntersects = 0;
	std::vector<int> indices;
	
	mCollide.clear();
	for ( size_t i=0; i<mJBBoxList.size(); ++i ) {
		indices.clear();
		// transformation !!! do
		xtBBOX box = mJBBoxList[i];
		int nhits = mT->tree.Search(&(box.min[0]),&(box.max[0]),SearchCallBack,(void*) &indices);
		// detailed tri-tri check
		for ( int j=0; j<nhits; ++j ) {
			if ( TriTriCollision(indices[j],i) ) {
				nIntersects++;
				xtCollidePair pair;
				pair.i = i; 
				pair.j = indices[j];
				mCollide.push_back(pair);
			}
		}
	}
	if ( 0<nIntersects ) {
		return true;
	} else {
		return false;
	}
	
}

bool xtCollisionEngine::TriTriCollision(int i, int j)
{
	std::vector<xtVector3d> &iverts = mSurfI->verts;
	xtIndexTria3 &itria = mSurfI->indices[i];
	xtVector3d ipa = iverts[itria.a[0]];
	xtVector3d ipb = iverts[itria.a[1]];
	xtVector3d ipc = iverts[itria.a[2]];

	ipa = mSurfI->tran + mSurfI->rot*ipa;
	ipb = mSurfI->tran + mSurfI->rot*ipb;
	ipc = mSurfI->tran + mSurfI->rot*ipc;

	std::vector<xtVector3d> &jverts = mSurfJ->verts;
	xtIndexTria3 &jtria = mSurfJ->indices[i];
	xtVector3d jpa = jverts[jtria.a[0]];
	xtVector3d jpb = jverts[jtria.a[1]];
	xtVector3d jpc = jverts[jtria.a[2]];

	jpa = mSurfJ->tran + mSurfJ->rot*jpa;
	jpb = mSurfJ->tran + mSurfJ->rot*jpb;
	jpc = mSurfJ->tran + mSurfJ->rot*jpc;

	float v0[3] = {(float)ipa.x(),(float)ipa.y(),(float)ipa.z()};
	float v1[3] = {(float)ipb.x(),(float)ipb.y(),(float)ipb.z()};
	float v2[3] = {(float)ipc.x(),(float)ipc.y(),(float)ipc.z()};

	float u0[3] = {(float)jpa.x(),(float)jpa.y(),(float)jpa.z()};
	float u1[3] = {(float)jpb.x(),(float)jpb.y(),(float)jpb.z()};
	float u2[3] = {(float)jpc.x(),(float)jpc.y(),(float)jpc.z()};

	if ( 1==NoDivTriTriIsect(v0,v1,v2,u0,u1,u2) ) {
		return true;
		xtCollidePair cp = {i,j};
		mCollide.push_back(cp);
	} else {
		return false;
	}
	
		//;, ipb, ipc;
	//xtVector3d jpa, jpb, jpc;
	return false;
}

void xtCollisionEngine::BuildSurfBBOX()
{
	BuildSurfaceBBox(this->mSurfI,this->mIBBoxList);
	BuildSurfaceBBox(this->mSurfJ,this->mJBBoxList);
}
