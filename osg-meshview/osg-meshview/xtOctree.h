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

			if ( currentnode->mDepth == level ) {
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
		}
	}

	bool IsDoublePntInCubeT(double pnt[], xtOctreeNode<T> *root)
	{
		for ( int i=0; i<3; ++i ) {
			if ( (&(root->mLB.x))[i]>pnt[i] ||
				(&(root->mRT.x))[i]<pnt[i])
				return false;
		}

		return true;
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

