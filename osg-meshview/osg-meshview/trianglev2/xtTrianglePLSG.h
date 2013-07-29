#pragma once
#include <vector>
#include <string>

struct xtTriPnt2 
{
	double p[2];
};

struct xtSeg2WithMarker
{
	int seg[2];
	int marker;
};

struct xtTriIndexO
{
	int idx[3];
};

struct triangulateio;
class xtTrianglePLSG
{
public:
	//xtTrianglePLSG(void);
	xtTrianglePLSG( 
		std::vector<xtTriPnt2> &verts, std::vector<xtSeg2WithMarker> &segs,
		std::vector<xtTriIndexO> &tris);
	//void GetResults();

	~xtTrianglePLSG(void);

	void PrintOutInfo();

private:
	void report(struct triangulateio *io,
		int markers,
		int reporttriangles,
		int reportneighbors,
		int reportsegments,
		int reportedges,
		int reportnorms);

private:
	triangulateio *mIn;
	triangulateio *mOut;

	std::vector<xtTriPnt2> mOutVerts;
	std::vector<int> mVertsMarkers;
	std::vector<xtTriIndexO> mTris;
};

