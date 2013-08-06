#pragma once

struct xtOpenMeshW;
struct xtGeometrySurfaceDataS;
class xtAbosoluteVertexPool;
class xtCollisionEntity ;

void xtBuildRawMesh(xtGeometrySurfaceDataS *surf, xtOpenMeshW &data);

class xtCPMesh
{
public:
	xtCPMesh(void);
	~xtCPMesh(void);

	void LoadRawMesh(xtGeometrySurfaceDataS *surf);

	void LoadRawData(xtCollisionEntity *ce, xtAbosoluteVertexPool *abpool);

private:
	void BuildRawMesh();
	void Remesh();
	// for debug
	void DumpMeshOff();

private:
	xtGeometrySurfaceDataS *mSurf;
	xtAbosoluteVertexPool *mAbpool;
	xtCollisionEntity *mCE;
	xtOpenMeshW *mData;
};

