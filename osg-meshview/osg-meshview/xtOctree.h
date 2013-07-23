#pragma once
#include "xtOctreeNode.h"
#include <vector>
#include <queue>

#include "xtTriangleBox3Overlap.h"

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

	void InsertTriangle( double p0[], double p1[], double p2[] )
	{
		InsertTriangle( p0, p1, p2, this->mRoot );
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

	void DumpAxisXSection(double xplane, std::vector<xtOctreeNode<T> *> &intersectnode)
	{
		DumpAxisSection( xplane, 0, intersectnode );
	}

	void DumpAxisYSection(double yplane, std::vector<xtOctreeNode<T> *> &intersectnode)
	{
		DumpAxisSection( yplane, 1, intersectnode );
	}

	void DumpAxisZSection(double zplane, std::vector<xtOctreeNode<T> *> &intersectnode)
	{
		DumpAxisSection( zplane, 2, intersectnode );
	}

private:

	void DumpAxisSection( double v, int axis, std::vector<xtOctreeNode<T> *> &intersectnode ) 
	{
		assert(axis>=0 && axis<=2);
		// Initial xplane didn't intersect with the root box
		if ( mRoot->mLB[axis]>v || mRoot->mRT[axis]<v) {
			return ;
		}

		std::queue<xtOctreeNode<T> *> nodequeue;
		nodequeue.push(mRoot);

		while ( !nodequeue.empty() ) {
			xtOctreeNode<T> *currentnode = nodequeue.front();
			nodequeue.pop();

			if ( currentnode->mLB[axis]<=v && currentnode->mRT[axis]>=v ) {
				intersectnode.push_back(currentnode);
				if ( currentnode->mChild ) {
					for ( int i=0; i<8; ++i ) {
						nodequeue.push(&(currentnode->mChild[i]));
					}
				}
			}
		}
	}

private:
	//void InsertElement

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

	void InsertTriangle(double p0[], double p1[], double p2[], xtOctreeNode<T> *root )
	{
		if ( IsTriangleInCubeT(p0,p1,p2, root) ) {
			root->mState = OCTPARTIAL;
			if ( root->mDepth == this->mLargestDepth ) {
				root->mState = OCTPARTIAL;
			} else {
				if ( !root->mChild ) {
					root->AddChild(this->mLargestDepth);
				}

				for ( int i=0; i<8; ++i ) {
					InsertTriangle(p0,p1,p2,&(root->mChild[i]));
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

	static bool IsTriangleCubeOverlap( double p0[], double p1[], double p2[], double min[], double max[] ) 
	{
		float tri[3][3] = {
			{(float) p0[0], (float) p0[1], (float) p0[2], },
			{(float) p1[0], (float) p1[1], (float) p1[2], },
			{(float) p2[0], (float) p2[1], (float) p2[2], }
		};

		float boxcenter[3] = { (float)(min[0]+max[0])/2.,(float)(min[1]+max[1])/2.,(float)(min[2]+max[2])/2. };
		float boxhalfsize[3] = { (float)(-min[0]+max[0])/2.,(float)(-min[1]+max[1])/2.,(float)(-min[2]+max[2])/2. };

		return 1==triBoxOverlap(boxcenter,boxhalfsize,tri);
	}

	// becarefull the offset!!!
	bool IsDoublePntInCubeT(double pnt[], xtOctreeNode<T> *root)
	{
		// code should be extract as function!!!
		xtPnt3<int> cubMin = root->mLB + this->mCenter;
		xtPnt3<int> cubMax = root->mRT + this->mCenter;

		double dcubMin[3] = {(double)cubMin.x,(double)cubMin.y,(double)cubMin.z};
		double dcubMax[3] = {(double)cubMax.x,(double)cubMax.y,(double)cubMax.z};
		
		return IsPointCubeOverlap(pnt,dcubMin,dcubMax);
	}

	bool IsTriangleInCubeT(double p0[], double p1[], double p2[], xtOctreeNode<T> *root ) 
	{
		xtPnt3<int> cubMin = root->mLB + this->mCenter;
		xtPnt3<int> cubMax = root->mRT + this->mCenter;

		double dcubMin[3] = {(double)cubMin.x,(double)cubMin.y,(double)cubMin.z};
		double dcubMax[3] = {(double)cubMax.x,(double)cubMax.y,(double)cubMax.z};
	
		return IsTriangleCubeOverlap(p0,p1,p2,dcubMin, dcubMax);
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

