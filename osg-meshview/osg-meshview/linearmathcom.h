#pragma once
#include <Eigen/Dense>

typedef Eigen::Matrix<double, 3, 1> xtVector3d;
typedef Eigen::Matrix<float, 3, 1> xtVector3f;
typedef Eigen::Matrix<double, 3, 3> xtMatrix3d;
typedef Eigen::Matrix<double, 4, 4> xtMatrix4d;

inline void SetMatrix4FromTranRot(xtMatrix4d &m, xtVector3d &tran, xtMatrix3d &rot)
{
	//m = rot;
	for ( int i=0; i<3; ++i ) 
		for ( int j=0; j<3; ++j )
			m(i,j) = rot(i,j);

	for ( int i=0; i<3; i++ ) {
		m(3,i) = 0.0;
		m(i,3) = tran(i);
	}
	m(3,3) = 1.0;
}

inline void AssignM3ToM4(xtMatrix3d &m3, xtMatrix4d &m4)
{
	for ( int i=0; i<3; ++i ) 
		for ( int j=0; j<3; ++j )
			m4(i,j) = m3(i,j);

	for ( int i=0; i<3; ++i ) {
		m4(3,i) = 0.0;
		m4(i,3) = 0.0;
	}
	m4(3,3) = 1.0;
}

inline void AssignM4ToM3(xtMatrix4d &m4, xtMatrix3d &m3)
{
	for ( int i=0; i<3; ++i ) 
		for ( int j=0; j<3; ++j ) 
			m3(i,j) = m4(i,j);
}

inline void AssignColM4ToV3(xtMatrix4d &m4, xtVector3d &v3, int col)
{
	for ( int i=0; i<3; ++i ) {
		v3(i) = m4(i,col);
	}
}