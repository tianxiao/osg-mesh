#pragma once
#include <vector>
#include "../buildpyramidGeometry/osgcommon.h"



struct xtObjShapeImpl;

class xtObjScene
{
	friend class xtObjOctreeScene;
public:
	xtObjScene(void);
	~xtObjScene(void);

	bool LoadObj(const char *basepath, const char *filename);

	osg::Geode *CreateObjScene();

private:
	

private:
	osg::Geometry *CreatObjScene(void *shape);

private:
	//std::vector<tinyobj::shape_t *> mRawObjShapes;
	xtObjShapeImpl *mRawObjData;
	
};

