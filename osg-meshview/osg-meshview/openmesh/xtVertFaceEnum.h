#pragma once

// RED means the two model overlapping area
// GREEN means the gouge free area.

enum xtPntOverlapState
{
	PNT_UNDER,
	PNT_ABOVE,
	PNT_UNTAGGED,
};

enum xtIntersectionPnt
{
	UN_TAGEDPNT,
	XT_BOUNDARY_SPLIT_PNT,
	GREENPNT,
	REDPNT,
};

enum xtIntersectFace
{
	UN_TAGEDISTFACE,
	XT_INTERSECT_FACE,
	RED_FACE,
	GREEN_FACE,
};

enum xtHalfEdgeMarker
{
	ON_BOUNDARY_VERTEX,
	ON_BOUNDARYO,
	UN_TAGGED,
};

enum xtEdgeMarker
{
	IST_EDGE,
	UN_TAGGED_EDGE,
};

/**
* Red, Green didn't have realy meaning. 
* whether the red is outside of the boolean or not
* it will be determined in another pass
*/
enum xtOPFaceMarker
{
	RED,
	GREEN,
	SPLIT_PNT,
	UN_TAGGEDFACE,
};