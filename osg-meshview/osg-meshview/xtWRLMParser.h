#pragma once
#include <vector>
#include "Vec2.h"
#include "Vec3.h"
#include "xtPrimitive.h"

class txFemSurf;
class xtWRLMParser
{
public:
	xtWRLMParser(void);
	~xtWRLMParser(void);

	void LoadWRLFile(char *fliename);
	void DumpToFemFile(char *femFile);
	
	txFemSurf *ConverToFem();

private:
	std::vector<Vec3> mVertices;
	std::vector<xtIndexTria3> mTris;
	std::vector<xtIndexCquad4> mQuads;
	std::vector<Vec2> mTextureCoords;

	bool mIsSeparator;
	bool mIsTexture2;
	bool mStartTextureData;
	bool mIsTextureCoordinate2;
	bool mIsTextureCoordinate2Point;

	bool mIsCoordinate3;
	bool mIsCoordinate3Point;

	bool mIsIndexedFaceSet;
	bool mIsIndexedFaceSetcoordIndex;
	bool mIsIndexedFaceSettextureCoordIndex;
};

