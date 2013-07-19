#include "StdAfx.h"
#include "xtWRLMParser.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <assert.h>
#include "xtDebutUtil.h"
#include "txFemSurf.h"
#include "xtVec3d.h"

xtWRLMParser::xtWRLMParser(void)
{
}


xtWRLMParser::~xtWRLMParser(void)
{
}

static inline bool isSpace(const char c) {
  return (c == ' ') || (c == '\t');
}

static inline std::string parseString(const char*& token)
{
  std::string s;
  int b = strspn(token, " \t");
  int e = strcspn(token, " \t\r");
  s = std::string(&token[b], &token[e]);

  token += (e - b);
  return s;
}

static inline int parseInt( const char *&token ) {
	token += strspn(token, " \t");
	int temp = (int) atoi(token);
	token += strcspn(token, " \t\r");
	return temp;
}

static inline void parseInt2( int &a, int &b, const char *&token )
{
	a = parseInt(token);
	b = parseInt(token);
}

static inline float parseFloat(const char*& token)
{
  token += strspn(token, " \t");
  float f = (float)atof(token);
  token += strcspn(token, " \t\r");
  return f;
}

static inline void parseFloat3(
  float& x, float& y, float& z,
  const char*& token)
{
  x = parseFloat(token);
  y = parseFloat(token);
  z = parseFloat(token);
}

static inline int parseIntWC(const char *& token)
{
	token += strcspn(token, "-0123456789");
	int temp = (int) atoi(token);
	token += strcspn(token, ", \t\r");
	return temp;
}

static inline void parseIntWC4( int &a, int &b, int &c, int &d, const char *& token) 
{
	a = parseIntWC(token);
	b = parseIntWC(token);
	c = parseIntWC(token);
	d = parseIntWC(token);
}

static inline void parseIntWC5( int &a, int &b, int &c, int &d, int &e, const char *& token) 
{
	a = parseIntWC(token);
	b = parseIntWC(token);
	c = parseIntWC(token);
	d = parseIntWC(token);
	e = parseIntWC(token);
}


void xtWRLMParser::LoadWRLFile(char *filename)
{
	//FILE *fp = fopen(filename,"r");
	//fclose(fp);

	std::stringstream err;
	
	std::ifstream ifs(filename);
	if ( !ifs ) {
		err << "Cannot open file[" << filename << std::endl;
		return ;
	}

	int maxchars = 8192;
	std::vector<char> buf(maxchars);

	this->mIsSeparator = false;
	this->mIsTexture2 = false;
	this->mStartTextureData = false;
	this->mIsTextureCoordinate2 = false;
	this->mIsTextureCoordinate2Point = false;
	this->mIsCoordinate3 = false;
	this->mIsIndexedFaceSet = false;
	this->mIsIndexedFaceSetcoordIndex = false;

	while( ifs.peek() != -1 ) {
		ifs.getline(&buf[0],maxchars);
		std::string linebuf(&buf[0]);

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

		// Skip if empty line
		if ( linebuf.empty() ) {
			continue;
		}

		// Skip leading space.
		const char *token = linebuf.c_str();
		token += strspn(token, " \t");

		assert(token);
		if ( token[0]=='\0' ) continue; // empty line
		if ( token[0]=='#' ) continue; // comment line

		// Separator
		if ( (0==strncmp(token, "Separator",9))&& isSpace(token[9]) ) {
			token += 10;
			//std::string str = parseString(token);
			token += strspn(token, " \t");
			if ( token[0] == '{' ) {
				this->mIsSeparator = true;
				continue;
			} else {
				assert(false);
			}
		}

		if ( (0==strncmp(token, "Texture2", 8)) && isSpace(token[8])) {
			token += 9;
			token += strspn(token, " \t" );
			if ( token[0] == '{' ) {
				this->mIsTexture2 = true;
				continue;
			} else {
				assert(false);
			}
		}

		if ( (0==strncmp(token, "image", 5)) && isSpace(token[5])) {
			if(mIsSeparator&&mIsTexture2) {
				token += 6;
				int width, height;
				parseInt2(width, height, token);
				this->mStartTextureData = true;
				continue;
			} else {
				assert(false);
			}
		}

		if ( '}' == token[0] ) {
			if (this->mStartTextureData ) {
				this->mStartTextureData = false;
				continue;
			}
			if ( this->mIsTextureCoordinate2 ) {
				this->mIsTextureCoordinate2 = false;
				continue;
			}
			if ( this->mIsCoordinate3 ) {
				this->mIsCoordinate3 = false;
				continue;
			}
			if ( this->mIsIndexedFaceSet ) {
				this->mIsIndexedFaceSet = false;
				continue;
			}
		}

		if ( ']' == token[0] ) {
			if ( this->mIsTextureCoordinate2 && this->mIsTextureCoordinate2Point ) {
				this->mIsTextureCoordinate2Point = false;
				continue;
			}
			if ( this->mIsCoordinate3 && this->mIsCoordinate3Point ) {
				this->mIsCoordinate3Point = false;
				continue;
			}
			if ( this->mIsIndexedFaceSet && this->mIsIndexedFaceSetcoordIndex ) {
				this->mIsIndexedFaceSetcoordIndex = false;
				continue;
			}
			if ( this->mIsIndexedFaceSettextureCoordIndex) {
				this->mIsIndexedFaceSettextureCoordIndex = false;
				continue;
			}
			if ( this->mIsSeparator ) {
				this->mIsSeparator = false;
			}
		}

		if ( this->mStartTextureData ) {
			// parse texture data
			continue;
		}

		if ( (0==strncmp(token,"TextureCoordinate2", 18)) && isSpace(token[18]) ) {
			token += 19;
			token += strspn(token, " \t" );
			if ( token[0] == '{' ) {
				this->mIsTextureCoordinate2 = true;
				continue;
			} else {
				assert(false);
			}
		}

		// this point token has been used both for the texture and 
		// the coordinate3 should be pay special attention!
		if ( (0==strncmp(token, "point", 5)) && isSpace(token[5])) {
			token += 6;
			token += strspn(token, " \t");
			if ( token[0] == '[' ) {
				if ( this->mIsSeparator&&this->mIsTexture2 &&this->mIsTextureCoordinate2Point ) {
					this->mIsTextureCoordinate2Point = true;
					continue;
				}
				if ( this->mIsCoordinate3 ) {
					this->mIsCoordinate3Point = true;
					continue;
				}
			} else {
				XTASSERT(false,"Format error! Point should be followed by >>[<<");
			}

		}

		if ( this->mIsTextureCoordinate2 && this->mIsTextureCoordinate2Point ) {
			// parse texture coordinate
			continue;
		}

		if ( (0==strncmp(token,"Coordinate3",11)) && isSpace(token[11]) ) {
			token += 12;
			token += strspn(token," \t");
			if ( '{' == token[0] ) {
				this->mIsCoordinate3 =true;
				continue;
			} else {
				//assert(false);
				XTASSERT(false,"Format error! >>{<< should followed by Coordinate3");
			}
		}

		if ( this->mIsCoordinate3 && this->mIsCoordinate3Point ) {
			// parse coordinate3
			float x, y, z;
			parseFloat3(x,y,z,token);
			Vec3 v(x,y,z);
			this->mVertices.push_back(v);
			continue;
		}

		if ( (0==strncmp(token,"IndexedFaceSet",14)) && isSpace(token[14]) ) {
			token += 15;
			token += strspn(token," \t");
			if ( token[0]=='{' ) {
				this->mIsIndexedFaceSet = true;
				continue;
			} else {
				XTASSERT(false,"Format error! >>{<< should followed by IndexedFaceSet");
			}
		}

		if ( (0==strncmp(token,"coordIndex", 10)) && isSpace(token[10]) ) {
			token += 11;
			token += strspn(token, " \t");
			if ( token[0] == '[' && this->mIsIndexedFaceSet) {
				this->mIsIndexedFaceSetcoordIndex = true;
				continue;
			} else {
				XTASSERT(false,"Format error! >>[<< should followed by coordIndex");
			}
		}

		// parse face
		if ( this->mIsIndexedFaceSet && this->mIsIndexedFaceSetcoordIndex ) {
			int intnum = 0; 
			const char* tokentem = token;
			while( *tokentem!='\0' ) {
				if ( *tokentem==',' ) {
					intnum++;
				} 
				tokentem++;
			}
			if ( intnum == 4 ) {
				int a, b, c, d;
				parseIntWC4(a,b,c,d, token);
				xtIndexTria3 tria3(a,b,c);
				this->mTris.push_back(tria3);
				continue;
			} else if ( intnum == 5 ) {
				int a,b,c,d,e;
				parseIntWC5(a,b,c,d,e,token);
				xtIndexCquad4 quad4(a,b,c,d);
				this->mQuads.push_back(quad4);
				continue;
			}
			
		}

		if ( (0==strncmp(token,"textureCoordIndex",17))&&isSpace(token[17]) ) {
			token += 18;
			token += strspn(token," \t");
			if ( token[0]=='[' ) {
				this->mIsIndexedFaceSettextureCoordIndex = false;
				continue;
			} else {
				XTASSERT(false, "Format error!");
			}
		}

		if ( this->mIsIndexedFaceSettextureCoordIndex ) {
			// parse 
			continue;
		}
	}
}

void xtWRLMParser::DumpToFemFile(char *femFile)
{
	txFemSurf *temp = ConverToFem();
	temp->DumpToFem(femFile);
	delete temp;
}

txFemSurf *xtWRLMParser::ConverToFem()
{
	txFemSurf *tempFemSurf = new txFemSurf;
	std::vector<xtVec3df> verts;
	verts.reserve(this->mVertices.size());
	for ( size_t i=0; i<mVertices.size(); ++i ) {
		xtVec3df v(mVertices[i].x, mVertices[i].y, mVertices[i].z);
		verts.push_back(v);
	}
	tempFemSurf->LoadFemFile(verts, this->mTris, this->mQuads);

	return tempFemSurf;
}
