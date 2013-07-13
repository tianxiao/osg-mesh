#pragma once
#include <vector>
#include <assert.h>
#include <math.h>

template<typename T>
struct xtPnt3Adapter
{
	T x, y, z;
	void Set(T x, T y, T z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void Min(xtPnt3Adapter<T> &rhs) 
	{
		x = x<rhs.x?x:rhs.x;
		y = y<rhs.y?y:rhs.y;
		z = z<rhs.z?z:rhs.z;
	}

	void Max(xtPnt3Adapter<T> &rhs)
	{
		x = x>rhs.x?x:rhs.x;
		y = y>rhs.y?y:rhs.y;
		z = z>rhs.z?z:rhs.z;
	}

	xtPnt3Adapter<T> operator+( xtPnt3Adapter<T> &rhs ) 
	{
		xtPnt3Adapter<T> temp;
		temp.x = x+rhs.x;
		temp.y = y+rhs.y;
		temp.z = z+rhs.z;
		return temp;
	}

	xtPnt3Adapter<T> operator-( xtPnt3Adapter<T> &rhs ) 
	{
		xtPnt3Adapter<T> temp;
		temp.x = x - rhs.x;
		temp.y = y - rhs.y;
		temp.z = z - rhs.z;
		return temp;
	}

	xtPnt3Adapter<T> operator*( double scale ) {
		xtPnt3Adapter<T> temp;
		temp.x = x*scale;
		temp.y = y*scale;
		temp.z = z*scale;
		return temp;
	}

	T Longest() {
		T temp = x>y?x:y;
		temp = temp>z?temp:z;
		return temp;
	}
	
};

class xtOctreeAdapter
{
public:
	xtOctreeAdapter(void);
	~xtOctreeAdapter(void);

	xtPnt3Adapter<double> &GetCenter() { return mCenter; };
	xtPnt3Adapter<double> &GetExtend() { return mExtend; };

	void ReserveSize(int vsize) {
		mPnts.reserve(vsize);
	}

	void AddPoint(double pnt[])
	{
		xtPnt3Adapter<double> dpnt;
		dpnt.Set(pnt[0],pnt[1],pnt[2]);
		mPnts.push_back(dpnt);
	}

	void CalculateBBox()
	{
		assert(mPnts.size());

		mMin = mPnts[0];
		mMax = mMin;

		for ( size_t i=0; i<mPnts.size(); ++i ) {
			mMin.Min(mPnts[i]);
			mMax.Max(mPnts[i]);
		}
	}

	int AdaptDataForOctree(const double resolution)
	{
		CalculateBBox();

		mCenter = (mMin+mMax)*0.5;
		mExtend = (mMax - mMin)*0.5;

		const double llength = mExtend.Longest();
		const double numGrid = llength/resolution;

		int depth = (int) ( log(numGrid)/log(2.)+1 );

		const double scalefactor = (1<<depth)/llength;

		for ( size_t i=0; i<mPnts.size(); ++i ) {
			mPnts[i] = mCenter + ( mPnts[i] - mCenter ) *scalefactor;
		}

		return depth;
	}

	std::vector<xtPnt3Adapter<double>> &GetPnts() { return mPnts; };

private:
	std::vector<xtPnt3Adapter<double>> mPnts;
	xtPnt3Adapter<double> mMin;
	xtPnt3Adapter<double> mMax;
	xtPnt3Adapter<double> mCenter;
	xtPnt3Adapter<double> mExtend;
};

