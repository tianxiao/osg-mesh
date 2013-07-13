#pragma once
#include "xtOctreeNode.h"
#include <vector>
#include <queue>

template<typename T>
class xtOctree
{
public:
	xtOctree(T center[], unsigned int depth) 
	{
		mCenter.x = center[0];
		mCenter.y = center[1];
		mCenter.z = center[2];
		mLargestDepth = depth;

		assert(depth>0);

		T width = 1<<(depth);
		xtPnt3<T> lb, rt;
		lb.x = -width;
		lb.y = -width;
		lb.z = -width;
		rt.Set(width, width, width);

		mRoot = new xtOctreeNode<T>;
		mRoot->SetNode(NULL,lb,rt,0);

	}
	~xtOctree(void);

	void InsertPoint(double pnt[]) 
	{
		InsertPoint(pnt,this->mRoot);
	}

	void DumpLevel(unsigned int level, std::vector<xtOctreeNode<T> *> &levelnodelist)
	{
		std::queue<xtOctreeNode<T> *> nodequeue;
		nodequeue.push(mRoot);

		while( !nodequeue.empty() ) {
			xtOctreeNode<T> *currentnode = nodequeue.front();
			nodequeue.pop();

			if ( currentnode->mDepth == level && currentnode->mState == OCTPARTIAL) {
				levelnodelist.push_back( currentnode );
			} else if ( currentnode->mDepth < level ) {
				if ( currentnode->mChild ) {
					for ( int i=0; i<8; ++i ) {
						nodequeue.push(&(currentnode->mChild[i]));
					}
				}
			}
		}
	}

private:
	void InsertPoint(double pnt[], xtOctreeNode<T> *root) 
	{
		if ( IsDoublePntInCubeT(pnt, root) ) {
			root->mState = OCTPARTIAL;
			if ( root->mDepth == this->mLargestDepth ) {
				root->mState = OCTPARTIAL;
			} else {
				if ( !root->mChild ) {
					root->AddChild(this->mLargestDepth);
				}

				for ( int i=0; i<8; ++i ) {
					InsertPoint(pnt,&(root->mChild[i]));
				}
			}
			//return;
		} else {
			// bug fixed
			// This branch will cause a overlapping. The checked at one point
			// the other point who don't overlap will set this point to empty!
			//root->mState = OCTEMPTY;
		}
		
	}

	static bool IsPointCubeOverlap(double p[], double min[], double max[])
	{
		for ( int i=0; i<3; ++i ) {
			if ( min[i]>p[i] ||
				 max[i]<p[i] )
				return false;
		}

		return true;
	}

	// becarefull the offset!!!
	bool IsDoublePntInCubeT(double pnt[], xtOctreeNode<T> *root)
	{

		xtPnt3<int> cubMin = root->mLB + this->mCenter;
		xtPnt3<int> cubMax = root->mRT + this->mCenter;

		double dcubMin[3] = {(double)cubMin.x,(double)cubMin.y,(double)cubMin.z};
		double dcubMax[3] = {(double)cubMax.x,(double)cubMax.y,(double)cubMax.z};
		
		return IsPointCubeOverlap(pnt,dcubMin,dcubMax);
	}


private:
	xtOctreeNode<T> *mRoot;
	xtPnt3<T> mCenter;

	unsigned int mLargestDepth;

};

template<typename T>
xtOctree<T>::~xtOctree(void)
{
}

