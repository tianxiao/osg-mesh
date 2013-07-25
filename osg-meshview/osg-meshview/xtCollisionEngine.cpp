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

void SetM4Tran2(xtVector3d &tran0, xtMatrix3d &rot0, xtVector3d &tran1, xtMatrix3d &rot1, xtMatrix4d &rlt120 )
{
	xtMatrix4d m0;
	SetMatrix4FromTranRot(m0,tran0,rot0);

	xtMatrix4d m1;
	SetMatrix4FromTranRot(m1,tran1,rot1);

	rlt120 = m0.inverse()*m1;

	//AssignM4ToM3(mj2i,rotj2i);
	//AssignColM4ToV3(mj2i,tranj2i,3);
}

void xtCollisionEngine::SetM4J2I()
{
	xtMatrix4d mj2i;
	SetM4Tran2(mSurfI->tran,mSurfI->rot,mSurfJ->tran,mSurfJ->rot,mj2i);

	AssignM4ToM3(mj2i,rotj2i);
	AssignColM4ToV3(mj2i,tranj2i,3);
}

void xtCollisionEngine::SetM4I2J()
{
	xtMatrix4d mi2j;
	SetM4Tran2(mSurfJ->tran,mSurfJ->rot,mSurfI->tran,mSurfI->rot,mi2j);

	AssignM4ToM3(mi2j,roti2j);
	AssignColM4ToV3(mi2j,trani2j,3);
}


bool xtCollisionEngine::Collide()
{
	SetM4J2I();
	SetM4I2J();

	int nIntersects = 0;
	std::vector<int> indices;
	
	mCollide.clear();
	for ( size_t i=0; i<mJBBoxList.size(); ++i ) {
		indices.clear();
		// transformation !!! do
		xtBBOX box = mJBBoxList[i];
		box.TranRot(tranj2i,rotj2i);
		int nhits = mT->tree.Search(&(box.min[0]),&(box.max[0]),SearchCallBack,(void*) &indices);
		// detailed tri-tri check
		for ( int j=0; j<nhits; ++j ) {
			if ( TriTriCollision(indices[j],i) ) {
				nIntersects++;
				xtCollidePair pair;
				pair.i = indices[j];
				pair.j = i;
				mCollide.push_back(pair);

				// Detail split the triangle

			}
		}
#if RTREE_DEBUG_RESULT
		if (nhits) {
			tempIBoxList.push_back(indices);
			tempJlist.push_back(i);
		}
#endif
		
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

	//ipa = mSurfI->tran + mSurfI->rot*ipa;
	//ipb = mSurfI->tran + mSurfI->rot*ipb;
	//ipc = mSurfI->tran + mSurfI->rot*ipc;

	std::vector<xtVector3d> &jverts = mSurfJ->verts;
	xtIndexTria3 &jtria = mSurfJ->indices[j];
	xtVector3d jpa = jverts[jtria.a[0]];
	xtVector3d jpb = jverts[jtria.a[1]];
	xtVector3d jpc = jverts[jtria.a[2]];

	//jpa = mSurfJ->tran + mSurfJ->rot*jpa;
	//jpb = mSurfJ->tran + mSurfJ->rot*jpb;
	//jpc = mSurfJ->tran + mSurfJ->rot*jpc;
	jpa = tranj2i + rotj2i*jpa;
	jpb = tranj2i + rotj2i*jpb;
	jpc = tranj2i + rotj2i*jpc;

	float v0[3] = {(float)ipa.x(),(float)ipa.y(),(float)ipa.z()};
	float v1[3] = {(float)ipb.x(),(float)ipb.y(),(float)ipb.z()};
	float v2[3] = {(float)ipc.x(),(float)ipc.y(),(float)ipc.z()};

	float u0[3] = {(float)jpa.x(),(float)jpa.y(),(float)jpa.z()};
	float u1[3] = {(float)jpb.x(),(float)jpb.y(),(float)jpb.z()};
	float u2[3] = {(float)jpc.x(),(float)jpc.y(),(float)jpc.z()};

	if ( 1==NoDivTriTriIsect(v0,v1,v2,u0,u1,u2) ) {
		return true;
	} else {
		return false;
	}
	
}

void xtCollisionEngine::BuildSurfBBOX()
{
	BuildSurfaceBBox(this->mSurfI,this->mIBBoxList);
	BuildSurfaceBBox(this->mSurfJ,this->mJBBoxList);
}
