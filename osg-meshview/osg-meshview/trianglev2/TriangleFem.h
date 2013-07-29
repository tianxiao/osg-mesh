#pragma once
#include "triangle.h"
#include <vector>
#include <string>
using namespace std;

class TriangleFem
{
public:
	TriangleFem(vector< vector<double> > &bVerticeVect,
				vector< vector<int> > &bSegmentVect,
				//vector<int> &bSegmentMarkerList,
				vector< vector<double> > &holePointVect,
				//The output
				vector< vector<double> > &outVertice,
				vector<int> &verticesMarkerVect,
				vector< vector<int> > &outTriEle,
				string area
				);
	~TriangleFem(void);
	//vector< vector<double> > bVerticeVect;
	//vector<int> bVerticeCond;
	//vector< vector<double> > holePointVect;


	void triangleCFD();
	void TriangleFem::report(struct triangulateio *io,
		int markers,
		int reporttriangles,
		int reportneighbors,
		int reportsegments,
		int reportedges,
		int reportnorms);

private:
	struct triangulateio in;
	struct triangulateio out;
	vector< vector<double> > &outVertice;
	vector<int> &verticesMarkerVect;
	vector< vector<int> > &outTriEle;
	//constrainArea is used to fit the mesh area
	string constrainArea;


};

