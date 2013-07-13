#pragma once
#include <assert.h>

template<typename T>
struct xtPnt3
{
	T x, y, z;

	xtPnt3 operator+( xtPnt3 &lhs ) 
	{
		xtPnt3 tem;
		tem.x = x+lhs.x;
		tem.y = y+lhs.y;
		tem.z = z+lhs.z;
		return tem;
	}

	xtPnt3 Half() {
		xtPnt3 temp;
		temp.x = x/2;
		temp.y = y/2;
		temp.z = z/2;
		return temp;
	}

	void Set( T x, T y, T z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

};


enum xtOctreeIntersectState
{
	OCTPARTIAL,
	OCTFULL,
	OCTEMPTY,
};

template<typename T>
class xtOctreeNode
{
public:
	xtOctreeNode(void);
	~xtOctreeNode(void);


	//===============================================
	xtOctreeNode *mParent;
	xtOctreeNode *mChild;
	unsigned int mDepth;
	xtOctreeIntersectState mState;

	void *pv;

	xtPnt3<T> mLB;   // left bottom
	xtPnt3<T> mRT;   // right top
	//===============================================

	void SetNode(xtOctreeNode<T> *parent, const xtPnt3<T> &lb, const xtPnt3<T> &rt, const int depth)
	{
		this->mParent = parent;
		this->mLB = lb;
		this->mRT = rt;
		this->mDepth = depth;
		this->mChild = NULL;
		
	}

	void SetNode(xtOctreeNode *parent,const int depth)
	{
		this->mParent = parent;
		this->mDepth = depth;
		this->mChild = NULL;
		this->mState = OCTPARTIAL;
	}

	void SetNode(xtPnt3<T> &lb, xtPnt3<T> &rt) 
	{
		this->mLB = lb;
		this->mRT = rt;
	}

	void AddChild(const unsigned int LD)
	{
		assert(LD>this->mDepth);
		mChild = new xtOctreeNode[8];
		for ( int i=0; i<8; ++i ) {
			mChild[i].SetNode(this,this->mDepth+1);
		}

		// split this into 8 cube
		xtPnt3<T> p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12;

		p0 = (mLB+mRT).Half();
		mChild[0].mLB = mLB;
		mChild[0].mRT = p0;

		p7.Set(mLB.x,mLB.y,p0.z);
		p8.Set(p0.x,p0.y,mRT.z);
		mChild[1].mLB = p7;
		mChild[1].mRT = p8;

		p1.Set(mLB.x,p0.y,mLB.z);
		p4.Set(p0.x,mRT.y,p0.z);
		mChild[2].mLB = p1;
		mChild[2].mRT = p4;

		p9.Set(p1.x,p1.y,p0.z);
		p10.Set(p4.x,p4.y,mRT.z);
		mChild[3].mLB = p9;
		mChild[3].mRT = p10;

		p2.Set(p0.x,mLB.y,mLB.z);
		p6.Set(mRT.x,p0.y,p0.z);
		mChild[4].mLB = p2;
		mChild[4].mRT = p6;

		p11.Set(p2.x,p2.y,p0.z);
		p12.Set(p6.x,p6.z,mRT.z);
		mChild[5].mLB = p11;
		mChild[5].mRT = p12;

		p3.Set(p0.x,p0.y,mLB.z);
		p5.Set(mRT.x,mRT.y,p0.z);
		mChild[6].mLB = p3;
		mChild[6].mRT = p5;

		mChild[7].mLB = p0;
		mChild[7].mRT = mRT;

	}



};

template<typename T>
xtOctreeNode<T>::xtOctreeNode(void)
{
}

template<typename T>
xtOctreeNode<T>::~xtOctreeNode(void)
{
}

