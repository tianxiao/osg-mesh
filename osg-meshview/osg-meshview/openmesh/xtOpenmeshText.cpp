#include "StdAfx.h"
#include "xtOpenmeshText.h"
#include "xtOpenmeshHeader.h"

xtOpenmeshText::xtOpenmeshText(void)
{
}


xtOpenmeshText::~xtOpenmeshText(void)
{
}

void xtOpenmeshText::TestOppositeFaceHandel()
{
	txOPMesh mesh;
	txOPMesh::Point p[4];
	p[0][0] = 0.0;
	p[0][1] = 0.0;
	p[1][0] = 1.0;
	p[1][1] = 0.0;
	p[2][0] = 1.0;
	p[2][1] = 1.0;
	p[3][0] = 0.0;
	p[3][1] = 1.0;
	for ( int i=0; i<4; ++i ) {
		p[i][2] = 0.0;
	}
	txOPMesh::VertexHandle vh[4];
	for ( int i=0; i<4; ++i ) {
		vh[i] = mesh.add_vertex( p[i] );
	}

	std::vector<txOPMesh::FaceHandle> fh(2);

	std::vector<txOPMesh::VertexHandle> vhtri(3);
	vhtri[0] = vh[0];
	vhtri[1] = vh[1];
	vhtri[2] = vh[3];
	fh[0] = mesh.add_face( vhtri );
	vhtri[0] = vh[3];
	vhtri[1] = vh[1];
	vhtri[2] = vh[2];
	fh[1] = mesh.add_face( vhtri );

	//
	printf( "HalfEdge:\n" );
	txOPMesh::FHIter fhit=mesh.fh_begin( fh[0] ) ;
	for ( ; fhit!=mesh.fh_end( fh[0] ); ++fhit ) {
		printf ( "------\n" );
		printf ( "Fist face he %d\n", fhit.handle().idx() );
		//printf ( "Face hand %d \n", fhit.handle() );
		printf ( "Face hand %d \n", mesh.face_handle( fhit.handle() ) );
		printf ( "HEID face node %d, %d \n" , 
			//fhit.handle
			//mesh.vertex_handle(fhit.current_halfedge_handle).idx(),
			//mesh.vertex_handle( fhit.current_halfedge_handle().idx() ).idx(),
			//mesh.vertex_handle( fhit.handle().idx() ).idx(),
			mesh.from_vertex_handle( fhit ).idx(),
			//mesh.opposite_vh(fhit).idx()
			mesh.to_vertex_handle( fhit ).idx()
			);
		
		printf ( "Fist face he opp %d\n", mesh.opposite_face_handle( fhit ) );
	}

	txOPMesh::FHIter fhit1=mesh.fh_begin( fh[1] );
	for ( ; fhit1!=mesh.fh_end( fh[1] ); ++fhit1 ) {
		printf ( "Secd face he %d\n", fhit1.handle().idx() );
		printf ( "Secd face he opp%d\n", mesh.opposite_face_handle( fhit ) );
	}

	// 
	//printf( "Face Adj:\n" );


	// write mesh to output.obj
	try
	{
		if ( !OpenMesh::IO::write_mesh(mesh, "testsquare.off") )
		{
			std::cerr << "Cannot write mesh to file 'output.off'" << std::endl;
			//return 1;
		}
	} catch ( std::exception& x )
	{
		std::cerr << x.what() << std::endl;
		//return 1;
	}

}
