#include "StdAfx.h"
#include "xtTrianglePLSG.h"
#include "common.h"
#include "triangle.h"
//
//xtTrianglePLSG::xtTrianglePLSG(void)
//{
//}


xtTrianglePLSG::~xtTrianglePLSG(void)
{
	free(mIn->pointlist);
	free(mIn->segmentlist);
	free(mIn->segmentmarkerlist);
	
	free(mOut->pointlist);
	free(mOut->pointmarkerlist);
	free(mOut->trianglelist);
	free(mOut->segmentlist);
	free(mOut->segmentmarkerlist);

	delete mIn;
	delete mOut;
}

xtTrianglePLSG::xtTrianglePLSG( 
	std::vector<xtTriPnt2> &verts, std::vector<xtSeg2WithMarker> &segs,
	std::vector<xtTriIndexO> &tris)
{
	mIn = new triangulateio;
	mOut = new triangulateio;

	mIn->numberofpoints = verts.size();
	mIn->numberofpointattributes = 0;
	mIn->pointlist = (REAL *) malloc (mIn->numberofpoints *2 *sizeof(REAL));

	for ( int nodeidx=0; nodeidx<mIn->numberofpoints; ++nodeidx ) {
		mIn->pointlist[nodeidx*2]   = verts[nodeidx].p[0];
		mIn->pointlist[nodeidx*2+1] = verts[nodeidx].p[1];
	}

	mIn->numberofsegments = segs.size();
	mIn->segmentlist = (int*) malloc(mIn->numberofsegments*2*sizeof(int));
	mIn->segmentmarkerlist = (int*) malloc(mIn->numberofsegments*sizeof(int));

	for ( int segidx=0; segidx<mIn->numberofsegments; ++segidx ) {
		mIn->segmentlist[2*segidx]     = segs[segidx].seg[0];
		mIn->segmentlist[2*segidx+1]   = segs[segidx].seg[1];
		mIn->segmentmarkerlist[segidx] = segs[segidx].marker;
	}

	mIn->numberofholes = 0;
	mIn->holelist = (REAL*)NULL;

	mIn->numberofregions = 0;

	mIn->pointmarkerlist = (int*) NULL;

	mOut->pointlist = (REAL*)NULL;
	mOut->pointmarkerlist = (int*) NULL;
	mOut->trianglelist = (int*) NULL;
	mOut->segmentlist = (int*) NULL;
	mOut->segmentmarkerlist = (int*) NULL;

	std::string paras = "pz";

	triangulate(const_cast<char *>(paras.c_str()), mIn, mOut, (struct triangulateio*) NULL);

	xtTriIndexO tri;
	for ( int triidx=0; triidx<mOut->numberoftriangles; ++triidx ) {
		for ( int i=0; i<3; ++i ) {
			tri.idx[i] = mOut->trianglelist[3*triidx+i];
		}
		tris.push_back(tri);
	}

	report(mOut,1, 1, 0, 0, 0, 0);
}


void xtTrianglePLSG::PrintOutInfo()
{
	
}

void xtTrianglePLSG::report(struct triangulateio *io,
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


