#pragma once
#include "osgcommon.h"

class xtCreateHUD
{
public:
	xtCreateHUD(void);
	~xtCreateHUD(void);
};

osg::Node *createHUD(osgText::Text *updateText);

