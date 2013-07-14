#pragma once
#include "./tinyobjloader-master/tiny_obj_loader.h"
class xtTinyIMPL
{
public:
	xtTinyIMPL(void);
	~xtTinyIMPL(void);
};

using namespace tinyobj;
struct xtObjShapeImpl
{
	std::vector<shape_t> mShapes;
};

