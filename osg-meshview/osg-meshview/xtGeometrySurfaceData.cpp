#include "StdAfx.h"
#include "xtGeometrySurfaceData.h"


xtGeometrySurfaceData::xtGeometrySurfaceData(void)
{
}


xtGeometrySurfaceData::~xtGeometrySurfaceData(void)
{
}

static inline void SetMin(xtVector3d &min, xtVector3d &p )
{
	min.x() = min.x()<p.x()?min.x():p.x();
	min.y() = min.y()<p.y()?min.y():p.y();
	min.z() = min.z()<p.z()?min.z():p.z();
}

static inline void SetMax(xtVector3d &max, xtVector3d &p ) 
{
	max.x() = max.x()>p.x()?max.x():p.x();
	max.y() = max.y()>p.y()?max.y():p.y();
	max.z() = max.z()>p.z()?max.z():p.z();
}

void BuildSurfaceBBox(xtGeometrySurfaceDataS *surf, std::vector<xtBBOX> &bboxlist)
{
	assert(surf);
	assert(surf->verts.size());
	xtVector3d min, max;
	
	for ( size_t i=0; i<surf->indices.size(); ++i ) {
		xtIndexTria3 &tria = surf->indices[i];
		
		xtVector3d &pa = surf->verts[tria.a[0]];
		xtVector3d &pb = surf->verts[tria.a[1]];
		xtVector3d &pc = surf->verts[tria.a[2]];
		min = max = pa;
		/*SetMin(min,pa); */SetMin(min,pb); SetMin(min,pc);
		/*SetMax(max,pa); */SetMax(max,pb); SetMax(max,pc);
		xtBBOX bbox = { min, max};
		bboxlist.push_back(bbox);
	}
}
