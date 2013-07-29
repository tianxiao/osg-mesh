#include "stdafx.h"
#include "common.h"

#include "TriangleFem.h"
#include <vector>
//#include "triangle.h"
#include <stdio.h>
#include <stdlib.h>

TriangleFem::TriangleFem(vector< vector<double> > &bVerticeVect,
	vector< vector<int> > &bSegmentVect,
	//vector<int> &bSegmentMarkerList,
	vector< vector<double> > &holePointVect,
	//The output vector
	vector< vector<double> > &outVertice0,
	vector<int> &verticesMarkerVect0,
	vector< vector<int> > &outTriEle0,
	//The area constrain
	string area)
	//Initialize the output vector
	:outVertice(outVertice0),
	verticesMarkerVect(verticesMarkerVect0),
	outTriEle(outTriEle0)
{

	in.numberofpoints = bVerticeVect.size();
	in.numberofpointattributes = 0;
	in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
	//Initialize the point coordinate
	for(int nodeIndex=0; nodeIndex<in.numberofpoints; nodeIndex++)
	{
		in.pointlist[nodeIndex*2] = bVerticeVect[nodeIndex][0];
		in.pointlist[nodeIndex*2+1] = bVerticeVect[nodeIndex][1];
		//cout << in.pointlist[nodeIndex*2] <<
		//	in.pointlist[nodeIndex*2+1] << endl;
	}

	in.numberofsegments = bSegmentVect.size();
	in.segmentlist = (int *) malloc(in.numberofsegments * 2 * sizeof(int));
	in.segmentmarkerlist = (int *) malloc(in.numberofsegments * sizeof(int));

	//Initialize the segment point  end1 and end2
	for(int segIndex=0; segIndex<in.numberofsegments; segIndex++)
	{
		in.segmentlist[2*segIndex  ] = bSegmentVect[segIndex][0];
		in.segmentlist[2*segIndex+1] = bSegmentVect[segIndex][1];
		in.segmentmarkerlist[segIndex] = bSegmentVect[segIndex][2];
	}

	in.numberofholes = holePointVect.size();
	in.holelist = (REAL *) malloc(in.numberofholes * 2 * sizeof(REAL));
	//Initialize the holes
	for(int holeIndex=0; holeIndex<in.numberofholes; holeIndex++)
	{
		in.holelist[2*holeIndex  ] = holePointVect[holeIndex][0];
		in.holelist[2*holeIndex+1] = holePointVect[holeIndex][1];
	}

	in.numberofregions = 0;

	in.pointmarkerlist = (int *) NULL;


	//make necessary initialization of out
	out.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
	out.pointmarkerlist = (int *) NULL;
	out.trianglelist = (int *) NULL;
	//out.triangleattributelist = (REAL *) NULL;
	out.segmentlist = (int *) NULL;
	out.segmentmarkerlist = (int *) NULL;


	//Initial the constrain area
	constrainArea = area;


}


TriangleFem::~TriangleFem(void)
{

	free(in.pointlist);
	free(in.segmentlist);
	free(in.segmentmarkerlist);
	free(in.holelist);


	free(out.pointlist);
	free(out.pointmarkerlist);
	free(out.trianglelist);
	free(out.segmentlist);
	free(out.segmentmarkerlist);

}


void TriangleFem::triangleCFD()
{
	printf("Input point set:\n\n");
	report(&in, 0, 0, 0, 3, 0, 0);
	//Here is the hard code parameter
	//We will add other parameter to fit futrue use
	string parameterStr = "pqjzqa" ;
	parameterStr += constrainArea;
	//The hard code parameters
	//   "pqjzqa150"
	char * temp = (char *)parameterStr.c_str();
	triangulate(temp, &in, &out, (struct triangulateio *) NULL);
	report(&out,1, 1, 0, 0, 0, 0);


	//Pack the output
	vector<double> tempdVect;
	for(int verticeIndex=0; verticeIndex<out.numberofpoints; verticeIndex++)
	{
		tempdVect.push_back(out.pointlist[2*verticeIndex  ]);
		tempdVect.push_back(out.pointlist[2*verticeIndex+1]);
		outVertice.push_back(tempdVect);
		tempdVect.clear();
		verticesMarkerVect.push_back(out.pointmarkerlist[verticeIndex]);
	}

	vector<int> tempiVect;
	for(int triIndex=0; triIndex<out.numberoftriangles; triIndex++)
	{
		tempiVect.push_back(out.trianglelist[3*triIndex  ]);
		tempiVect.push_back(out.trianglelist[3*triIndex+1]);
		tempiVect.push_back(out.trianglelist[3*triIndex+2]);
		outTriEle.push_back(tempiVect);
		tempiVect.clear();
	}


}



void TriangleFem::report(struct triangulateio *io,
	int markers,
	int reporttriangles,
	int reportneighbors,
	int reportsegments,
	int reportedges,
	int reportnorms)
{
  int i, j;

  for (i = 0; i < io->numberofpoints; i++) {
    printf("Point %4d:", i);
    for (j = 0; j < 2; j++) {
      printf("  %.6g", io->pointlist[i * 2 + j]);
    }
    if (io->numberofpointattributes > 0) {
      printf("   attributes");
    }
    for (j = 0; j < io->numberofpointattributes; j++) {
      printf("  %.6g",
             io->pointattributelist[i * io->numberofpointattributes + j]);
    }
    if (markers) {
      printf("   marker %d\n", io->pointmarkerlist[i]);
    } else {
      printf("\n");
    }
  }
  printf("\n");

  if (reporttriangles || reportneighbors) {
    for (i = 0; i < io->numberoftriangles; i++) {
      if (reporttriangles) {
        printf("Triangle %4d points:", i);
        for (j = 0; j < io->numberofcorners; j++) {
          printf("  %4d", io->trianglelist[i * io->numberofcorners + j]);
        }
        if (io->numberoftriangleattributes > 0) {
          printf("   attributes");
        }
        for (j = 0; j < io->numberoftriangleattributes; j++) {
          printf("  %.6g", io->triangleattributelist[i *
                                         io->numberoftriangleattributes + j]);
        }
        printf("\n");
      }
      if (reportneighbors) {
        printf("Triangle %4d neighbors:", i);
        for (j = 0; j < 3; j++) {
          printf("  %4d", io->neighborlist[i * 3 + j]);
        }
        printf("\n");
      }
    }
    printf("\n");
  }

  if (reportsegments) {
    for (i = 0; i < io->numberofsegments; i++) {
      printf("Segment %4d points:", i);
      for (j = 0; j < 2; j++) {
        printf("  %4d", io->segmentlist[i * 2 + j]);
      }
      if (markers) {
        printf("   marker %d\n", io->segmentmarkerlist[i]);
      } else {
        printf("\n");
      }
    }
    printf("\n");
  }

  if (reportedges) {
    for (i = 0; i < io->numberofedges; i++) {
      printf("Edge %4d points:", i);
      for (j = 0; j < 2; j++) {
        printf("  %4d", io->edgelist[i * 2 + j]);
      }
      if (reportnorms && (io->edgelist[i * 2 + 1] == -1)) {
        for (j = 0; j < 2; j++) {
          printf("  %.6g", io->normlist[i * 2 + j]);
        }
      }
      if (markers) {
        printf("   marker %d\n", io->edgemarkerlist[i]);
      } else {
        printf("\n");
      }
    }
    printf("\n");
  }
}
