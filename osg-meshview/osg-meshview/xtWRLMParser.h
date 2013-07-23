#pragma once
#include <vector>
#include "Vec2.h"
#include "Vec3.h"
#include "xtPrimitive.h"



struct xtBMPPicFormat;
struct xtWRLDataS;
class txFemSurf;
class xtWRLMParser
{
public:
	xtWRLMParser(void);
	~xtWRLMParser(void);

	void LoadWRLFile(char *fliename);
	void DumpToFemFile(char *femFile);
	
	txFemSurf *ConverToFem();

	xtWRLDataS *GetData() { return this->data; }

	xtBMPPicFormat *GetPicData() { return this->img; }

private:
	xtWRLDataS *data;
	xtBMPPicFormat *img;

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

