#include "StdAfx.h"
#include "txFemSurf.h"
#include <assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <algorithm>

//#include <gl/GL.h>
//#include "../../freeglut-2.6.0/include/GL/glut.h"

#include <osg/Array>
#include <osg/Geode>
#include <osg/Vec3>
#include <osg/MatrixTransform>
#include <osg/PolygonStipple>
#include <osg/TriangleFunctor>
#include <osg/io_utils>
#include <osg/ComputeBoundsVisitor>
#include <osg/PolygonMode>
#include <osg/LineWidth>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgViewer/Viewer>

#include <osgGA/TrackballManipulator>
#include <osg/Math>
#include <iostream>

txFemSurf::txFemSurf(void)
{
}


txFemSurf::~txFemSurf(void)
{
}

int getline(FILE *fp, char line[], int max)
{
	int nch = 0;
	int c;
	max = max - 1;			/* leave room for '\0' */

	while((c = getc(fp)) != EOF)
	{
		if(c == '\n')
			break;

		if(nch < max)
		{
			line[nch] = c;
			nch = nch + 1;
		}
	}

	if(c == EOF && nch == 0)
		return EOF;

	line[nch] = '\0';
	return nch;
}

bool IsHeadCharsEqual(const char *token, const int tokenlen, const char *str)
{
	for ( int i=0; i<tokenlen; ++i ) {
		if ( str[i] != token[i] ) {
			return false;
		}
	}
	return true;
}

double ConvertToFloat(const char *str,const int begin, const int buffersize=9, const int tokelen=8)
{
	double value;

	char cstr[9];
	strncpy(cstr,&str[begin],tokelen);
	cstr[8]='\0';
	sscanf(cstr,"%lf",&value);

	return value;
}

int ExtractGRID(const char* str, const int len, float &x, float &y, float &z)
{
	char gridtokenstr[20];
	//char gridindex[20];
	int gridIndex;
	char gridxyzstr[50];
	sscanf(str,"%s %d %s",gridtokenstr,&gridIndex,gridxyzstr);
	//printf(gridxyzstr);
	// NOTE!!! This have lost some precision
	x = (float)ConvertToFloat(gridxyzstr,0);
	y = (float)ConvertToFloat(gridxyzstr,8);
	z = (float)ConvertToFloat(gridxyzstr,16);
	//printf("x:\t %f, y:\t %f, z:\t %f\n",x,y,z);
	//map.insert(gridIndex,
	return gridIndex;
}

void ExtractTRIA3(const char* str, const int len, int &a, int &b, int &c)
{
	char ctria3tokestr[20];
	char ctria3indexstr[20];
	char stria3opt[20];
	sscanf(str,"%s %s %s %d %d %d", ctria3tokestr,ctria3indexstr,stria3opt,&a,&b,&c);
	//printf("a:\t %d, b:\t %d, c:\t %d\n",a,b,c);

}

void ExtractCQUAD4(const char *str, const int len, int &a, int &b, int &c, int &d)
{
	char cquad4tokestr[20];
	char cquad4indexstr[20];
	char cquad4opt[20];
	sscanf(str,"%s %s %s %d %d %d %d", cquad4tokestr,cquad4indexstr,cquad4opt,&a,&b,&c,&d);
	//printf("a:\t %d, b:\t %d, c:\t %d\n",a,b,c);
}


void txFemSurf::LoadFemFile(char *file)
{
	std::vector<xtVec3df> tempverts;
	std::vector<int> tempctria3;
	std::vector<int> tempcquad4;


	FILE *fp = fopen(file,"r");
	assert(fp);
	const int BufferSize = 256;
	char linebuff[BufferSize];
	char content[BufferSize] ;
	//fgets(linebuff,BufferSize,fp);
	//sscanf(linebuff,"%s",content);

	char linestr[BufferSize];
	char linetemstr[BufferSize];
	char token[BufferSize];
	size_t len = 0;
	size_t readstate;

	int linenum = 0;
	int numGRID = 0;
	int numCTRIA3 = 0;
	int numCQUAD4 = 0;

	std::map<int, int> gridIndexInnerMap;
	while ( (readstate=getline(fp,linestr,BufferSize))!=EOF) {
		//printf("%d\t:%s\n",linenum,linestr);
		linenum++;
		// parse token
		if ( IsHeadCharsEqual("GRID",4,linestr) ) {
			float x, y, z;
			int gridIndex = ExtractGRID(linestr,readstate,x,y,z);
			xtVec3df vertex(x,y,z);
			tempverts.push_back(vertex);
			gridIndexInnerMap.insert(std::pair<int, int>(gridIndex,numGRID));
			numGRID++;
			continue;
		}

		if ( IsHeadCharsEqual("CTRIA3",6,linestr) ) {
			int a, b, c;
			ExtractTRIA3(linestr,readstate,a,b,c);
			//tempctria3.push_back(a-1);
			//tempctria3.push_back(b-1);
			//tempctria3.push_back(c-1);
			tempctria3.push_back(gridIndexInnerMap[a]);
			tempctria3.push_back(gridIndexInnerMap[b]);
			tempctria3.push_back(gridIndexInnerMap[c]);
			numCTRIA3++;
			continue;
		}

		if ( IsHeadCharsEqual("CQUAD4",6,linestr) ) {
			int a, b, c, d;
			ExtractCQUAD4(linestr,readstate,a,b,c,d);
			//tempcquad4.push_back(a-1);
			//tempcquad4.push_back(b-1);
			//tempcquad4.push_back(c-1);
			//tempcquad4.push_back(d-1);
			tempcquad4.push_back(gridIndexInnerMap[a]);
			tempcquad4.push_back(gridIndexInnerMap[b]);
			tempcquad4.push_back(gridIndexInnerMap[c]);
			tempcquad4.push_back(gridIndexInnerMap[d]);
			numCQUAD4++;
			continue;
		}
	}

	printf("GRID:\t %d, CTRIA3:\t %d, CQUAD4:\t %d\n",numGRID,numCTRIA3,numCQUAD4);

	fclose(fp);

	FillVC3C4(tempverts,tempctria3,tempcquad4);

	RequireNormal();

	BuildRendData();
}

void txFemSurf::FillVC3C4(std::vector<xtVec3df> &tempverts,
	std::vector<int> &tempctria3, std::vector<int> &tempcquad4)
{
	//std::copy(tempverts.begin(), tempverts.end(),this->verts);
	//this->verts.insert(tempverts.begin(),tempverts.end());
	this->verts.reserve(tempverts.size());
	for ( size_t i=0; i<tempverts.size(); ++i ) {
		this->verts.push_back(tempverts[i]);
	}
	//assert(!(1+tempctria3.size())%3);
	//assert(!(1+tempcquad4.size())%4);
	int a,b,c,d;
	this->ctria3index.reserve(tempctria3.size()/3);
	this->cquad4index.reserve(tempcquad4.size()/4);
	for ( size_t i=0; i<tempctria3.size()/3; ++i ) {
		a = tempctria3[i*3];
		b = tempctria3[i*3+1];
		c = tempctria3[i*3+2];
		xtIndexTria3 temptria3(a,b,c);
		ctria3index.push_back(temptria3);
	}

	for ( size_t i=0; i<tempcquad4.size()/4; ++i ) {
		a = tempcquad4[i*4];
		b = tempcquad4[i*4+1];
		c = tempcquad4[i*4+2];
		d = tempcquad4[i*4+3];
		xtIndexCquad4 temcquad4(a,b,c,d);
		cquad4index.push_back(temcquad4);
	}
}

void txFemSurf::RequireNormal() 
{
	//===================================================================
	// Calculate the face normal
	CalculateFaceNormal();

	//===================================================================
	// Initialize the face map, 
	for( size_t i=0; i<this->verts.size(); ++i ) {
		xtNormalFaceList xtNFL;
		normalfacelist.push_back(xtNFL);
	}

	// Push the adjacent face
	for ( size_t i=0; i<ctria3index.size(); ++i ) {
		xtIndexTria3 ctria3 = ctria3index[i];
		for ( int vertId=0; vertId<3; ++vertId ) {
			normalfacelist[ctria3.a[vertId]].facelist.push_back(i);
		}
	}

	for ( size_t i=0; i<cquad4index.size(); ++i ) {
		xtIndexCquad4 cquad4 = cquad4index[i];
		for ( int vertId=0; vertId<4; ++vertId ) {
			normalfacelist[cquad4.a[vertId]].facelist.push_back(i);
		}
	}

	//===================================================================
	// Simpply average the face normal as a weight equal 1
	vertsnormal.reserve(verts.size());
	for ( size_t i=0; i<normalfacelist.size(); ++i ) {
		xtNormalFaceList currFlist = normalfacelist[i];
		xtVec3df normal;
		normal.Zero();
		for ( size_t vertId=0; vertId<currFlist.facelist.size(); ++vertId ) {
			normal = normal + facenormal[currFlist.facelist[vertId]];
		}
		normal.Scale(1./currFlist.facelist.size());
		normal.Normal();
		vertsnormal.push_back(normal);
	}
}

void txFemSurf::CalculateBBox()
{
	xtVec3df min, max;
	for ( size_t i=0; i<verts.size(); ++i ) {
		
	}
}

void txFemSurf::CalculateFaceNormal()
{
	facenormal.reserve(ctria3index.size()+cquad4index.size());

	ctria3normal.reserve(ctria3index.size());
	for ( size_t i=0; i<ctria3index.size(); ++i ) {
		xtIndexTria3 ctria3 = ctria3index[i];
		xtVec3df ba = verts[ctria3.a[1]] - verts[ctria3.a[0]];
		xtVec3df ca = verts[ctria3.a[2]] - verts[ctria3.a[0]];
		xtVec3df normal;
		ba.CrossProduct(ca,normal);
		normal.Normal();
		ctria3normal.push_back(normal);
		facenormal.push_back(normal);
	}
	cquad4normal.reserve(cquad4index.size());
	for ( size_t i=0; i<cquad4index.size(); ++i ) {
		xtIndexCquad4 cquad4 = cquad4index[i];
		for ( int di=0; di<4; di++ ) {
			if ( cquad4.a[di]>verts.size() ) {
				continue;
			}
		}
		xtVec3df ba = verts[cquad4.a[1]] - verts[cquad4.a[0]];
		xtVec3df da = verts[cquad4.a[3]] - verts[cquad4.a[0]];
		xtVec3df normal;
		ba.CrossProduct(da,normal);
		normal.Normal();
		cquad4normal.push_back(normal);
		facenormal.push_back(normal);
	}

}

/*
void txFemSurf::Draw()
{
	float shininess = 15.0f;
	float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
	float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};

	// set specular and shiniess using glMaterial (gold-yellow)
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); // range 0 ~ 128
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);

	// set ambient and diffuse color using glColorMaterial (gold-yellow)
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3fv(diffuseColor);


	double color[3] = { 0.0, 1.0, 0.0 };
	float norm[3];
	float p[3];
//
//  Specify that polygons facing the front are to be filled.
//
  //glPolygonMode ( GL_FRONT, GL_FILL );
//
//  Draw the polygonal elements, in GREEN.
//
  glColor3f ( 0.0, 1.0, 0.0 );

  for ( size_t i=0; i<ctria3index.size(); ++i )
  {
	  xtIndexTria3 currTriIndex = ctria3index[i];
    glBegin ( GL_TRIANGLES );
    for ( int j=0; j<3; ++j)
    {

      glColor3dv ( color );

      norm[0] = vertsnormal[currTriIndex.a[j]].v[0];
      norm[1] = vertsnormal[currTriIndex.a[j]].v[1];
      norm[2] = vertsnormal[currTriIndex.a[j]].v[2];
      glNormal3fv ( norm );

      p[0] = verts[currTriIndex.a[j]].v[0];
      p[1] = verts[currTriIndex.a[j]].v[1];
      p[2] = verts[currTriIndex.a[j]].v[2];
      glVertex3fv ( p );
    }
    glEnd ( );
  }

  glColor3f ( 0.0, 1.0, 0.0 );

  for ( size_t i=0; i<cquad4index.size(); ++i )
  {
	  xtIndexCquad4 currQuadIndex = cquad4index[i];
    glBegin ( GL_QUAD_STRIP );
    for ( int j=0; j<4; ++j)
    {

      glColor3dv ( color );

      norm[0] = vertsnormal[currQuadIndex.a[j]].v[0];
      norm[1] = vertsnormal[currQuadIndex.a[j]].v[1];
      norm[2] = vertsnormal[currQuadIndex.a[j]].v[2];
      glNormal3fv ( norm );

      p[0] = verts[currQuadIndex.a[j]].v[0];
      p[1] = verts[currQuadIndex.a[j]].v[1];
      p[2] = verts[currQuadIndex.a[j]].v[2];
      glVertex3fv ( p );
    }
    glEnd ( );
  }
//
//  Draw lines delimiting the polygonal elements, in RED.
//
  glColor3f ( 1.0, 0.0, 0.0 );

  for ( size_t i=0; i<ctria3index.size(); ++i )
  {
	  xtIndexTria3 currTriIndex = ctria3index[i];
    glBegin ( GL_LINE_LOOP );
    for ( int j=0; j<3; ++j )
    {
      p[0] = verts[currTriIndex.a[j]].v[0];
      p[1] = verts[currTriIndex.a[j]].v[1];
      p[2] = verts[currTriIndex.a[j]].v[2];
      glVertex3fv ( p );
    }
    glEnd ( );
  }
//
//  Draw the nodes in BLUE.
//  Draw them AFTER the lines, so they are more likely to show up!
//
  glColor3f ( 0.0, 0.0, 1.0 );

  for ( size_t i=0; i<verts.size(); ++i )
  {
	  //xtIndexTria3 currTriIndex = ctria3index[i];
    glBegin ( GL_POINTS );
      p[0] = verts[i].v[0];
      p[1] = verts[i].v[1];
      p[2] = verts[i].v[2];
      glVertex3fv ( p );
    glEnd ( );
  }

  //glutSolidCone(3,3,50,50);
  glColor3f ( 1.0, .0, .0 );
  glutWireSphere(500,50,50);
  glColor3f ( .0, 1.0, .0 );
  //glutSolidSphere(500,50,50);
  RenderSurf(true);
//
//  Clear all the buffers.
//
  //glFlush ( );
//
//  Switch between the two buffers for fast animation.
//
  //glutSwapBuffers ( );
}
*/

void txFemSurf::DestroyRenderData()
{
	delete []pVertList;
	delete []pNormList;
	delete []pColorList;
}

void txFemSurf::BuildRendData()
{
	const int numoftrifaces = ctria3index.size() + 2*cquad4index.size();
	nbFaces_ = numoftrifaces;
	pVertList = new float[nbFaces_*3*3];
	pNormList = new float[nbFaces_*3*3];
	pColorList = new float[nbFaces_*4*3];

	int vertIndex = 0;
	int normIndex = 0;
	int colorIndex = 0;

	for(int i=0;i<(int)ctria3index.size();i++)
	{
		xtIndexTria3 currTria3 = ctria3index[i];
		for(int j=0;j<3;j++)
		{
			pVertList[vertIndex++] = verts[currTria3.a[j]].v[0];
			pVertList[vertIndex++] = verts[currTria3.a[j]].v[1];
			pVertList[vertIndex++] = verts[currTria3.a[j]].v[2];

			pNormList[normIndex++] = vertsnormal[currTria3.a[j]].v[0];
			pNormList[normIndex++] = vertsnormal[currTria3.a[j]].v[1];
			pNormList[normIndex++] = vertsnormal[currTria3.a[j]].v[2];

			pColorList[colorIndex++] = 0.0f;
			pColorList[colorIndex++] = 1.0f;
			pColorList[colorIndex++] = 0.0f;
			pColorList[colorIndex++] = 1.0f;
		}
	}

	for(int i=0;i<(int)cquad4index.size();i++)
	{
		xtIndexCquad4 currquad4 = cquad4index[i];
		xtIndexTria3 currTria31(currquad4.a[0],currquad4.a[1],currquad4.a[2]);
		xtIndexTria3 currTria32(currquad4.a[0],currquad4.a[2],currquad4.a[3]);

		for(int j=0;j<3;j++)
		{
			pVertList[vertIndex++] = verts[currTria31.a[j]].v[0];
			pVertList[vertIndex++] = verts[currTria31.a[j]].v[1];
			pVertList[vertIndex++] = verts[currTria31.a[j]].v[2];

			pNormList[normIndex++] = vertsnormal[currTria31.a[j]].v[0];
			pNormList[normIndex++] = vertsnormal[currTria31.a[j]].v[1];
			pNormList[normIndex++] = vertsnormal[currTria31.a[j]].v[2];

			pColorList[colorIndex++] = 0.0f;
			pColorList[colorIndex++] = 1.0f;
			pColorList[colorIndex++] = 0.0f;
			pColorList[colorIndex++] = 1.0f;
		}

		for(int j=0;j<3;j++)
		{
			pVertList[vertIndex++] = verts[currTria32.a[j]].v[0];
			pVertList[vertIndex++] = verts[currTria32.a[j]].v[1];
			pVertList[vertIndex++] = verts[currTria32.a[j]].v[2];

			pNormList[normIndex++] = vertsnormal[currTria32.a[j]].v[0];
			pNormList[normIndex++] = vertsnormal[currTria32.a[j]].v[1];
			pNormList[normIndex++] = vertsnormal[currTria32.a[j]].v[2];

			pColorList[colorIndex++] = 0.0f;
			pColorList[colorIndex++] = 1.0f;
			pColorList[colorIndex++] = 0.0f;
			pColorList[colorIndex++] = 1.0f;
		}
	}

}

/*
void txFemSurf::RenderSurf(bool addWireframe, bool defaultt )
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	if (defaultt) {
		
		glVertexPointer(3,GL_FLOAT, 0, pVertList);
		
		glNormalPointer(GL_FLOAT, 0, pNormList);
		glColorPointer(4,GL_FLOAT, 0, pColorList);
		
		glDrawArrays(GL_TRIANGLES, 0, nbFaces_*3);
	}



	if(addWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glVertexPointer(3,GL_FLOAT, 0, pVertList);
		glNormalPointer(GL_FLOAT, 0, pNormList);

		glDisableClientState(GL_COLOR_ARRAY);

		glLineWidth(2.0f);

		glColor4f(0.2f, 0.2f, 0.6f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, nbFaces_*3);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
	}

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void txFemSurf::RenderSurf2()
{
	glColor3f ( 1.0, .0, .0 );
	glutWireSphere(500,50,50);


	for ( size_t i=0; i<cquad4index.size(); ++i ) {
		xtIndexCquad4 cquad4 = cquad4index[i];
		glBegin( GL_POLYGON );
		for ( int j=0; j<4; ++j ) {
			xtVec3df normal = vertsnormal[cquad4.a[j]];
			glNormal3f(normal.v[0],normal.v[1],normal.v[2]);
			xtVec3df vert = verts[cquad4.a[j]];
			glVertex3f(vert.v[0],vert.v[1],vert.v[2]);
		}
		glEnd();
	}
}
*/





class BoundingBoxCallback : public osg::NodeCallback
{
public:
	virtual void operator() ( osg::Node* Node, osg::NodeVisitor *nv ) 
	{
		osg::BoundingBox bb;
		for ( unsigned int i=0; i<m_NodesToCompute.size(); ++i ) {
			osg::Node *node = m_NodesToCompute[i];
			osg::ComputeBoundsVisitor cbbv;
			node->accept( cbbv );

			osg::BoundingBox localBB = cbbv.getBoundingBox();
			osg::Matrix localToWorld = osg::computeLocalToWorld(node->getParent(0)->getParentalNodePaths()[0]);

			for ( unsigned int i=0; i<8; ++i ) {
				bb.expandBy( localBB.corner(i)*localToWorld );
			}
		}
	}

	osg::NodePath m_NodesToCompute;

};




osg::Geode* txFemSurf::CreateMesh()
{

    osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
    shared_colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));

	// same trick for shared normal.
	osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
	shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));

	osg::Vec3Array* vertices = new osg::Vec3Array;
	//vertices->push_back(osg::Vec3(-1.02168, -2.15188e-09, 0.885735));

	osg::Vec3Array* normals = new osg::Vec3Array;
	//normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));


	for ( size_t i=0; i<cquad4index.size(); ++i ) {
		xtIndexCquad4 cquad4 = cquad4index[i];
		//glBegin( GL_POLYGON );
		for ( int j=0; j<4; ++j ) {
			xtVec3df normal = vertsnormal[cquad4.a[j]];
			//glNormal3f(normal.v[0],normal.v[1],normal.v[2]);
			normals->push_back(osg::Vec3(normal.v[0],normal.v[1],normal.v[2]));
			xtVec3df vert = verts[cquad4.a[j]];
			//glVertex3f(vert.v[0],vert.v[1],vert.v[2]);
			vertices->push_back(osg::Vec3(vert.v[0],vert.v[1],vert.v[2]));
		}
		//glEnd();
	}


	osg::Geode* geode = new osg::Geode();


	{
		osg::ref_ptr<BoundingBoxCallback> bbcb = new BoundingBoxCallback;
		

		osg::Geometry* polyGeom = new osg::Geometry();
		//osg::Geometry *polyGeom = new osg::Geometry();

		polyGeom->setVertexArray(vertices);

		polyGeom->setColorArray(shared_colors.get());
		polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

		//polyGeom->setNormalArray(shared_normals.get());
		//polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
		polyGeom->setNormalArray( normals );
		polyGeom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );


		polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,vertices->size()));


		geode->addDrawable(polyGeom);
	}

	{

		osg::Vec3Array* verticeslocal = new osg::Vec3Array;
		//vertices->push_back(osg::Vec3(-1.02168, -2.15188e-09, 0.885735));

		osg::Vec3Array* normalslocal = new osg::Vec3Array;
		//normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));


		for ( size_t i=0; i<cquad4index.size(); ++i ) {
			xtIndexCquad4 cquad4 = cquad4index[i];
			for ( int j=0; j<3; ++j ) {
				xtVec3df normal0 = vertsnormal[cquad4.a[j]];
				normalslocal->push_back(osg::Vec3(normal0.v[0],normal0.v[1],normal0.v[2]));
				xtVec3df normal1 = vertsnormal[cquad4.a[j+1]];
				normalslocal->push_back(osg::Vec3(normal1.v[0],normal1.v[1],normal1.v[2]));
				xtVec3df vert0 = verts[cquad4.a[j]];
				verticeslocal->push_back(osg::Vec3(vert0.v[0],vert0.v[1],vert0.v[2]));
				xtVec3df vert1 = verts[cquad4.a[j+1]];
				verticeslocal->push_back(osg::Vec3(vert1.v[0],vert1.v[1],vert1.v[2]));
			}
			xtVec3df normal1 = vertsnormal[cquad4.a[3]];
			normalslocal->push_back(osg::Vec3(normal1.v[0],normal1.v[1],normal1.v[2]));
			xtVec3df vert1 = verts[cquad4.a[3]];
			verticeslocal->push_back(osg::Vec3(vert1.v[0],vert1.v[1],vert1.v[2]));
			xtVec3df normal0 = vertsnormal[cquad4.a[0]];
			normalslocal->push_back(osg::Vec3(normal0.v[0],normal0.v[1],normal0.v[2]));
			xtVec3df vert0 = verts[cquad4.a[0]];
			verticeslocal->push_back(osg::Vec3(vert0.v[0],vert0.v[1],vert0.v[2]));
			
		}

		osg::Geometry* linesGeom = new osg::Geometry();

		// pass the created vertex array to the points geometry object.
        linesGeom->setVertexArray(verticeslocal);

		// set the line width
		osg::StateSet *stateset = new osg::StateSet;
		osg::LineWidth *linewidth = new osg::LineWidth;
		linewidth->setWidth(4.0f);
		stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		linesGeom->setStateSet(stateset);
        
        // set the colors as before, plus using the above
        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back(osg::Vec4(0.0f,1.0f,0.0f,1.0f));
        linesGeom->setColorArray(colors);
        linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
        

        // set the normal in the same way color.
        //osg::Vec3Array* normals = new osg::Vec3Array;
        //normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
        linesGeom->setNormalArray(normalslocal);
        linesGeom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
        

        // This time we simply use primitive, and hardwire the number of coords to use 
        // since we know up front,
		linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,vertices->size()));
        
        
        // add the points geometry to the geode.
        geode->addDrawable(linesGeom);

	}

	return geode;
}