#include "StdAfx.h"
#include "xtGTSParser.h"
#include "xtOctreeDisplayUtil.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include "xtGeometrySurfaceData.h"
#include <set>
#include <algorithm>




xtGTSParser::xtGTSParser(void)
{
	this->mSurface = new xtGeometrySurfaceDataS;
	InitialLizeTranRot();
}


xtGTSParser::~xtGTSParser(void)
{
	delete this->mSurface;
	this->mSurface = NULL;
}

void xtGTSParser::InitialLizeTranRot() 
{
	mSurface->tran = xtVector3d::Zero();
	mSurface->rot.Zero();
	mSurface->rot << 
		1.0,0,0,
		0,1.0,0,
		0,0,1.0;
}

void xtGTSParser::SetTranRot(xtVector3d *tran, xtMatrix3d *rot) {
	if ( tran ) {
		mSurface->tran = *tran;
	}
	if ( rot ) {
		mSurface->rot = *rot;
	} 
}

static inline double parseDouble(const char *&token)
{
	token += strspn(token, " \t");
	double f = (double)atof(token);
	token += strcspn(token, " \t\r");
	return f;
}

static inline int parseInt(const char *&token)
{
	token += strspn(token, " \t");
	int index = (int) atoi(token);
	token += strcspn(token, " \t\r");
	return index;
}

static inline void parseInt3( int &a, int &b, int &c, const char *&token) 
{
	a = parseInt(token);
	b = parseInt(token);
	c = parseInt(token);
}

static inline void parseInt2( int &a, int &b, const char *&token) 
{
	a = parseInt(token);
	b = parseInt(token);
}

static inline void parseDouble3( double &x, double &y, double &z, const char *&token )
{
	x = parseDouble(token);
	y = parseDouble(token);
	z = parseDouble(token);
}



void xtGTSParser::LoadGTS(const char *filename)
{
	std::stringstream err;

	std::ifstream ifs(filename);
	if ( !ifs ) {
		err << "Cannot open file [  " << filename << std::endl;
		return ;
	}

	int nv, ne, nf; // number of points, edges, faces
	nv=ne=nf=0;
	bool start = false;

	std::vector<xtEdge> tempEdges;

	const int maxchars = 8192; // 8M
	std::vector<char> buf(maxchars);
	while (ifs.peek() != -1 ) {
		ifs.getline( &buf[0], maxchars );

		std::string linebuf( &buf[0] );
		/****
		// Trim newline '\r\n' or '\r'
		if ( linebuf.size()>0 ) {
			if ( linebuf[linebuf.size()-1] == '\n' ) {
				linebuf.erase(linebuf.size()-1);
			}
		}
		if ( linebuf.size()>0 ) {
			if ( linebuf[linebuf.size()-1] == '\n' ) {
				linebuf.erase(linebuf.size()-1);
			}
		}

		if ( linebuf.empty() ) {
			continue;
		}

		const char *token = linebuf.c_str();
		token += strspn(token, " \t" );

		assert(token);
		if ( token[0] == '\0' ) continue;
		if ( token[0] == '#' ) continue;

		//if ( token[0] 
		****/

		const char *token = linebuf.c_str();
		token += strspn(token, " \t");

		if ( nv==0 && ne==0 && nf==0 ) {
			parseInt3(nv, ne, nf, token);
			mSurface->verts.reserve(nv);
			mSurface->indices.reserve(nf);
			tempEdges.reserve(ne);
			start = true;
			continue;
		}

		if ( nv>0 ) {
			double x, y, z;
			parseDouble3(x,y,z,token);
			xtVector3d point(x,y,z);
			mSurface->verts.push_back(point);
			nv--;
			continue;
		}

		if ( nv==0 && ne>0 ) {
			// skip the edge
			xtEdge edge;
			int a, b;
			parseInt2(a,b,token);
			edge.a = a-1;
			edge.b = b-1;
			tempEdges.push_back(edge);
			ne--;
			continue;
		}

		if ( nv==0 && ne==0 && nf>0 ) {
			int a, b, c;
			parseInt3(a,b,c,token);
			std::set<int> intset;

			xtIndexTria3 tria;
			xtEdge edge0, edge1, edge2;
			edge0 = tempEdges[a-1];
			edge1 = tempEdges[b-1];
			edge2 = tempEdges[c-1];

			if ( edge0.a==edge1.a ) {
				tria.a[0] = edge0.b;
				tria.a[1] = edge0.a;
				tria.a[2] = edge1.b;
			} else if ( edge0.a==edge1.b ){
				tria.a[0] = edge0.b;
				tria.a[1] = edge0.a;
				tria.a[2] = edge1.a;
			} else if ( edge0.b==edge1.a ) {
				tria.a[0] = edge0.a;
				tria.a[1] = edge0.b;
				tria.a[2] = edge1.b;
			} else if ( edge0.b==edge1.b ) {
				tria.a[0] = edge0.a;
				tria.a[1] = edge0.b;
				tria.a[2] = edge1.a;
			} else {
				assert(false);
			}


			/****
			if ( edge0.b==edge1.a ) {
				tria.a[0] = edge0.a;
				tria.a[1] = edge1.a;
				tria.a[2] = edge2.a;
			} else if ( edge0.b==edge2.a ) {
				tria.a[0] = edge0.a;
				tria.a[1] = edge2.a;
				tria.a[2] = edge1.a;
			} else {
				assert(false);
			}
			****/

			/****
			intset.insert(tempEdges[a-1].a);intset.insert(tempEdges[a-1].b);
			intset.insert(tempEdges[a-1].a);intset.insert(tempEdges[a-1].b);

			intset.insert(tempEdges[b-1].a);intset.insert(tempEdges[b-1].b);
			intset.insert(tempEdges[b-1].a);intset.insert(tempEdges[b-1].b);

			intset.insert(tempEdges[c-1].a);intset.insert(tempEdges[c-1].b);
			intset.insert(tempEdges[c-1].a);intset.insert(tempEdges[c-1].b);

			assert(intset.size()==3);
			
			std::vector<int> setint;//
			setint.reserve(3);
			for ( std::set<int>::const_iterator cit=intset.begin();
				cit!=intset.end(); cit++) {
					setint.push_back(*cit);
			}
			

			xtIndexTria3 tria(setint[0],setint[1],setint[2]);
			****/

			mSurface->indices.push_back(tria);
			nf--;
			continue;
		} 

	}


}


osg::Geode *xtGTSParser::CreateScene() {
	
	return xtOctreeDisplayUtility::DisplayxtGeometrySurfaceDataS(mSurface);
}

osg::Node *xtGTSParser::CreateSceneWithTranRot()
{
	return xtOctreeDisplayUtility::DisplayxtGeometrySurfaceDataWithTranRot(mSurface);
}


