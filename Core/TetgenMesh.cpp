/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2010 Louise Brown

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

#include "PrecompiledHeaders.h"
#include "TexGen.h"
#include "TetgenMesh.h"

extern "C"
{
#include "../Triangle/triangle.h"
#include "../Triangle/triangle_api.h"
}

using namespace TexGen;
using namespace std;

CTetgenMesh::CTetgenMesh(double Seed)
{
	m_Seed = Seed;
}

CTetgenMesh::~CTetgenMesh(void)
{
}

void CTetgenMesh::SaveTetgenMesh( CTextile &Textile, string OutputFilename, string Parameters, bool bPeriodic )
{
	tetgenio::facet *f;
	tetgenio::polygon *p;

	vector<CMesh> DomainMeshes;
	CMesh DomainMesh;
	
	pair<XYZ, XYZ> DomainAABB;
	XYZ P;

	if ( !Textile.AddSurfaceToMesh( m_Mesh, DomainMeshes, true ) )
	{
		TGERROR("Error creating surface mesh. Cannot generate tetgen mesh");
		return;
	}
	m_Mesh.ConvertQuadstoTriangles(true);

	// Add facets for domain planes
	vector<CMesh>::iterator itDomainMeshes;
	vector<CMesh> TriangulatedMeshes;
	vector<CMesh>::iterator itTriangulatedMeshes;
	int NumEdgeTris = 0;
	int iNumNodes = CMesh::GetNumNodes(CMesh::TRI);
	vector<XYZ> DomainMeshNormals;
	int i;
	vector<PLANEPARAMS> PlaneParams;
	vector<vector<int> > PolygonNumVertices;

	for ( itDomainMeshes = DomainMeshes.begin(), i = 0; itDomainMeshes != DomainMeshes.end(); itDomainMeshes++, ++i )
	{
		const list<int> &QuadIndices = itDomainMeshes->GetIndices(CMesh::QUAD);
		const list<int> &PolygonIndices = itDomainMeshes->GetIndices(CMesh::POLYGON);
		list<int>::const_iterator itQuadIndices;
		list<int>::const_iterator itPolyIndices;
		vector<int> NumVertices;


		CMesh TriangleMesh;

		// Convert domain quad points to 2D points.  ConvertRef contains information to restore 2D points back to 3D on correct plane
		vector< vector<XY> > ArrayPoints2D;
		vector<XY> Points2D;
		PLANEPARAMS ConvertRef;
		ConvertDomainPointsTo2D( QuadIndices, *itDomainMeshes, Points2D, ConvertRef );
		PlaneParams.push_back( ConvertRef );
		ArrayPoints2D.push_back( Points2D );

		// Save number of indices in each polygon (each polygon represents intersection of yarn with domain plane)
		if ( PolygonIndices.size() > 0 )
		{
			for ( itPolyIndices = PolygonIndices.begin(); itPolyIndices != PolygonIndices.end();  )
			{
				int Num = 0;
				int Start = *itPolyIndices;
				do
				{
					++Num;
					++itPolyIndices;
				}while( (itPolyIndices != PolygonIndices.end()) && ((*itPolyIndices) != Start) );

				if ( (*itPolyIndices) != Start )  // Reached end and not found complete polygon
				{
					TGERROR("Error creating intersection of yarn with domain");
					return;
				}
				++Num;
				NumVertices.push_back(Num);
				++itPolyIndices;
			}
			PolygonNumVertices.push_back(NumVertices);
		}
		// Add yarn end polygons to 2D point array
		int Poly = 0;
		for (itPolyIndices = PolygonIndices.begin(); itPolyIndices != PolygonIndices.end(); )
		{
			vector<XYZ> Points3D;
		    for ( int iNode = 0; iNode < NumVertices[Poly]-1; ++iNode )
			{
				XYZ Point = itDomainMeshes->GetNode( *(itPolyIndices++) );
				Points3D.push_back( Point );
		    }
			Points2D.clear();
			Convert3DTo2DCoordinates( Points3D, ConvertRef, Points2D );
			ArrayPoints2D.push_back( Points2D );
			itPolyIndices++;
			Poly++;
		}
		

		int j;
		if ( bPeriodic )
		{
			for ( j = 0; j < i; ++j )
			{
				// If planes normals are equal and opposite (ie opposite faces of box) replicate mesh on second plane
				if ( GetLength((ConvertRef.Normal + PlaneParams[j].Normal)) < 0.000000001 )
				{
					double dDist = DotProduct( PlaneParams[j].Normal, ( PlaneParams[j].RefPoint - ConvertRef.RefPoint ) );
					TriangleMesh = TriangulatedMeshes[j];
					OffsetMeshPoints( TriangleMesh, ConvertRef.Normal, dDist );
					break;
				}
			}
			if ( j == i ) // If not already triangulated plane, do so now
			{
				vector<XY> SeededSides;
				SeedSides( ArrayPoints2D[0] );
				Triangulate( ArrayPoints2D, TriangleMesh, ConvertRef );
			}
			NumEdgeTris += (int)TriangleMesh.GetIndices(CMesh::TRI).size() / iNumNodes;
			TriangulatedMeshes.push_back( TriangleMesh );
			}
		
	}

	m_in.numberoffacets = (int)m_Mesh.GetNumElements() + (int)DomainMeshes.size();
	m_in.facetlist = new tetgenio::facet[m_in.numberoffacets];

	// Add facets for yarn elements
	list<int>::const_iterator itIter;
	
	i = 0;
	for ( int j = 0; j < CMesh::NUM_ELEMENT_TYPES; ++j)
	{
		const list<int> &Indices = m_Mesh.GetIndices((CMesh::ELEMENT_TYPE)j);
		iNumNodes = CMesh::GetNumNodes((CMesh::ELEMENT_TYPE)j);
		for (itIter = Indices.begin(); itIter != Indices.end(); )
		{
			if ( j == CMesh::QUAD || j == CMesh::TRI )  // For the moment assume that all surface elements are quad or tri
			{
				  f = &m_in.facetlist[i];
				  f->numberofpolygons = 1;
				  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
				  f->numberofholes = 0;
				  f->holelist = NULL;
				  p = &f->polygonlist[0];
				  p->numberofvertices = iNumNodes;
				  p->vertexlist = new int[p->numberofvertices];
				  for ( int iNode = 0; iNode < iNumNodes; ++iNode )
				  {
					  p->vertexlist[iNode] = *(itIter++) + 1;
				  }
				  ++i;
			}	
			else
			{
				break;
			}
		}
	}

	// Add facets for domain planes
	if ( bPeriodic )
	{
		for ( itTriangulatedMeshes = TriangulatedMeshes.begin(); itTriangulatedMeshes != TriangulatedMeshes.end(); ++itTriangulatedMeshes )
		{
			const list<int> &TriIndices = itTriangulatedMeshes->GetIndices(CMesh::TRI);
			list<int>::const_iterator itTriIndices;

			int iNodeOffset = m_Mesh.GetNumNodes();  // Adding domain plane points to m_Mesh so need to continue from current max index
			int iNumNodes = 3;

			f = &m_in.facetlist[i];
			f->numberofpolygons = (int)TriIndices.size()/iNumNodes;
			f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
			f->numberofholes = 0;
			f->holelist = NULL;

			int iList = 0;
			
			int PolyInd = 0;
			for (itTriIndices = TriIndices.begin(); itTriIndices != TriIndices.end(); )
			{
				p = &f->polygonlist[PolyInd++];
				p->numberofvertices = iNumNodes;
				p->vertexlist = new int[p->numberofvertices];

				for ( int iNode = 0; iNode < p->numberofvertices; ++iNode )
				{
					XYZ Point = itTriangulatedMeshes->GetNode( *(itTriIndices++) );
					int ind = m_Mesh.GetClosestNodeDistance( Point, 0.000001);
					if ( ind == -1 ) // Add node if not in mesh yet
					{
						m_Mesh.AddNode( Point );
						p->vertexlist[iNode] = m_Mesh.GetNumNodes();
					}
					else  // Add existing node index to vertex list
	  					p->vertexlist[iNode] = ind + 1;
				}
			}
			++i;
		}
	}
	else // if not periodic need to add quad for domain edge and point for any polygons intersecting domain plane
	{
		int iFace = 0;
		for ( itDomainMeshes = DomainMeshes.begin(); itDomainMeshes != DomainMeshes.end(); ++itDomainMeshes )
		{
			const list<int> &QuadIndices = itDomainMeshes->GetIndices(CMesh::QUAD);
			const list<int> &PolygonIndices = itDomainMeshes->GetIndices(CMesh::POLYGON);
			list<int>::const_iterator itQuadIndices;
			list<int>::const_iterator itPolygonIndices;


			int iNodeOffset = m_Mesh.GetNumNodes();

			f = &m_in.facetlist[i];
			if ( PolygonIndices.empty() )
				f->numberofpolygons = 1;
			else
				f->numberofpolygons = (int)PolygonNumVertices[iFace].size() + 1;  // Number of polygons + the quad
			f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
			f->numberofholes = 0;
			f->holelist = NULL;

			int iList = 0;
			
			int PolyInd = 0;
			// Add quad points
			for (itQuadIndices = QuadIndices.begin(); itQuadIndices != QuadIndices.end(); )
			{
				p = &f->polygonlist[PolyInd++];
				p->numberofvertices = 4;
				p->vertexlist = new int[p->numberofvertices];

				for ( int iNode = 0; iNode < p->numberofvertices; ++iNode )
				{
					XYZ Point = itDomainMeshes->GetNode( *(itQuadIndices++) );
					int ind = m_Mesh.GetClosestNodeDistance( Point, 0.000001);
					if ( ind == -1 ) // Add node if not in mesh yet
					{
						m_Mesh.AddNode( Point );
						p->vertexlist[iNode] = m_Mesh.GetNumNodes();
					}
					else  // Add existing node index to vertex list
	  					p->vertexlist[iNode] = ind + 1;
				}
			}

			// Add polygon points
			if ( !PolygonIndices.empty() )
			{
				vector<int>::iterator itNumVertices = PolygonNumVertices[iFace].begin();
				for (itPolygonIndices = PolygonIndices.begin(); itPolygonIndices != PolygonIndices.end(); )
				{
					p = &f->polygonlist[PolyInd++];
					p->numberofvertices = *(itNumVertices++);
					p->vertexlist = new int[p->numberofvertices];
				
					for ( int iNode = 0; iNode < p->numberofvertices; ++iNode )
					{
						XYZ Point = itDomainMeshes->GetNode( *(itPolygonIndices++) );
						int ind = m_Mesh.GetClosestNodeDistance( Point, 0.000001);
						if ( ind == -1 ) // Add node if not in mesh yet
						{
							m_Mesh.AddNode( Point );
							p->vertexlist[iNode] = m_Mesh.GetNumNodes();
						}
						else  // Add existing node index to vertex list
	  						p->vertexlist[iNode] = ind + 1;
					}
				}
				++iFace;
			}
			++i;
			
		}
	}

	// Add the mesh nodes to the tetgen point list
	// All indices start from 1.
	m_in.firstnumber = 1;
	m_in.numberofpoints = m_Mesh.GetNumNodes();

	m_in.pointlist = new REAL[m_in.numberofpoints * 3];
	
	vector<XYZ>::iterator itNode;
	int iNodeInd = 0;
	for ( itNode = m_Mesh.NodesBegin(); itNode != m_Mesh.NodesEnd(); ++itNode )
	{
		m_in.pointlist[iNodeInd++] = (*itNode).x;
		m_in.pointlist[iNodeInd++] = (*itNode).y;
		m_in.pointlist[iNodeInd++] = (*itNode).z;
	}
	
	string strOutput;
	string strInput;
	int size = (int)OutputFilename.length();
	
	char* TetgenOutput = new char[size];
	char* TetgenInput = new char[size+5];
	strOutput = RemoveExtension( OutputFilename, ".inp" );
	strInput = strOutput + "Input";
	strcpy(TetgenOutput, strOutput.c_str());
	strcpy( TetgenInput, strInput.c_str());
	
	m_in.save_nodes(TetgenInput);
	m_in.save_poly(TetgenInput);
	delete [] TetgenInput;

	// Check the input mesh first
	try
	{
		tetrahedralize("d", &m_in, &m_out);
	}
	catch(...)
	{
		TGERROR("Tetrahedralize failed.  Intersections in PLC");
		return;
	}
	// Then create the mesh
	try
	{
		tetrahedralize((char*)Parameters.c_str(), &m_in, &m_out);
	}
	catch(...)
	{
		TGERROR("Tetrahedralize failed.  No mesh generated");
		TGERROR(Parameters);
		return;
	}
			
	// Output mesh to files 'barout.node', 'barout.ele' and 'barout.face'.
	m_out.save_nodes(TetgenOutput);
	m_out.save_elements(TetgenOutput);
	m_out.save_faces(TetgenOutput);
	delete [] TetgenOutput;

	SaveToAbaqus( OutputFilename, Textile );
}

// Version which triangulates domain boundaries and saves each triangle on boundaries as separate facet
/*void CTetgenMesh::SaveTetgenMesh( CTextile &Textile, string OutputFilename )
{
	tetgenio::facet *f;
	tetgenio::polygon *p;

	vector<CMesh> DomainMeshes;
	CMesh DomainMesh;
	pair<XYZ, XYZ> DomainAABB;
	XYZ P;

	Textile.AddSurfaceToMesh( m_Mesh, DomainMeshes, true );

	// Add facets for domain planes
	vector<CMesh>::iterator itDomainMeshes;
	vector<CMesh> TriangulatedMeshes;
	vector<CMesh>::iterator itTriangulatedMeshes;
	int NumEdgeTris = 0;
	int iNumNodes = CMesh::GetNumNodes(CMesh::TRI);
	vector<XYZ> DomainMeshNormals;
	int i;
	vector<PLANEPARAMS> PlaneParams;

	for ( itDomainMeshes = DomainMeshes.begin(), i = 0; itDomainMeshes != DomainMeshes.end(); itDomainMeshes++, ++i )
	{
		const list<int> &QuadIndices = itDomainMeshes->GetIndices(CMesh::QUAD);
		const list<int> &PolygonIndices = itDomainMeshes->GetIndices(CMesh::POLYGON);
		list<int>::const_iterator itQuadIndices;
		list<int>::const_iterator itPolyIndices;
		vector<int> NumVertices;


		CMesh TriangleMesh;

		vector< vector<XY> > ArrayPoints2D;
		vector<XY> Points2D;
		PLANEPARAMS ConvertRef;
		ConvertDomainPointsTo2D( QuadIndices, *itDomainMeshes, Points2D, ConvertRef );
		PlaneParams.push_back( ConvertRef );
		ArrayPoints2D.push_back( Points2D );

		// Save number of indices in each polygon (each polygon represents intersection of yarn with domain plane)
		if ( PolygonIndices.size() > 0 )
		{
			for ( itPolyIndices = PolygonIndices.begin(); itPolyIndices != PolygonIndices.end();  )
			{
				int Num = 0;
				int Start = *itPolyIndices;
				do
				{
					++Num;
					++itPolyIndices;
				}while( (*itPolyIndices) != Start );
				++Num;
				NumVertices.push_back(Num);
				++itPolyIndices;
			}
		}
		// Add yarn end polygons to 
		int Poly = 0;
		for (itPolyIndices = PolygonIndices.begin(); itPolyIndices != PolygonIndices.end(); )
		{
			vector<XYZ> Points3D;
		    for ( int iNode = 0; iNode < NumVertices[Poly]-1; ++iNode )
			{
				XYZ Point = itDomainMeshes->GetNode( *(itPolyIndices++) );
				Points3D.push_back( Point );
		    }
			Points2D.clear();
			Convert3DTo2DCoordinates( Points3D, ConvertRef, Points2D );
			ArrayPoints2D.push_back( Points2D );
			itPolyIndices++;
			Poly++;
		}
		

		int j;
		for ( j = 0; j < i; ++j )
		{
			double length = GetLength(ConvertRef.Normal + PlaneParams[j].Normal);
			if ( GetLength((ConvertRef.Normal + PlaneParams[j].Normal)) < 0.000000001 )
			//if ( ConvertRef.Normal == ( -1 * PlaneParams[i].Normal ) )
			{
				
				double dDist = DotProduct( PlaneParams[j].Normal, ( PlaneParams[j].RefPoint - ConvertRef.RefPoint ) );
				TriangleMesh = TriangulatedMeshes[j];
				OffsetMeshPoints( TriangleMesh, ConvertRef.Normal, dDist );
				break;
			}
		}
		if ( j == i )
		{
			vector<XY> SeededSides;
			SeedSides( ArrayPoints2D[0] );
			Triangulate( ArrayPoints2D, TriangleMesh, ConvertRef );
		}
		NumEdgeTris += TriangleMesh.GetIndices(CMesh::TRI).size() / iNumNodes;
		TriangulatedMeshes.push_back( TriangleMesh );
	}

	m_in.numberoffacets = m_Mesh.GetNumElements() + NumEdgeTris;
	m_in.facetlist = new tetgenio::facet[m_in.numberoffacets];

	// Add facets for yarn elements
	list<int>::const_iterator itIter;
	
	i = 0;
	for ( int j = 0; j < CMesh::NUM_ELEMENT_TYPES; ++j)
	{
		const list<int> &Indices = m_Mesh.GetIndices((CMesh::ELEMENT_TYPE)j);
		iNumNodes = CMesh::GetNumNodes((CMesh::ELEMENT_TYPE)j);
		for (itIter = Indices.begin(); itIter != Indices.end(); )
		{
			if ( j == CMesh::QUAD || j == CMesh::TRI )  // For the moment assume that all surface elements are quad or tri
			{
				  f = &m_in.facetlist[i];
				  f->numberofpolygons = 1;
				  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
				  f->numberofholes = 0;
				  f->holelist = NULL;
				  p = &f->polygonlist[0];
				  p->numberofvertices = iNumNodes;
				  p->vertexlist = new int[p->numberofvertices];
				  for ( int iNode = 0; iNode < iNumNodes; ++iNode )
				  {
					  p->vertexlist[iNode] = *(itIter++) + 1;
				  }
				  ++i;
			}	
			else
			{
				break;
			}
		}
	}

	for ( itTriangulatedMeshes = TriangulatedMeshes.begin(); itTriangulatedMeshes != TriangulatedMeshes.end(); ++itTriangulatedMeshes )
	{
		const list<int> &TriIndices = itTriangulatedMeshes->GetIndices(CMesh::TRI);
		list<int>::const_iterator itTriIndices;

		

		int iList = 0;
		int iNumNodes = 3;
		for (itTriIndices = TriIndices.begin(); itTriIndices != TriIndices.end(); )
		{
			f = &m_in.facetlist[i];
			f->numberofpolygons = 1;
			f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
			f->numberofholes = 0;
			f->holelist = NULL;
			p = &f->polygonlist[0];
			p->numberofvertices = iNumNodes;
		    p->vertexlist = new int[p->numberofvertices];
			for ( int iNode = 0; iNode < p->numberofvertices; ++iNode )
			{
				XYZ Point = itTriangulatedMeshes->GetNode( *(itTriIndices++) );
				int ind = m_Mesh.GetClosestNodeDistance( Point, 0.000001);
				if ( ind == -1 ) // Add node if not in mesh yet
				{
					m_Mesh.AddNode( Point );
					p->vertexlist[iNode] = m_Mesh.GetNumNodes();
				}
				else  // Add existing node index to vertex list
		  			p->vertexlist[iNode] = ind + 1;
		    }
			++i;
		}
	}

	// All indices start from 1.
	m_in.firstnumber = 1;
	m_in.numberofpoints = m_Mesh.GetNumNodes();

	m_in.pointlist = new REAL[m_in.numberofpoints * 3];
	
	vector<XYZ>::iterator itNode;
	int iNodeInd = 0;
	for ( itNode = m_Mesh.NodesBegin(); itNode != m_Mesh.NodesEnd(); ++itNode )
	{
		m_in.pointlist[iNodeInd++] = (*itNode).x;
		m_in.pointlist[iNodeInd++] = (*itNode).y;
		m_in.pointlist[iNodeInd++] = (*itNode).z;
	}
	
	m_in.save_nodes("TetGenInput");
	m_in.save_poly("TetGenInput");

	try
	{
		tetrahedralize("d", &m_in, &m_out);
	}
	catch(...)
	{
		TGERROR("Tetrahedralize failed.  Intersections in PLC");
		return;
	}
	try
	{
		tetrahedralize("pqA", &m_in, &m_out);
	}
	catch(...)
	{
		TGERROR("Tetrahedralize failed.  No mesh generated");
		return;
	}
	  // Output mesh to files 'barout.node', 'barout.ele' and 'barout.face'.
	  m_out.save_nodes("TetGenOut");
	  m_out.save_elements("TetGenOut");
	  m_out.save_faces("TetGenOut");
}*/

// Version which just uses domain plane outline and any yarn intersect polygons
// Allows additional points to be generated on boundaries
/*void CTetgenMesh::SaveTetgenMesh( CTextile &Textile, string OutputFilename )
{
	tetgenio::facet *f;
	tetgenio::polygon *p;
	
	vector<XYZ>::iterator itNode;
	vector<CMesh> DomainMeshes;

	CMesh DomainMesh;
	pair<XYZ, XYZ> DomainAABB;
	XYZ P;

	Textile.AddSurfaceToMesh( m_Mesh, DomainMeshes, true );

	m_in.numberoffacets = m_Mesh.GetNumElements() + DomainMeshes.size();//(CMesh::QUAD);  // Change back to () if use all elements
	m_in.facetlist = new tetgenio::facet[m_in.numberoffacets];

	// Add facets for yarn elements
	list<int>::const_iterator itIter;
	int iNumNodes;
	int i = 0;
	for ( int j = 0; j < CMesh::NUM_ELEMENT_TYPES; ++j)
	{
		const list<int> &Indices = m_Mesh.GetIndices((CMesh::ELEMENT_TYPE)j);
		iNumNodes = CMesh::GetNumNodes((CMesh::ELEMENT_TYPE)j);
		for (itIter = Indices.begin(); itIter != Indices.end(); )
		{
			if ( j == CMesh::QUAD || j == CMesh::TRI )  // For the moment assume that all surface elements are quad or tri
			{
				  f = &m_in.facetlist[i];
				  f->numberofpolygons = 1;
				  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
				  f->numberofholes = 0;
				  f->holelist = NULL;
				  p = &f->polygonlist[0];
				  p->numberofvertices = iNumNodes;
				  p->vertexlist = new int[p->numberofvertices];
				  for ( int iNode = 0; iNode < iNumNodes; ++iNode )
				  {
					  p->vertexlist[iNode] = *(itIter++) + 1;
				  }
				  ++i;
			}	
			else
			{
				break;
			}
		}
	}

	// Add facets for domain planes
	vector<CMesh>::iterator itDomainMeshes;
	for ( itDomainMeshes = DomainMeshes.begin(); itDomainMeshes != DomainMeshes.end(); itDomainMeshes++ )
	{
		const list<int> &QuadIndices = itDomainMeshes->GetIndices(CMesh::QUAD);
		const list<int> &PolygonIndices = itDomainMeshes->GetIndices(CMesh::POLYGON);
		list<int>::const_iterator itPolyIndices;
		list<int>::const_iterator itQuadIndices;
		vector<int> NumVertices;


		// Save number of indices in each polygon (each polygon represents intersection of yarn with domain plane)
		if ( PolygonIndices.size() > 0 )
		{
			for ( itPolyIndices = PolygonIndices.begin(); itPolyIndices != PolygonIndices.end();  )
			{
				int Num = 0;
				int Start = *itPolyIndices;
				do
				{
					++Num;
					++itPolyIndices;
				}while( (*itPolyIndices) != Start );
				++Num;
				NumVertices.push_back(Num);
				++itPolyIndices;
			}
		}

		f = &m_in.facetlist[i];
		f->numberofpolygons = itDomainMeshes->GetNumElements() + NumVertices.size();
		f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
		f->numberofholes = 0;
		f->holelist = NULL;

		int iList = 0;
		for (itQuadIndices = QuadIndices.begin(); itQuadIndices != QuadIndices.end(); )
		{
			p = &f->polygonlist[iList++];
			p->numberofvertices = CMesh::GetNumNodes(CMesh::QUAD);
		    p->vertexlist = new int[p->numberofvertices];
			for ( int iNode = 0; iNode < p->numberofvertices; ++iNode )
			{
				XYZ Point = itDomainMeshes->GetNode( *(itQuadIndices++) );
				int ind = m_Mesh.GetClosestNodeDistance( Point, 0.000001);
				if ( ind == -1 ) // Add node if not in mesh yet
				{
					m_Mesh.AddNode( Point );
					p->vertexlist[iNode] = m_Mesh.GetNumNodes();
				}
				else  // Add existing node index to vertex list
		  			p->vertexlist[iNode] = ind + 1;
		    }
		}
		
		int Poly = 0;
		for (itPolyIndices = PolygonIndices.begin(); itPolyIndices != PolygonIndices.end(); )
		{
			p = &f->polygonlist[iList++];
			p->numberofvertices = NumVertices[Poly]-1; // Don't want to duplicate start point
		    p->vertexlist = new int[p->numberofvertices];
		    for ( int iNode = 0; iNode < NumVertices[Poly]-1; ++iNode )
			{
				XYZ Point = itDomainMeshes->GetNode( *(itPolyIndices++) );
				int ind = m_Mesh.GetClosestNodeDistance( Point, 0.000001);
				if ( ind == -1 )  // Add node if not in mesh yet
				{
					m_Mesh.AddNode( Point );
					p->vertexlist[iNode] = m_Mesh.GetNumNodes();
				}
				else  // Add existing node index to vertex list
		  			p->vertexlist[iNode] = ind + 1;
		    }
			itPolyIndices++;
			Poly++;
		}
		++i;
	}

/*	m_in.regionlist = new REAL[2 * 5];
	m_in.numberofregions = 2;
	m_in.regionlist[0] = -0.1;
	m_in.regionlist[1] = 0.0;
	m_in.regionlist[2] = 0.0;
	m_in.regionlist[3] = -10;
	m_in.regionlist[4] = 0.0;

	m_in.regionlist[5] = 0.5;
	m_in.regionlist[6] = 0.0;
	m_in.regionlist[7] = 0.0;
	m_in.regionlist[8] = -20;
	m_in.regionlist[9] = 0.0;*/

	// All indices start from 1.
/*	m_in.firstnumber = 1;
	m_in.numberofpoints = m_Mesh.GetNumNodes();

	m_in.pointlist = new REAL[m_in.numberofpoints * 3];
	
	int iNodeInd = 0;
	for ( itNode = m_Mesh.NodesBegin(); itNode != m_Mesh.NodesEnd(); ++itNode )
	{
		m_in.pointlist[iNodeInd++] = (*itNode).x;
		m_in.pointlist[iNodeInd++] = (*itNode).y;
		m_in.pointlist[iNodeInd++] = (*itNode).z;
	}
	
	m_in.save_nodes("TetGenInput");
	m_in.save_poly("TetGenInput");
	//m_in.load_poly("TetGenInputEnd2");*/

/*	try
	{
		tetrahedralize("d", &m_in, &m_out);
	}
	catch(...)
	{
		TGERROR("Tetrahedralize failed.  Intersections in PLC");
		return;
	}
	try
	{
		tetrahedralize("pqA", &m_in, &m_out);
	}
	catch(...)
	{
		TGERROR("Tetrahedralize failed.  No mesh generated");
		return;
	}
	  // Output mesh to files 'barout.node', 'barout.ele' and 'barout.face'.
	  m_out.save_nodes("TetGenOut");
	  m_out.save_elements("TetGenOut");
	  m_out.save_faces("TetGenOut");
}*/
/*
int main(int argc, char *argv[])
{
  tetgenio in, out;
  tetgenio::facet *f;
  tetgenio::polygon *p;
  int i;

  // All indices start from 1.
  in.firstnumber = 1;

  in.numberofpoints = 8;
  in.pointlist = new REAL[in.numberofpoints * 3];
  in.pointlist[0]  = 0;  // node 1.
  in.pointlist[1]  = 0;
  in.pointlist[2]  = 0;
  in.pointlist[3]  = 2;  // node 2.
  in.pointlist[4]  = 0;
  in.pointlist[5]  = 0;
  in.pointlist[6]  = 2;  // node 3.
  in.pointlist[7]  = 2;
  in.pointlist[8]  = 0;
  in.pointlist[9]  = 0;  // node 4.
  in.pointlist[10] = 2;
  in.pointlist[11] = 0;
  // Set node 5, 6, 7, 8.
  for (i = 4; i < 8; i++) {
    in.pointlist[i * 3]     = in.pointlist[(i - 4) * 3];
    in.pointlist[i * 3 + 1] = in.pointlist[(i - 4) * 3 + 1];
    in.pointlist[i * 3 + 2] = 12;
  }

  in.numberoffacets = 6;
  in.facetlist = new tetgenio::facet[in.numberoffacets];
  in.facetmarkerlist = new int[in.numberoffacets];

  // Facet 1. The leftmost facet.
  f = &in.facetlist[0];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 1;
  p->vertexlist[1] = 2;
  p->vertexlist[2] = 3;
  p->vertexlist[3] = 4;
  
  // Facet 2. The rightmost facet.
  f = &in.facetlist[1];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 5;
  p->vertexlist[1] = 6;
  p->vertexlist[2] = 7;
  p->vertexlist[3] = 8;

  // Facet 3. The bottom facet.
  f = &in.facetlist[2];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 1;
  p->vertexlist[1] = 5;
  p->vertexlist[2] = 6;
  p->vertexlist[3] = 2;

  // Facet 4. The back facet.
  f = &in.facetlist[3];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 2;
  p->vertexlist[1] = 6;
  p->vertexlist[2] = 7;
  p->vertexlist[3] = 3;

  // Facet 5. The top facet.
  f = &in.facetlist[4];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 3;
  p->vertexlist[1] = 7;
  p->vertexlist[2] = 8;
  p->vertexlist[3] = 4;

  // Facet 6. The front facet.
  f = &in.facetlist[5];
  f->numberofpolygons = 1;
  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
  f->numberofholes = 0;
  f->holelist = NULL;
  p = &f->polygonlist[0];
  p->numberofvertices = 4;
  p->vertexlist = new int[p->numberofvertices];
  p->vertexlist[0] = 4;
  p->vertexlist[1] = 8;
  p->vertexlist[2] = 5;
  p->vertexlist[3] = 1;

  // Set 'in.facetmarkerlist'

  in.facetmarkerlist[0] = -1;
  in.facetmarkerlist[1] = -2;
  in.facetmarkerlist[2] = 0;
  in.facetmarkerlist[3] = 0;
  in.facetmarkerlist[4] = 0;
  in.facetmarkerlist[5] = 0;

  // Output the PLC to files 'barin.node' and 'barin.poly'.
  in.save_nodes("barin");
  in.save_poly("barin");

  // Tetrahedralize the PLC. Switches are chosen to read a PLC (p),
  //   do quality mesh generation (q) with a specified quality bound
  //   (1.414), and apply a maximum volume constraint (a0.1).

  tetrahedralize("pq1.414a0.1", &in, &out);

  // Output mesh to files 'barout.node', 'barout.ele' and 'barout.face'.
  out.save_nodes("barout");
  out.save_elements("barout");
  out.save_faces("barout");

  return 0;
}
*/

bool CTetgenMesh::Triangulate( vector<vector<XY> > &PolygonPoints, CMesh& OutputMesh, PLANEPARAMS& ConvertRef )
{
//	char szSwitches[128];
	stringstream Switches;

	double dMaxArea = 0.5*m_Seed*m_Seed;
	double dMinAngle = 20;

	// These are the switches to be used with triangle
	// http://www-2.cs.cmu.edu/~quake/triangle.switch.html
	// -p Triangulates a Planar Straight Line Graph (.poly file).
	// -z Numbers all items starting from zero (rather than one).
	// -n Outputs (to a .neigh file) a list of triangles neighboring each triangle.
	// -P Suppresses the output .poly file.
	// -B Suppresses boundary markers in the output .node, .poly, and .edge output files.
	// -A Assigns a regional attribute to each triangle that identifies what segment-bounded region it belongs to.
	// -Q Quiet: Suppresses all explanation of what Triangle is doing, unless an error occurs.
	// -q Quality mesh generation with no angles smaller than 20 degrees. An alternate minimum angle may be specified after the `q'.
	// -a Imposes a maximum triangle area constraint. A fixed area constraint (that applies to every triangle) may be specified after the `a', or varying area constraints may be read from a .poly file or .area file.
	// -Y Prohibits the insertion of Steiner points on the mesh boundary

/*#ifdef _DEBUG
	sprintf(szSwitches, "pzAPBq%fa%f", dMinAngle, dMaxArea);
#else // _DEBUG
	sprintf(szSwitches, "pzAQPBq%fa%f", dMinAngle, dMaxArea);
#endif // _DEBUG*/
#ifndef _DEBUG
	Switches << "Q";
#endif
	// Triangle has trouble parsing values given in scientific format so use fixed format with a
	// rediculously high precision to get around the problem
	Switches << "pzAPBq" << setiosflags(ios::fixed) << setprecision(20) << dMinAngle << "a" << dMaxArea;
	Switches << "YY";

	triangleio TriangleInput;
	triangleio TriangleOutput;

	context *ctx;
	ctx = triangle_context_create();

	triangle_context_options(ctx, (char*)Switches.str().c_str());

	memset(&TriangleInput, 0, sizeof(TriangleInput));
	memset(&TriangleOutput, 0, sizeof(TriangleOutput));

	// Input Nodes
	vector<vector<XY> >::iterator itArrayPolygonPoints;
	int iNumPoints = 0;
	for ( itArrayPolygonPoints = PolygonPoints.begin(); itArrayPolygonPoints != PolygonPoints.end(); ++ itArrayPolygonPoints )
	{
		iNumPoints += (int)(*itArrayPolygonPoints).size();
	}
	TriangleInput.pointlist = new REAL [iNumPoints*2];
	TriangleInput.numberofpoints = iNumPoints;

	TriangleInput.segmentlist = new int [iNumPoints*2];
	TriangleInput.numberofsegments = iNumPoints;
	
	int i = 0;
	for ( itArrayPolygonPoints = PolygonPoints.begin(); itArrayPolygonPoints != PolygonPoints.end(); ++ itArrayPolygonPoints )
	{
		vector<XY>::iterator itPolyPoints;
		int j = 0;
		int iNumPolyPoints = (int)(*itArrayPolygonPoints).size();
		int StartIndex = i;
		for (itPolyPoints = (*itArrayPolygonPoints).begin(); itPolyPoints != (*itArrayPolygonPoints).end(); ++itPolyPoints)
		{
			// Input nodes
			TriangleInput.pointlist[i*2] = itPolyPoints->x;
			TriangleInput.pointlist[i*2+1] = itPolyPoints->y;

			// Input Segments
			TriangleInput.segmentlist[i*2] = i;
			if ( j < iNumPolyPoints-1 )
			{
				TriangleInput.segmentlist[i*2+1] = i+1;
			}
			else
			{
				TriangleInput.segmentlist[i*2+1] = StartIndex;
			}

			++i;
			++j;
		}
	}

	// Input regions
/*	TriangleInput.regionlist = new REAL [m_ProjectedRegions.size()*4];
	TriangleInput.numberofregions = m_ProjectedRegions.size();

	for (i=0; i<TriangleInput.numberofregions; ++i)
	{
		TriangleInput.regionlist[i*4] = m_ProjectedRegions[i].Center.x;
		TriangleInput.regionlist[i*4+1] = m_ProjectedRegions[i].Center.y;
		TriangleInput.regionlist[i*4+2] = i;
		TriangleInput.regionlist[i*4+3] = 0;	// this is unused
	}*/

	triangle_mesh_create(ctx, &TriangleInput);


	delete [] TriangleInput.pointlist;
	delete [] TriangleInput.segmentlist;
//	delete [] TriangleInput.regionlist;

//	m_ProjectedMesh.Clear();
	triangle_mesh_copy(ctx, &TriangleOutput, 1, 1);

	vector<XY> Points2D;
	for (int i=0; i<TriangleOutput.numberofpoints; ++i)
	{
		XY Point;
		Point.x = TriangleOutput.pointlist[i*2];
		Point.y = TriangleOutput.pointlist[i*2+1];
		Points2D.push_back(Point);
		//OutputMesh.AddNode(Point);
	}
	vector<XYZ> Points3D;
	vector<XYZ>::iterator itPoints3D;
	Convert2DTo3DCoordinates( Points2D, Points3D, ConvertRef );

	for ( itPoints3D = Points3D.begin(); itPoints3D != Points3D.end(); ++itPoints3D )
	{
		OutputMesh.AddNode( *itPoints3D );
	}

	//m_TriangleRegions.clear();
	for (int i=0; i<TriangleOutput.numberoftriangles; ++i)
	{
		OutputMesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i*3]);
		OutputMesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i*3+1]);
		OutputMesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i*3+2]);
		//m_TriangleRegions.push_back((int)TriangleOutput.triangleattributelist[i]);
	}

	triangle_free(TriangleOutput.pointlist);
	triangle_free(TriangleOutput.trianglelist);
	//trifree(TriangleOutput.triangleattributelist);
//	trifree(TriangleOutput.neighborlist);

	triangle_context_destroy(ctx);
	return true;
}

bool CTetgenMesh::ConvertDomainPointsTo2D( const list<int> &QuadIndices, CMesh& DomainMesh, vector<XY>& Points2D, PLANEPARAMS& ConvertRef )
{
	list<int>::const_iterator itQuadIndices;

	for ( itQuadIndices = QuadIndices.begin(); itQuadIndices != QuadIndices.end(); )
	{
		int numNodes = CMesh::GetNumNodes(CMesh::QUAD);
		vector<XYZ> Points3D;
		for ( int iNode = 0; iNode < numNodes; ++iNode )
		{
			XYZ Point = DomainMesh.GetNode( *(itQuadIndices++) );
			Points3D.push_back(Point);
	    }
		if ( Points3D.size() < 3 )
			return false;
		ConvertRef.RefPoint = Points3D[0];
		ConvertRef.XAxis = Points3D[1] - ConvertRef.RefPoint;
		ConvertRef.Normal = CrossProduct( ConvertRef.XAxis, (Points3D[2] - ConvertRef.RefPoint));
		ConvertRef.YAxis = CrossProduct( ConvertRef.Normal, ConvertRef.XAxis );
		ConvertRef.XAxis = Normalise( ConvertRef.XAxis );
		ConvertRef.YAxis = Normalise( ConvertRef.YAxis );
		ConvertRef.Normal = Normalise( ConvertRef.Normal );

		Convert3DTo2DCoordinates( Points3D, ConvertRef, Points2D );
	}
	return true;
}

void CTetgenMesh::Convert3DTo2DCoordinates( vector<XYZ>& Points3D, PLANEPARAMS& ConvertRef, vector<XY>& Points2D )
{
	vector<XYZ>::iterator itPoints3D;
		
	for ( itPoints3D = Points3D.begin(); itPoints3D != Points3D.end(); ++itPoints3D )
	{
		XYZ Relative = *itPoints3D - ConvertRef.RefPoint;
		XY Point2D;
		Point2D.x = DotProduct( ConvertRef.XAxis, Relative );
		Point2D.y = DotProduct( ConvertRef.YAxis, Relative );
		Points2D.push_back( Point2D );
	}
}

void CTetgenMesh::Convert2DTo3DCoordinates( vector<XY>& Points2D, vector<XYZ>& Points3D, PLANEPARAMS& ConvertRef )		
{
	// Convert the 2D points back to the global 3d coordinate system
	vector<XY>::iterator itPoints2D;
	
	for ( itPoints2D = Points2D.begin(); itPoints2D != Points2D.end(); ++itPoints2D )
	{
		XYZ Point3D;
		Point3D = ConvertRef.XAxis * itPoints2D->x;
		Point3D += ConvertRef.YAxis * itPoints2D->y;
		// Translate the point to its global position
		Point3D += ConvertRef.RefPoint;
		// Add the new 3d point to the list
		Points3D.push_back(Point3D);
	}	
}

void CTetgenMesh::SeedSides( vector<XY>& Points )
{
	vector<XY> SeededSides;
	vector<XY>::iterator itPoints;

	XY StartPoint;
	for ( itPoints = Points.begin(); itPoints != Points.end(); )
	{

		if ( itPoints == Points.begin() )
		{
			StartPoint = *itPoints;
		}
		XY P1 = *(itPoints++);
		XY P2;
		if ( itPoints != Points.end() )
		{
			P2 = *itPoints;
		}
		else
		{
			P2 = StartPoint;
		}
		SeededSides.push_back(P1);
		double dLength = GetLength(P1, P2);
		int iNumSplits = int( floor( dLength/m_Seed ));
		for ( int i = 1; i < iNumSplits; ++i )
		{
			double u = double(i) / double(iNumSplits);
			XY Point = P1 + (P2-P1)*u;
			SeededSides.push_back(Point);
		}
	}
	Points.clear();
	Points.insert( Points.begin(), SeededSides.begin(), SeededSides.end() );
}

void CTetgenMesh::OffsetMeshPoints( CMesh& Mesh, XYZ& Normal, double dDist )
{
	int iNumNodes = Mesh.GetNumNodes();
	for ( int i = 0; i < iNumNodes; ++i )
	{
		XYZ Point = Mesh.GetNode( i );
		Point += Normal * dDist;
		Mesh.SetNode( i, Point );
	}
}

void CTetgenMesh::SaveToAbaqus( string Filename, CTextile &Textile )
{
	CMesh TetMesh;

	// Store output mesh in CMesh
	for ( int i = 0; i < m_out.numberofpoints*3; ) // Three REALs in pointlist for each point
	{
		XYZ Point;
		Point.x = m_out.pointlist[i++];
		Point.y = m_out.pointlist[i++];
		Point.z = m_out.pointlist[i++];
		TetMesh.AddNode( Point );
	}
	
	CMesh::ELEMENT_TYPE ElementType = m_out.numberofcorners == 4 ? CMesh::TET : CMesh::QUADRATIC_TET;

	for ( int i = 0; i < m_out.numberoftetrahedra; i++ )
	{
		vector<int> Indices;
		for ( int j = 0; j < m_out.numberofcorners; j++ )
		{
			Indices.push_back( m_out.tetrahedronlist[i*m_out.numberofcorners + j]-1 );  // Tetgen indices start from 1
		}
		TetMesh.AddElement( ElementType, Indices );
	}

	vector<POINT_INFO> ElementsInfo;
	Textile.GetPointInformation( TetMesh.GetElementCenters( ElementType ), ElementsInfo );
	TetMesh.SaveToABAQUS( Filename, &ElementsInfo, false, false );
}
