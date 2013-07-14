#pragma once
class xtTriangleBox3Overlap
{
public:
	xtTriangleBox3Overlap(void);
	~xtTriangleBox3Overlap(void);
};

int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3]);

