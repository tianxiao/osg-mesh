#pragma once
#include "linearmathcom.h"
#include "xtPrimitive.h"
#include <vector>

namespace osg
{
	class Group;
};

bool TriTriCollide( xtTriangle *tri0, xtTriangle *tri1 ) ;

class xtTriTriCollideScene
{
public:
	xtTriTriCollideScene(void);
	~xtTriTriCollideScene(void);

	void Initial() ;

	void SetUpScene();

	int RunRender();

private:
	xtTriangle mTri0;
	xtTriangle mTri1;
	xtTriangle mTri2;

	osg::Group *sceneRoot;

};

