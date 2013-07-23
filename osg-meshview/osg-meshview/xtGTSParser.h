#pragma once
#include "linearmathcom.h"
/**
* Parse the gts data
*/
namespace osg
{
	class Geode;
	class Node;
};
struct xtGeometrySurfaceDataS;
class xtGTSParser
{
public:
	xtGTSParser(void);
	~xtGTSParser(void);

	void LoadGTS(const char *filename);
	void SetTranRot(xtVector3d *tran, xtMatrix3d *rot) ;

	osg::Geode *CreateScene();
	osg::Node *CreateSceneWithTranRot();


	xtGeometrySurfaceDataS *GetGTSSurface() { return this->mSurface; };

private:
	void InitialLizeTranRot() ;

private:
	xtGeometrySurfaceDataS * mSurface;

};

