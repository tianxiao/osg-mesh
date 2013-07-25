#pragma once
#include "linearmathcom.h"

namespace osg
{
	class Group;
};

class xtGTSParser;
struct xtGeometrySurfaceDataS;
class xtCollisionEngine;
class xtSplitBuilder;
class xtInterferecenTest
{
public:
	xtInterferecenTest(void);
	~xtInterferecenTest(void);

	void LoadSurf(const char *fileI, const char *fileJ);
	void SetTranRot(xtVector3d *tranI, xtMatrix3d *rotI, xtVector3d *tranJ, xtMatrix3d *rotJ);

	void SetUpScene();
	int RunScene();


private:
	xtGeometrySurfaceDataS *mSurfI;
	xtGeometrySurfaceDataS *mSurfJ;
	xtGTSParser *mGtsparserI, *mGtsparserJ;

	// Broad & mediate phase
	xtCollisionEngine *mCE;
	// Detail split pahse
	xtSplitBuilder *mSB;

	osg::Group *sceneRoot;
};

