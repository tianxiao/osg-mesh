#pragma once
#include "Vectors.h"
#include "linearmathcom.h"

class xtRayTriOverlay
{
public:
	xtRayTriOverlay(void);
	~xtRayTriOverlay(void);
};

 bool IntersectTriangle(const Vector3& orig, const Vector3& dir,
     Vector3& v0, Vector3& v1, Vector3& v2,
     float* t, float* u, float* v);


 // Translate the above to a template version
 template<typename xtTType>
 bool IntersectTriangleTemplate( const Eigen::Matrix<xtTType,3,1> &orig, const Eigen::Matrix<xtTType,3,1> &dir,
	 Eigen::Matrix<xtTType,3,1> &v0, Eigen::Matrix<xtTType,3,1> &v1, Eigen::Matrix<xtTType,3,1> &v2,
	 xtTType *t, xtTType *u, xtTType *v)
 {
     // E1
     //Vector3 E1 = v1 - v0;
	 Eigen::Matrix<xtTType,3,1> E1 = v1 - v0;
 
     // E2
     //Vector3 E2 = v2 - v0;
	 Eigen::Matrix<xtTType,3,1> E2 = v2 - v0;

     // P
	 //Vector3 P = dir.cross(E2);
	 Eigen::Matrix<xtTType,3,1> P = dir.cross(E2);
 
     // determinant
	 //float det = E1.dot(P);
	 xtTType det = E1.dot(P);
 
     // keep det > 0, modify T accordingly
     //Vector3 T;
	 Eigen::Matrix<xtTType,3,1> T;
     if( det >0 )
     {
         T = orig - v0;
     }
     else
     {
         T = v0 - orig;
         det = -det;
     }
 
     // If determinant is near zero, ray lies in plane of triangle
     if( det < 0.0001f )
         return false;
 
     // Calculate u and make sure u <= 1
	 *u = T.dot(P);
     if( *u < 0.0f || *u > det )
         return false;
 
     // Q
	 //Vector3 Q = T.cross(E1);
	 Eigen::Matrix<xtTType,3,1> Q = T.cross(E1);
 
     // Calculate v and make sure u + v <= 1
	 *v = dir.dot(Q);
     if( *v < 0.0f || *u + *v > det )
         return false;
 
     // Calculate t, scale parameters, ray intersects triangle
	 *t = E2.dot(Q);
 
     xtTType fInvDet = 1.0f / det;
     *t *= fInvDet;
     *u *= fInvDet;
     *v *= fInvDet;
 
     return true;
 }

