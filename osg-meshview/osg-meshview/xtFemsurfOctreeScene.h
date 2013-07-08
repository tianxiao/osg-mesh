#pragma once
#include "../buildpyramidGeometry/osgcommon.h"

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <algorithm>

#include "Octree.h"
#include "Stopwatch.h"

class txFemSurf;

using namespace brandonpelfrey;
class xtFemsurfOctreeScene
{
public:
	xtFemsurfOctreeScene(void);
	~xtFemsurfOctreeScene(void);

	void LoadFemSurf(/*char *filename*/);

	osg::Geode* CreateScene();

private:
	void BuildOctree();

private:
	void DestroyMem();

private:
	txFemSurf *femSurf;

	Octree *octree;
	OctreePoint *octreePoints;
	int nPnts;
	Vec3 qmin, qmax;

	std::vector<osg::Box *> leafBoxlist;
};

