/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2017 Mikhail Matveev

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
#include "OctreeVoxelMesh.h"
#include "TexGen.h"
#include "PeriodicBoundaries.h"
#include "omp.h"
#include <iterator>
#include <string>
#include <algorithm>

using namespace TexGen;

//CTextile gTextile;
//pair<XYZ, XYZ>	g_DomainAABB;

static const int    corner_num_hanging[P4EST_CHILDREN] =
#ifndef P4_TO_P8
{ 1, 2, 2, 1 }
#else
{ 1, 2, 2, 4, 2, 4, 4, 1 }
#endif
;

static const int    zero = 0;           /**< Constant zero. */
static const int    ones = P4EST_CHILDREN - 1;  /**< One bit per dimension. */
static const int   *corner_to_hanging[P4EST_CHILDREN];

/** For each node i of the reference quadrant, corner_num_hanging[i] many. */
// This stuff is defined in P4EST but by some reason it is "unresolved" when compiled
const int           p8est_face_corners[6][4] =
{{ 0, 2, 4, 6 },
 { 1, 3, 5, 7 },
 { 0, 1, 4, 5 },
 { 2, 3, 6, 7 },
 { 0, 1, 2, 3 },
 { 4, 5, 6, 7 }};
const int           p8est_edge_corners[12][2] =
{{ 0, 1 },
 { 2, 3 },
 { 4, 5 },
 { 6, 7 },
 { 0, 2 },
 { 1, 3 },
 { 4, 6 },
 { 5, 7 },
 { 0, 4 },
 { 1, 5 },
 { 2, 6 },
 { 3, 7 }};

int COctreeVoxelMesh::max_level;

vector<XYZ> COctreeVoxelMesh::cornerPoints;
vector<XYZ> COctreeVoxelMesh::CentrePoints;

vector<vector<int>> COctreeVoxelMesh::FaceX_min;
vector<vector<int>> COctreeVoxelMesh::FaceX_max;
vector<vector<int>> COctreeVoxelMesh::FaceY_min;
vector<vector<int>> COctreeVoxelMesh::FaceY_max;
vector<vector<int>> COctreeVoxelMesh::FaceZ_min;
vector<vector<int>> COctreeVoxelMesh::FaceZ_max;

CTextile COctreeVoxelMesh::gTextile;
pair<XYZ, XYZ>	COctreeVoxelMesh::g_DomainAABB;
vector<char> COctreeVoxelMesh::materialInfo;

// Tolerance is quarter of the max refinement
int my_comparison(double x, double y)
{
	if ( fabs(x - y) < 0.25* P4EST_QUADRANT_LEN(12) / P4EST_QUADRANT_LEN(0))
		return 1;
	else
		return 0;
}

// This function comes from a p4est example. It decodes tree information to node numbering.
// I still have no idea how exactly it works :)
static int
lnodes_decode2 (p4est_lnodes_code_t face_code, int hanging_corner[P4EST_CHILDREN])
{
	if (face_code) {
		const int           c = (int) (face_code & ones);
		int                 i, h;
		int                 work = (int) (face_code >> P4EST_DIM);

		/* These two corners are never hanging by construction. */
		hanging_corner[c] = hanging_corner[c ^ ones] = -1;
		for (i = 0; i < P4EST_DIM; ++i) {
		  /* Process face hanging corners. */
		  h = c ^ (1 << i);
		  hanging_corner[h ^ ones] = (work & 1) ? c : -1;
	#ifdef P4_TO_P8
		  /* Process edge hanging corners. */
		  hanging_corner[h] = (work & P4EST_CHILDREN) ? c : -1;
	#endif
		  work >>= 1;
		}
		return 1;
	}
	return 0;
}

// Return number of a node which is duplicated by a point or -1 if no duplicate found
// Do the shifts etc
int duplicatedHangingNode(double vxyz[3], double hang_coord[8][3], int hang_nums[8])
{
	int i = 0;

	for (i = 0; i < 8; i++) {
		if ( hang_coord[i][0] == vxyz[0] && hang_coord[i][1] == vxyz[1] && hang_coord[i][2] == vxyz[2]) {
			return hang_nums[i];
		}
	}

	// Shift array for a new point
	for (i = 0; i < 8; i++) {
		hang_coord[i][0] = hang_coord[i+1][0];
		hang_coord[i][1] = hang_coord[i+1][1];
		hang_coord[i][2] = hang_coord[i+1][2];
		hang_nums[i] = hang_nums[i+1];
	}
	return -1;
}

pair<int, int> most_common(vector<int> v) {
	int currentVal = v[0];
	int currentCount = 0;
	int mostVal = v[0];
	int mostCount = 0;

	for (int i = 0; i < v.size(); i++) {
		int c = count(v.begin(), v.end(), v[i]);
		if (c > mostCount) {
			mostCount = c;
			mostVal = v[i];
		}
	}

	return make_pair(mostVal, mostCount);
}

// P4EST should be initialised with initial coordinates of the unit cell vertices
// TODO: Try to initialise P4EST with several elements to have better refinement in a certain direction
int writeTempFile(string filename, pair<XYZ, XYZ> myDomain)
{
	ofstream TempFile(filename);
	if (!TempFile) {
		TGERROR("Cannot create an initialisation *.inp file " << filename);
		return -1;
	}
	TempFile << "*HEADING" << endl;
	TempFile << "This is a temp input for the octree refinement" << endl;
	TempFile << "*NODE" << endl;
	TempFile << "1, " << myDomain.first.x<< ", " << myDomain.first.y << ", " << myDomain.first.z << endl;
	TempFile << "2, " << myDomain.second.x<< ", " << myDomain.first.y << ", " << myDomain.first.z << endl;
	TempFile << "3, " << myDomain.first.x<< ", " << myDomain.second.y << ", " << myDomain.first.z << endl;
	TempFile << "4, " << myDomain.second.x<< ", " << myDomain.second.y << ", " << myDomain.first.z << endl;

	TempFile << "5, " << myDomain.first.x <<", " << myDomain.first.y << ", " << myDomain.second.z << endl;
	TempFile << "6, " << myDomain.second.x<< ", " << myDomain.first.y << ", " << myDomain.second.z << endl;
	TempFile << "7, " << myDomain.first.x<< ", " << myDomain.second.y << ", " << myDomain.second.z << endl;
	TempFile << "8, " << myDomain.second.x<< ", " << myDomain.second.y << ", " << myDomain.second.z << endl;

	TempFile << "*ELEMENT,TYPE=C3D8R" << endl;
	TempFile << "1, 5, 7, 3, 1, 6, 8, 4, 2" << endl;
	TempFile.close();
	return 0;
}

vector<int> GetFaceIndices(CMesh::ELEMENT_TYPE ElemType, const set<int> &NodeIndices)
{
	vector<int> facesInd;
	int numFaces = 0;
	if (NodeIndices.size() == 5)
		numFaces = 1;
	if (NodeIndices.size() == 6)
		numFaces = 2;
	if (NodeIndices.size() == 7)
		numFaces = 3;

	// We are in trouble! All the nodes belong to a surface. There is no way to find out which element faces are on surface
	if (NodeIndices.size() == 8) {
		return facesInd;
	}

	int i = 0, k = 0;
	while (i < numFaces) {
		if (NodeIndices.count(0) && NodeIndices.count(1) && NodeIndices.count(2) && NodeIndices.count(3))
			facesInd.push_back(0), i++;
		if (NodeIndices.count(4) && NodeIndices.count(5) && NodeIndices.count(6) && NodeIndices.count(7))
			facesInd.push_back(1), i++;
		if (NodeIndices.count(0) && NodeIndices.count(1) && NodeIndices.count(4) && NodeIndices.count(5))
			facesInd.push_back(2), i++;
		if (NodeIndices.count(1) && NodeIndices.count(2) && NodeIndices.count(5) && NodeIndices.count(6))
			facesInd.push_back(3), i++;
		if (NodeIndices.count(2) && NodeIndices.count(3) && NodeIndices.count(6) && NodeIndices.count(7))
			facesInd.push_back(4), i++;
		if (NodeIndices.count(3) && NodeIndices.count(0) && NodeIndices.count(7) && NodeIndices.count(4))
			facesInd.push_back(5), i++;
		if (k++ > numFaces) {
			// No faces found
			return facesInd;
		}
	}

	return facesInd;
}

int GetFaceIndex(CMesh::ELEMENT_TYPE ElemType, const set<int> &NodeIndices)
{
	// Face indices taken from abaqus manual 22.1.4 Three-dimensional solid element library
	switch (ElemType)
	{
	case CMesh::HEX:
		if (NodeIndices.count(0) && NodeIndices.count(1) && NodeIndices.count(2) && NodeIndices.count(3))
			return 0;
		if (NodeIndices.count(4) && NodeIndices.count(5) && NodeIndices.count(6) && NodeIndices.count(7))
			return 1;
		if (NodeIndices.count(0) && NodeIndices.count(1) && NodeIndices.count(4) && NodeIndices.count(5))
			return 2;
		if (NodeIndices.count(1) && NodeIndices.count(2) && NodeIndices.count(5) && NodeIndices.count(6))
			return 3;
		if (NodeIndices.count(2) && NodeIndices.count(3) && NodeIndices.count(6) && NodeIndices.count(7))
			return 4;
		if (NodeIndices.count(3) && NodeIndices.count(0) && NodeIndices.count(7) && NodeIndices.count(4))
			return 5;
		break;
	}
	assert(false);
	return -1;
}

set<int> GetCommonIndices(const vector<int> &SurfIndices, const vector<int> &VolIndices)
{
	set<int> Common;
	vector<int>::const_iterator itSurf;
	vector<int>::const_iterator itVol;
	int i;
	/*for (itSurf = SurfIndices.begin(); itSurf != SurfIndices.end(); ++itSurf)
	{
		for (itVol = VolIndices.begin(), i=0; itVol != VolIndices.end(); ++itVol, ++i)
		{
			if (*itSurf == *itVol)
				Common.insert(i);
			{
			}
		}
	}*/
	//return Common;
	vector<int>::const_iterator maxValue = max_element(VolIndices.begin(), VolIndices.end());
	for (itSurf = SurfIndices.begin(); itSurf != SurfIndices.end(); ++itSurf)
	{

		vector<int>::const_iterator it = find(VolIndices.begin(), VolIndices.end(), *itSurf);
		if (it == VolIndices.end())
		{
			//check whether the last element or just that find could not find *itSurf in VolIndices
			if (*it > *maxValue)
				continue;
			else
				i = distance(VolIndices.begin(), it);
		}
		else
		{
			i = distance(VolIndices.begin(), it);
		}
		Common.insert(i);
	}
	return Common;
}

COctreeVoxelMesh::COctreeVoxelMesh(string Type)
:CVoxelMesh(Type)
{
}

COctreeVoxelMesh::~COctreeVoxelMesh(void)
{
	p4est_destroy (p4est);
	TGLOG("P4est object destroyed");
	p4est_connectivity_destroy (conn);
	TGLOG("Connectivity destoyed");
	//m_ElementsInfo.clear();
	
	//Centre
}

// Boundaries are in format:
int COctreeVoxelMesh::isBoundary(double point[3])
{
	if ( my_comparison(point[0], m_DomainAABB.first.x) || my_comparison(point[0], m_DomainAABB.second.x)) {
		return 1;
	} else {
		if ( my_comparison(point[1], m_DomainAABB.first.y) || my_comparison(point[1], m_DomainAABB.second.y)) {
			return 1;
		} else {
			if ( my_comparison(point[2], m_DomainAABB.first.z) || my_comparison(point[2], m_DomainAABB.second.z)) {
				return 1;
			} else {
				return 0;
			}
		}
	}
}



// This node is hanging and DOES NOT have a proper number, the master nodes should be stored
// Number of node which is hanging is hanging_corner[i]
int COctreeVoxelMesh::storeHangingNode(int *all_lni, int *hanging_corner, int node_i, int hanging_count)
{
	int c = hanging_corner[node_i];      /* Child id of quadrant. */
	int ncontrib = corner_num_hanging[node_i ^ c];
	const int *contrib_corner = corner_to_hanging[node_i ^ c];
	vector<int> master_nodes;

	for (int j = 0; j < ncontrib; ++j) {
		int h = contrib_corner[j]^c;
		master_nodes.push_back(all_lni[h] + 1);
	}

	m_NodeConstraints.insert(make_pair(hanging_count, master_nodes));
    return 0;
}

void COctreeVoxelMesh::OutputPeriodicBoundaries(ostream &Output, CTextile& Textile, int iBoundaryConditions, bool bMatrixOnly)
{
	Output << "*EQUATION" << endl;
	map<int, vector<int>>::iterator itConstraints;
	for (itConstraints = m_NodeConstraints.begin(); itConstraints != m_NodeConstraints.end(); itConstraints++) {
		for (int i = 0; i < 3; i++) { // Write for 3 DoFs
			int num = (int)itConstraints->second.size();
			Output << num + 1 << endl;
			Output << itConstraints->first << ", " << i + 1 << ", 1, ";
			for (int j = 0; j < num; ++j) {
				Output << itConstraints->second[j] << ", " << i + 1 << ", " << -1.0/num;
				if (j == 2) {
					Output << endl;
				} else {
					if ( j < num - 1 ) {
						Output << ", ";
					}
				}
			}
			Output << endl;
		}
	}

	m_PeriodicBoundaries->SetDomainSize( Textile.GetDomain()->GetMesh() );
	vector<int> FaceA, FaceB, FaceC, FaceD, FaceE, FaceF;
	vector<int> Edge1, Edge2, Edge3, Edge4, Edge5, Edge6, Edge7, Edge8, Edge9, Edge10, Edge11, Edge12;
	int vertices[8];
	double x,y,z;

	// Sort points by coordinate
	sort(m_boundaryPoints.begin(), m_boundaryPoints.end());
	double x_min = m_DomainAABB.first.x;
	double x_max = m_DomainAABB.second.x;
	double y_min = m_DomainAABB.first.y;
	double y_max = m_DomainAABB.second.y;
	double z_min = m_DomainAABB.first.z;
	double z_max = m_DomainAABB.second.z;

	// The code checks if the point belongs to a vertex, edge or face
	for (int i = 0; i < m_boundaryPoints.size(); i++) {
		x = (double)m_boundaryPoints[i].x;
		y = (double)m_boundaryPoints[i].y;
		z = (double)m_boundaryPoints[i].z;
		if		  ( my_comparison(x, x_min) && my_comparison(y, y_min) && my_comparison(z, z_min) ) {
			vertices[0] = m_boundaryPoints[i].nodeNum;
		} else if ( my_comparison(x, x_max) && my_comparison(y, y_min) && my_comparison(z, z_min) ) {
			vertices[1] = m_boundaryPoints[i].nodeNum;
		} else if ( my_comparison(x, x_max) && my_comparison(y, y_max) && my_comparison(z, z_min) ) {
			vertices[2] = m_boundaryPoints[i].nodeNum;
		} else if ( my_comparison(x, x_min) && my_comparison(y, y_max) && my_comparison(z, z_min) ) {
			vertices[3] = m_boundaryPoints[i].nodeNum;
		} else if ( my_comparison(x, x_min) && my_comparison(y, y_min) && my_comparison(z, z_max) ) {
			vertices[4] = m_boundaryPoints[i].nodeNum;
		} else if ( my_comparison(x, x_max) && my_comparison(y, y_min) && my_comparison(z, z_max) ) {
			vertices[5] = m_boundaryPoints[i].nodeNum;
		} else if ( my_comparison(x, x_max) && my_comparison(y, y_max) && my_comparison(z, z_max) ) {
			vertices[6] = m_boundaryPoints[i].nodeNum;
		} else if ( my_comparison(x, x_min) && my_comparison(y, y_max) && my_comparison(z, z_max) ) {
			vertices[7] = m_boundaryPoints[i].nodeNum;
		} else if ( my_comparison(x, x_min) && my_comparison(y, y_min) ) {
			Edge1.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(x, x_max) && my_comparison(y, y_min) ) {
			Edge2.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(x, x_max) && my_comparison(y, y_max) ) {
			Edge3.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(x, x_min) && my_comparison(y, y_max) ) {
			Edge4.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(x, x_min) && my_comparison(z, z_min) ) {
			Edge5.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(x, x_max) && my_comparison(z, z_min) ) {
			Edge6.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(x, x_max) && my_comparison(z, z_max) ) {
			Edge7.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(x, x_min) && my_comparison(z, z_max) ) {
			Edge8.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(y, y_min) && my_comparison(z, z_min) ) {
			Edge9.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(y, y_max) && my_comparison(z, z_min) ) {
			Edge10.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(y, y_max) && my_comparison(z, z_max) ) {
			Edge11.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(y, y_min) && my_comparison(z, z_max) ) {
			Edge12.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(x, x_max) ) {
			FaceA.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(x, x_min) ) {
			FaceB.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(y, y_max) ) {
			FaceC.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(y, y_min) ) {
			FaceD.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(z, z_max) ) {
			FaceE.push_back(m_boundaryPoints[i].nodeNum);
		} else if ( my_comparison(z, z_min) ) {
			FaceF.push_back(m_boundaryPoints[i].nodeNum);
		}
	}

	m_PeriodicBoundaries->SetFaceA( FaceA, FaceB );
	m_PeriodicBoundaries->SetFaceB( FaceC, FaceD );
	m_PeriodicBoundaries->SetFaceC( FaceE, FaceF );

	m_PeriodicBoundaries->SetEdges( Edge1 );
	m_PeriodicBoundaries->SetEdges( Edge2 );
	m_PeriodicBoundaries->SetEdges( Edge3 );
	m_PeriodicBoundaries->SetEdges( Edge4 );
	m_PeriodicBoundaries->SetEdges( Edge5 );
	m_PeriodicBoundaries->SetEdges( Edge6 );
	m_PeriodicBoundaries->SetEdges( Edge7 );
	m_PeriodicBoundaries->SetEdges( Edge8 );
	m_PeriodicBoundaries->SetEdges( Edge9 );
	m_PeriodicBoundaries->SetEdges( Edge10 );
	m_PeriodicBoundaries->SetEdges( Edge11 );
	m_PeriodicBoundaries->SetEdges( Edge12 );

	for (int i = 0; i < 8; i++) {
		m_PeriodicBoundaries->SetVertex( vertices[i] );
	}

	m_PeriodicBoundaries->CreatePeriodicBoundaries( Output, (int)AllNodes.size() + 1, Textile, iBoundaryConditions, bMatrixOnly );
}

//ostream &Output, bool bOutputMatrix, bool bOutputYarn, bool bAbaqus = true
int COctreeVoxelMesh::OutputHexElements(ostream &Output, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, bool bAbaqus )
{
	CTimer timer;
	timer.start("Writing elements");
	vector<vector<int>>::iterator itElements;
	vector<int>::iterator itNodes;
	int i, elem_count = 1;
	for (itElements = m_AllElements.begin(); itElements != m_AllElements.end(); itElements++) {
		Output << elem_count ++ << ", ";
		for (itNodes = itElements->begin(), i = 1; itNodes != itElements->end(); itNodes++, i++) {
			Output << *itNodes;
			if (i < 8) {
				Output << ", ";
			}
		}
		Output << endl;
	}

	timer.check("Elements written");
	timer.stop();

	if ( m_bCohesive ) {
		timer.start("Writing surfaces");
		map<int, vector<int>>::iterator itSurfaceNodes;
		for (itSurfaceNodes = m_SurfaceNodes.begin(); itSurfaceNodes != m_SurfaceNodes.end(); ++itSurfaceNodes) {
			if ( itSurfaceNodes->first == -1) {
				Output << "*NSET, NSET=SURFACE-NODES-MATRIX" << endl;
			} else {
				Output << "*NSET, NSET=SURFACE-NODES-YARN" << itSurfaceNodes->first << endl;
			}
			WriteValues(Output, itSurfaceNodes->second, 16);
		}

		map<int, vector< pair<int,int> > >::iterator itSurfaceFaces;
		for (itSurfaceFaces = m_SurfaceElementFaces.begin(); itSurfaceFaces != m_SurfaceElementFaces.end(); ++itSurfaceFaces) {
			if (itSurfaceFaces->first == -1) {
				Output << "*SURFACE, NAME=SURFACE-MATRIX" << endl;
			} else {
				Output << "*SURFACE, NAME=SURFACE-YARN" << itSurfaceFaces->first << endl;
			}
			vector< pair<int, int> >::iterator itFaces;
			for (itFaces = itSurfaceFaces->second.begin(); itFaces != itSurfaceFaces->second.end(); ++itFaces) {
				Output << itFaces->first << ", S" << itFaces->second << endl;
			}
		}

		timer.check("Surfaces written");
		timer.stop();
	}

	return elem_count;
}

// Uniform refinement
int COctreeVoxelMesh::refine_fn_uni(p4est_t * p4est, p4est_topidx_t which_tree, p4est_quadrant_t * quadrant)
{
	return 1;
}

void COctreeVoxelMesh::FindLocMinMax( int& XMin, int& XMax, int& YMin, int& YMax, XYZ& Min, XYZ& Max )
{
	double x_dist = (g_DomainAABB.second.x - g_DomainAABB.first.x)/pow(2, static_cast<double>(max_level));
	double y_dist = (g_DomainAABB.second.y - g_DomainAABB.first.y)/pow(2, static_cast<double>(max_level));
	XMin = (int)floor((Min.x - g_DomainAABB.first.x)/ x_dist);
	XMax = (int)ceil((Max.x - g_DomainAABB.first.x) / x_dist);
	YMin = (int)floor((Min.y - g_DomainAABB.first.y) / y_dist);
	YMax = (int)ceil((Max.y - g_DomainAABB.first.y) / y_dist);
}

// TODO: Ensure that the mesh is periodic in X-Y-Z directions
// Refinement only if at least two dissimilar materials are within an element
int COctreeVoxelMesh::refine_fn_periodic(p4est_t * p4est, p4est_topidx_t which_tree, p4est_quadrant_t * quadrant)
{
	vector<XYZ> CornerPoints;
	vector<POINT_INFO> CornerInfo;
	XYZ Point;
	int refine = 0;
	p4est_quadrant_t node_quadrant;
	XYZ Min, Max;
	///double x_min, x_max, y_min, y_max, z_min, z_max;
	double vxyz[3];

	for (int node_i=0; node_i < 8; node_i++) {
		p4est_quadrant_corner_node(quadrant, node_i, &node_quadrant);
		p4est_qcoord_to_vertex (p4est->connectivity, which_tree, node_quadrant.x, node_quadrant.y, node_quadrant.z, vxyz);
		Point.x = vxyz[0];
		Point.y = vxyz[1];
		Point.z = vxyz[2];

		if (node_i == 0) {
			Min.x = Max.x = Point.x;
			Min.y = Max.y = Point.y;
			Min.z = Max.z = Point.z;
		} else {
			if (Point.x < Min.x)
				Min.x = Point.x;
			if (Point.x > Max.x)
				Max.x = Point.x;
			if (Point.y < Min.y)
				Min.y = Point.y;
			if (Point.y > Max.y)
				Max.y = Point.y;
			if (Point.z < Max.z)
				Max.z = Point.z;
			if (Point.z > Max.z)
				Max.z = Point.z;
		}
	}

	int XMin, XMax, YMin, YMax;
	FindLocMinMax( XMin, XMax, YMin, YMax, Min, Max );

	if (my_comparison(Min.z, g_DomainAABB.first.z) && refine == 1) {
		//TGLOG("MIN: z_min " << z_min << ", "<<x_max <<", " << y_min);
		//TGLOG("Xmin, Xmax = " << loc_x_min << ", " << loc_x_max);
		//TGLOG("ymin, ymax = " << loc_y_min << ", " << loc_y_max);
		for (int i = XMin; i < XMax; i++) {
			for (int j = YMin; j < YMax; j++) {
				//TGLOG("Refinement +1");
				FaceZ_min[i][j] += 1;
				//TGLOG(".");
			}
		}
	}

	if (my_comparison(Max.z, g_DomainAABB.second.z) && refine == 1) {

		//TGLOG("MAX: z_min " << z_max << ", "<<x_max <<", " << y_min);
		//TGLOG("MAX: Xmin, Xmax = " << loc_x_min << ", " << loc_x_max);
		//TGLOG("MAX: ymin, ymax = " << loc_y_min << ", " << loc_y_max);
		for (int i = XMin; i < XMax; i++) {
			for (int j = YMin; j < YMax; j++) {
				FaceZ_max[i][j] += 1;
			}
		}
	}

	if (my_comparison(Max.z, g_DomainAABB.second.z)) {

		//TGLOG("MAX: z_min " << z_max << ", "<<x_max <<", " << y_min);
		//TGLOG("MAX: Xmin, Xmax = " << loc_x_min << ", " << loc_x_max);
		//TGLOG("MAX: ymin, ymax = " << loc_y_min << ", " << loc_y_max);
		for (int i = XMin; i < XMax; i++) {
			for (int j = YMin; j < YMax; j++) {
				if (FaceZ_max[i][j] < FaceZ_min[i][j]) {
					//TGLOG("Periodic refinement enacted!");
					refine = 1;
					FaceZ_max[i][j] += 1;
				}
			}
		}
	}

	if (my_comparison(Min.z, g_DomainAABB.first.z)) {

		//TGLOG("MIN: z_min " << z_min << ", "<<x_max <<", " << y_min);
		//TGLOG("MAX: Xmin, Xmax = " << loc_x_min << ", " << loc_x_max);
		//TGLOG("MAX: ymin, ymax = " << loc_y_min << ", " << loc_y_max);
		for (int i = XMin; i < XMax; i++) {
			for (int j = YMin; j < YMax; j++) {
				if (FaceZ_max[i][j] > FaceZ_min[i][j]) {
					//TGLOG("Periodic refinement enacted!");
					refine = 1;
					FaceZ_min[i][j] += 1;
				}
			}
		}
	}

	return refine;
}

// Refinement only if at least two dissimilar materials are within an element
int COctreeVoxelMesh::refine_fn(p4est_t * p4est, p4est_topidx_t which_tree, p4est_quadrant_t * quadrant)
{
	vector<XYZ> CornerPoints;
	vector<POINT_INFO> CornerInfo;
	XYZ Point;
	int refine = 0;
	p4est_quadrant_t node_quadrant;
	double x_min, x_max, y_min, y_max, z_min, z_max;
	double vxyz[3];
	double cx, cy, cz;
	cx = cy = cz = 0;
	int trig = 0;

	// We do not want to refine deeper than a given maximum level.
	if (quadrant->level > max_level - 1) {
		return 0;
	}

	if (quadrant->level <2) {
		return 1;
	}

	for (int node_i=0; node_i < 8; node_i++) {
		p4est_quadrant_corner_node(quadrant, node_i, &node_quadrant);
		p4est_qcoord_to_vertex (p4est->connectivity, which_tree, node_quadrant.x, node_quadrant.y, node_quadrant.z, vxyz);
		Point.x = vxyz[0];
		Point.y = vxyz[1];
		Point.z = vxyz[2];
		cx += 1.0/8.0 *vxyz[0];
		cy += 1.0/8.0 *vxyz[1];
		cz += 1.0/8.0 *vxyz[2];

		CornerPoints.push_back(Point);

		if (node_i == 0) {
			x_min = Point.x; x_max = Point.x;
			y_min = Point.y; y_max = Point.y;
			z_min = Point.z; z_max = Point.z;
		} else {
			if (Point.x < x_min)
				x_min = Point.x;
			if (Point.x > x_max)
				x_max = Point.x;
			if (Point.y < y_min)
				y_min = Point.y;
			if (Point.y > y_max)
				y_max = Point.y;
			if (Point.z < z_min)
				z_min = Point.z;
			if (Point.z > z_max)
				z_max = Point.z;
		}
	}

	double coef = 1.01;
	vector<XYZ> ExtraPoints;
	for (int i = 0; i < 8; i++) {
		XYZ NewPoint;
		NewPoint.x = (CornerPoints[i].x - cx)*coef + cx;
		NewPoint.y = (CornerPoints[i].y - cy)*coef + cy;
		NewPoint.z = (CornerPoints[i].z - cz)*coef + cz;
		ExtraPoints.push_back(NewPoint);
	}
	ExtraPoints.push_back(XYZ(cx, cy, cz));
	CornerPoints.push_back(XYZ(cx, cy, cz));

	if (getPointsInfo(CornerPoints, max_level ) == 1 || getPointsInfo(ExtraPoints, max_level) == 1 )
		return 1;

	return 0;
}

// After main refinement is done we want to ensure that there are no hanging nodes at the surface
int COctreeVoxelMesh::refine_fn_post(p4est_t * p4est, p4est_topidx_t which_tree, p4est_quadrant_t * quadrant)
{
	vector<XYZ> CornerPoints;
	vector<XYZ> ExtraPoints;
	vector<POINT_INFO> ExtraInfo;
	XYZ Point;
	p4est_quadrant_t node_quadrant;
	double vxyz[3];
	XYZ CentrePoint;
	CentrePoint.x = 0;
	CentrePoint.y = 0;
	CentrePoint.z = 0;

	// We do not want to refine deeper than a given maximum level.
	if (quadrant->level > max_level -1) {
		return 0;
	}


	//int trig = 0;
	for (int i = 0; i < 8; i++) {
		p4est_quadrant_corner_node(quadrant, i, &node_quadrant);
		p4est_qcoord_to_vertex (p4est->connectivity, which_tree, node_quadrant.x, node_quadrant.y, node_quadrant.z, vxyz);
		Point.x = vxyz[0];
		Point.y = vxyz[1];
		Point.z = vxyz[2];

		CentrePoint.x += vxyz[0] * 1.0/8.0;
		CentrePoint.y += vxyz[1] * 1.0/8.0;
		CentrePoint.z += vxyz[2] * 1.0/8.0;

		CornerPoints.push_back(Point);
	}

	ExtraPoints.push_back(CentrePoint);
	double coef = 2.01;
	for (int i = 0; i < 8; i++) {
		XYZ NewPoint;
		//NewPoint.x = (CornerPoints[i].x - CentrePoint.x)*coef + CentrePoint.x;
		//NewPoint.y = (CornerPoints[i].y - CentrePoint.y)*coef + CentrePoint.y;
		//NewPoint.z = (CornerPoints[i].z - CentrePoint.z)*coef + CentrePoint.z;
		NewPoint = (CornerPoints[i] - CentrePoint)*coef + CentrePoint;
		ExtraPoints.push_back(NewPoint);

		NewPoint.x = (CornerPoints[i].x - CentrePoint.x)*coef + CentrePoint.x;
		NewPoint.y = CentrePoint.y;
		NewPoint.z = CentrePoint.z;
		ExtraPoints.push_back(NewPoint);

		NewPoint.x = CentrePoint.x;
		NewPoint.y = (CornerPoints[i].y - CentrePoint.y)*coef + CentrePoint.y;
		NewPoint.z = CentrePoint.z;
		ExtraPoints.push_back(NewPoint);

		NewPoint.x = CentrePoint.x;
		NewPoint.y = CentrePoint.y;
		NewPoint.z = (CornerPoints[i].z - CentrePoint.z)*coef + CentrePoint.z;
		ExtraPoints.push_back(NewPoint);
	}

	coef = 1.25;
	for (int i = 0; i < 8; i++) {
		XYZ NewPoint;
		NewPoint.x = (CornerPoints[i].x - CentrePoint.x)*coef + CentrePoint.x;
		NewPoint.y = (CornerPoints[i].y - CentrePoint.y)*coef + CentrePoint.y;
		NewPoint.z = (CornerPoints[i].z - CentrePoint.z)*coef + CentrePoint.z;
		ExtraPoints.push_back(NewPoint);

		NewPoint.x = (CornerPoints[i].x - CentrePoint.x)*coef + CentrePoint.x;
		NewPoint.y = CentrePoint.y;
		NewPoint.z = CentrePoint.z;
		ExtraPoints.push_back(NewPoint);

		NewPoint.x = CentrePoint.x;
		NewPoint.y = (CornerPoints[i].y - CentrePoint.y)*coef + CentrePoint.y;
		NewPoint.z = CentrePoint.z;
		ExtraPoints.push_back(NewPoint);

		NewPoint.x = CentrePoint.x;
		NewPoint.y = CentrePoint.y;
		NewPoint.z = (CornerPoints[i].z - CentrePoint.z)*coef + CentrePoint.z;
		ExtraPoints.push_back(NewPoint);
	}

	if (getPointsInfo(ExtraPoints, max_level) == 1)
		return 1;

	return 0;
}

// Return 1 is at least one of the points is not the same material as others
// Return 0 is all the points are from the same materials
int COctreeVoxelMesh::getPointsInfo(vector<XYZ> myPoints, int refineLevel)
{
	double x0 = g_DomainAABB.first.x;
	double y0 = g_DomainAABB.first.y;
	double z0 = g_DomainAABB.first.z;
	double x_length = g_DomainAABB.second.x - x0;
	double y_length = g_DomainAABB.second.y - y0;
	double z_length = g_DomainAABB.second.z - z0;

	double dx = x_length / pow(2, static_cast<double>(refineLevel));
	double dy = y_length / pow(2, static_cast<double>(refineLevel));
	double dz = z_length / pow(2, static_cast<double>(refineLevel));
	int num = pow(2, static_cast<double>(refineLevel)) + 1;

	int row_len = num;
	int layer_len = num * num;

	vector<XYZ>::const_iterator itPoint;
	int previousMaterial;
	int i = 0;

	for (itPoint = myPoints.begin(); itPoint != myPoints.end(); ++itPoint)
	{
		double x = itPoint->x;
		double y = itPoint->y;
		double z = itPoint->z;

		if ( x > x0 + x_length )
			x -= x_length;

		if ( x < x0 )
			x += x_length;

		if ( y > y0 + y_length )
			y -= y_length;

		if ( y < y0 )
			y += y_length;

		if ( z > z0 + z_length )
			z -= z_length;

		if ( z < z0 )
			z += z_length;

		int index_i = ( (x - x0)/ dx - floor((x - x0) / dx) >= 0.5 ) ? ceil((x - x0) / dx) : floor((x - x0) / dx);
		int index_j = ( (y - y0)/ dy - floor((y - y0) / dy) >= 0.5 ) ? ceil((y - y0) / dy) : floor((y - y0) / dy);
		int index_k = ( (z - z0)/ dz - floor((z - z0) / dz) >= 0.5 ) ? ceil((z - z0) / dz) : floor((z - z0) / dz);

		if ( index_i + row_len * index_j + layer_len * index_k > num * num * num || index_i + row_len * index_j + layer_len * index_k < 0)
		{
			TGERROR("Something is not right - index is outside of stored info");
			TGERROR("X, Y, Z: " << x << ", " << y << ", " << z);
			return 0;
		}

		//TGLOG("X: " << x << "(" << index_i << "), Y: " << y << "(" << index_j << "), Z: " << z << "(" << index_k << ")");

		if (i == 0)
			previousMaterial = materialInfo[index_i + row_len * index_j + layer_len * index_k];

		if (i > 0 && previousMaterial != materialInfo[index_i + row_len * index_j + layer_len * index_k] )
			return 1;

		i++;
	}

	return 0;
}

void COctreeVoxelMesh::storePointInfo(int refineLevel)
{
	vector<XYZ> myPoints;
	vector<POINT_INFO> temp;
	double x0 = g_DomainAABB.first.x;
	double y0 = g_DomainAABB.first.y;
	double z0 = g_DomainAABB.first.z;
	double x_length = g_DomainAABB.second.x - x0;
	double y_length = g_DomainAABB.second.y - y0;
	double z_length = g_DomainAABB.second.z - z0;

	double dx = x_length / pow(2, static_cast<double>(refineLevel));
	double dy = y_length / pow(2, static_cast<double>(refineLevel));
	double dz = z_length / pow(2, static_cast<double>(refineLevel));

	int num = pow(2, static_cast<double>(refineLevel)) + 1;

	for (int k = 0; k < num; k++)
		for (int j = 0; j < num; j++)
			for (int i = 0; i < num; i++)
				myPoints.push_back(XYZ(x0 + dx*i, y0 + dy*j, z0 + dz*k));

	temp.clear();
	gTextile.GetPointInformation(myPoints,temp);

	vector<POINT_INFO>::const_iterator itInfo;

	materialInfo.clear();
	TGLOG("Infos " << myPoints.size());

	for (itInfo = temp.begin(); itInfo != temp.end(); ++itInfo)
		materialInfo.push_back(itInfo->iYarnIndex);

	TGLOG("Info stored. Elements = " << materialInfo.size());
	temp.clear();
}



void COctreeVoxelMesh::fillMaterialInfo() {
	vector <XYZ> myPoints;
	for (int i = 0; i < CentrePoints.size(); i++) {
		myPoints.push_back(CentrePoints[i]);
		for (int j = 0; j < 8; j++) {
			myPoints.push_back(cornerPoints[i*8 + j]);
		}
	}
	vector<POINT_INFO> myInfo;
	gTextile.GetPointInformation(myPoints, myInfo);

	POINT_INFO info;
	for (int i = 0; i < CentrePoints.size(); i++) {
		vector<int> v;
		for (int j = 0; j < 9; j++) {
			v.push_back(myInfo[i*9 + j].iYarnIndex);
		}
		pair<int, int> mat = most_common(v);
		vector<int>::iterator it = find(v.begin(), v.end(), mat.first);
		int pos = distance(v.begin(), it);
		v.clear();
		//m_ElementsInfo.push_back(myInfo[i*9 + pos]);
		m_ElementsInfo.push_back(myInfo[i*9 + 0 * pos]);
	}
}

int COctreeVoxelMesh::CreateP4ESTRefinement(int min_level, int refine_level)
{
	// The MPI is not included in TexGen - initialise dummy mpi objects for purpose of P4EST
	int mpiret = sc_MPI_Init (NULL, NULL);
	SC_CHECK_MPI (mpiret);
	sc_MPI_Comm mpicomm = sc_MPI_COMM_WORLD;

	//conn = p8est_connectivity_new_unitcube ();
	//p4est = p4est_new (mpicomm, conn, 0, NULL, NULL);

	int len = pow(2, static_cast<double>(max_level));
	for (int i = 0; i < len + 1; i++) {
		vector<int> temp(len, 0);
		FaceX_min.push_back(temp);
		FaceX_max.push_back(temp);
		FaceY_min.push_back(temp);
		FaceY_max.push_back(temp);
		FaceZ_min.push_back(temp);
		FaceZ_max.push_back(temp);
	}

	if (writeTempFile("temp_octree.inp", m_DomainAABB) == -1) {
		return -1;
	}

	storePointInfo(max_level);

	TGLOG("Stored");

	// Create a forest from the inp file
	conn = p4est_connectivity_read_inp ("temp_octree.inp");
	if (conn == NULL) {
		TGERROR("Failed to read a valid connectivity from temp_octree.inp");
		return -1;
	}
	// Create a forest that is not refined; it consists of the root octant.
	p4est = p4est_new (mpicomm, conn, 0, NULL, NULL);

	/* Comment from P4EST: Refine the forest iteratively, load balancing at each iteration.
	* This is important when starting with an unrefined forest */
	// Refine all elements to min_level
	for (int level = 0; level < min_level; ++level) {
		p4est_refine (p4est, 0, &TexGen::COctreeVoxelMesh::refine_fn_uni, NULL);
		p4est_partition (p4est, 0, NULL);
	}

	// Refine elements which have multiple materials within them
	for (int level = min_level; level < refine_level; ++level) {
		p4est_refine (p4est,1, refine_fn, NULL);
		p4est_partition (p4est, 0, NULL);
	}

	// P4EST_CONNECT_FULL is used for 2:1 balancing across all faces, edges and corners
	p4est_balance (p4est, P4EST_CONNECT_FULL, NULL);
	p4est_partition (p4est, 0, NULL);
	TGLOG("Post-refinement now");

	// Post refinement is needed to have the boundaries of the inclusions to be represented by the smallest refinement only

	for (int i = 0; i < 3; i++) {
		p4est_refine (p4est, 0, refine_fn_post, NULL);
		//p4est_refine (p4est, 1, refine_fn_periodic, NULL);
		p4est_partition (p4est, 0, NULL);
		p4est_balance (p4est, P4EST_CONNECT_FULL, NULL);
		p4est_partition (p4est, 0, NULL);
	}

	materialInfo.clear();

	return 0;
}

void COctreeVoxelMesh::SaveVoxelMesh(CTextile &Textile, string OutputFilename, int min_level, int refine_level, bool smoothing, int iter, double s1, double s2, bool surfaceOutput, bool cohesive)
{
	CTimer timer;
	max_level = refine_level;
	m_bSmooth = smoothing;
	m_smoothIter = iter;
	m_smoothCoef1 = s1;
	m_smoothCoef2 = s2;
	m_bSurface = surfaceOutput;
	m_bCohesive = cohesive;
	gTextile = Textile;
	m_DomainAABB = Textile.GetDomain()->GetMesh().GetAABB();
	g_DomainAABB = m_DomainAABB;

	if (min_level < 0 || refine_level < 0 || min_level > refine_level) {
		TGERROR("Incorrect refinement levels specified. min_level should be lower than refine_level");
		return;
	}

	if ( m_bSmooth) {
		if (!( ((s1 > 0 && s1 == s2) || (s1 > 0 && s2 < 0 && s1 < -s2)) && m_smoothIter > 0)) {
			TGERROR("Smoothing coefficients are not correct. It should be:\n1. Coef1 = Coef2 - for Laplacian smoothing\n2. Coef1 < -Coef2 (and Coef1 > 0) - for non-shrinking smoothing\nIter > 0");
			return;
		}
	}

	timer.start("Starting octree refinement...");
	if (CreateP4ESTRefinement(min_level, refine_level) == -1)
		return;

	//G - saving smooth voxel mesh with no periodic boundary conditions
	//CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum, bool bOutputMatrix, bool bOutputYarns, bool surfaceOutput, int iBoundaryConditions, int iElementType = 0
	CVoxelMesh::SaveVoxelMesh(Textile, OutputFilename, 1, 1, 1, true, true, true, 5, 1);


	timer.check("Octree refinement finished");
	timer.stop();
}

bool COctreeVoxelMesh::CalculateVoxelSizes(CTextile &Textile)
{
	return true;
}

void COctreeVoxelMesh::ConvertOctreeToNodes()
{
	p4est_ghost_t      *ghost;
	p4est_lnodes_t     *lnodes;
	// Create the ghost layer to learn about parallel neighbors.
	ghost = p4est_ghost_new (p4est, P4EST_CONNECT_FULL);
	// Create a node numbering for continuous linear finite elements.
	lnodes = p4est_lnodes_new (p4est, ghost, 1 );
	// Destroy the ghost structure -- no longer needed after node creation.
	p4est_ghost_destroy (ghost);
	ghost = NULL;
	CentrePoints.clear();

	// Assign independent nodes for hanging nodes (this piece is copied from one of the examples provided with p4est)
	corner_to_hanging[0] = &zero;
	corner_to_hanging[1] = p8est_edge_corners[0];
	corner_to_hanging[2] = p8est_edge_corners[4];
	corner_to_hanging[3] = p8est_face_corners[4];
	corner_to_hanging[4] = p8est_edge_corners[8];
	corner_to_hanging[ones - 2] = p4est_face_corners[2];
	corner_to_hanging[ones - 1] = p4est_face_corners[0];
	corner_to_hanging[ones] = &ones;

	int                 i, k, node_i, q, Q, used, anyhang, hanging_corner[P4EST_CHILDREN], node_elements[8], hang_nums[8];
	int elem_order[8] = {0, 2, 3, 1, 4, 6, 7, 5}; // That is how elements should be ordered in abaqus
	sc_array_t         *tquadrants;
	p4est_topidx_t      tt;
	p4est_locidx_t      all_lni[P4EST_CHILDREN];
	p4est_tree_t       *tree;
	p4est_quadrant_t   *quad, node_quadrant; //,node;

	double vxyz[3];
	int node_count = 0;
	int hanging_count = 2000000; // Large number for node offest

	// The mesh is stored as a tree and all of the last 8 hanging nodes belong to one
	// parent element. Therefore, it is enough to store last 8 elements to eliminate duplicates
	double hang_coord[8][3];

	int ElemCount = 1;
	// Loop over all the trees
	for (tt = p4est->first_local_tree, k = 0; tt <= p4est->last_local_tree; ++tt) {
		tree = p4est_tree_array_index (p4est->trees, tt);
		tquadrants = &tree->quadrants;
		Q = (p4est_locidx_t) tquadrants->elem_count;
		// loop over all the quadrant in the tree
		for (q = 0; q < Q; ++q, ++k) {
			XYZ CurrentCentre;
			// Extract an element by index
			quad = p4est_quadrant_array_index (tquadrants, q);
			// Get the numbers of the corners nodes
			for (i = 0; i < P4EST_CHILDREN; ++i) {
				all_lni[i] = lnodes->element_nodes[P4EST_CHILDREN * k + i];
			}

			// Figure out the hanging corners on this element, if any.
			anyhang = lnodes_decode2 (lnodes->face_code[k], hanging_corner);

			vector<int> elemNodes;
			// Loop through nodes
			for(node_i = 0; node_i < P4EST_CHILDREN; node_i++) {
				// Extract the coordinates
				p4est_quadrant_corner_node(quad, node_i, &node_quadrant);
				p4est_qcoord_to_vertex (p4est->connectivity, tt, node_quadrant.x, node_quadrant.y, node_quadrant.z, vxyz);

				// The node is not hanging and therefore has correct numbering
				if (!anyhang || hanging_corner[node_i] == -1) {
					if (node_count - 1 < lnodes->element_nodes[P4EST_CHILDREN * k + node_i]) {
						AllNodes.insert(make_pair(lnodes->element_nodes[P4EST_CHILDREN * k + node_i] + 1, XYZ(vxyz[0], vxyz[1], vxyz[2])));
						node_count++;

						if ( isBoundary(vxyz) ) {
							m_boundaryPoints.push_back(Point(lnodes->element_nodes[P4EST_CHILDREN * k + node_i] + 1, vxyz[0], vxyz[1], vxyz[2]));
						}
					}

					// Keep the information for the element connectivity
					node_elements[node_i] = lnodes->element_nodes[P4EST_CHILDREN * k + node_i] + 1;

				} else {
					// Check if that hanging corner has been already written
					used = duplicatedHangingNode(vxyz, hang_coord, hang_nums);

					if ( used > 0 ) {
						// The node has already appeared and therefore does not need to be written again, use it to create an element only
						node_elements[node_i] = used;
					} else {
						// The node has not appeared earlier, write it, form an element, store the node for further comparisons
						AllNodes.insert(make_pair(++hanging_count, XYZ(vxyz[0], vxyz[1], vxyz[2])));
						node_elements[node_i] = hanging_count;
						hang_coord[7][0] = vxyz[0];
						hang_coord[7][1] = vxyz[1];
						hang_coord[7][2] = vxyz[2];
						hang_nums[7] = hanging_count;

						// Write constraints for the hanging node
						storeHangingNode(all_lni, hanging_corner, node_i, hanging_count);
					}
				}
				CurrentCentre.x += 1.0/8.0 * vxyz[0];
				CurrentCentre.y += 1.0/8.0 * vxyz[1];
				CurrentCentre.z += 1.0/8.0 * vxyz[2];
				cornerPoints.push_back(XYZ(vxyz[0], vxyz[1], vxyz[2]));
			}

			for(i = 0; i < 8; i++) {
				elemNodes.push_back(node_elements[elem_order[i]]);
				m_NodesEncounter[node_elements[i]].push_back(ElemCount);
			}
			ElemCount++;
			CentrePoints.push_back(CurrentCentre);
			m_AllElements.push_back(elemNodes);

			// Create connectivity for the nodes in the element (only if it is the final level of the refinement)
			if ( quad->level == max_level ) {
				vector<int>::iterator itNodes;
				int i = 0;
				vector<int> temp;
				for (itNodes = elemNodes.begin(); itNodes != elemNodes.end(); ++itNodes, i++) {
					// No need to iterate through the elements which have this node as the loop will go through
					// these elements anyway. Only store the information available for this element - neighbouring nodes etc
					int a[3];
					switch(i) {
						case 0: { a[0] = elemNodes[1]; a[1] = elemNodes[3]; a[2] = elemNodes[4]; break; }
						case 1: { a[0] = elemNodes[0]; a[1] = elemNodes[2]; a[2] = elemNodes[5]; break; }
						case 2: { a[0] = elemNodes[1]; a[1] = elemNodes[3]; a[2] = elemNodes[6]; break; }
						case 3: { a[0] = elemNodes[0]; a[1] = elemNodes[2]; a[2] = elemNodes[7]; break; }
						case 4: { a[0] = elemNodes[0]; a[1] = elemNodes[5]; a[2] = elemNodes[7]; break; }
						case 5: { a[0] = elemNodes[1]; a[1] = elemNodes[4]; a[2] = elemNodes[6]; break; }
						case 6: { a[0] = elemNodes[2]; a[1] = elemNodes[5]; a[2] = elemNodes[7]; break; }
						case 7: { a[0] = elemNodes[3]; a[1] = elemNodes[4]; a[2] = elemNodes[6]; break; }
					}
					m_NeighbourNodes[*itNodes].push_back(a[0]);
					m_NeighbourNodes[*itNodes].push_back(a[1]);
					m_NeighbourNodes[*itNodes].push_back(a[2]);
				}
			}

			elemNodes.clear();
		}
	}
	p4est_lnodes_destroy (lnodes);
	TGLOG("Num of elements: " << m_AllElements.size());

}

void COctreeVoxelMesh::OutputNodes(ostream &Output, CTextile &Textile, bool surfaceOutput, bool bAbaqus )
{

	CTimer timer;
	//timer.start("Starting octree refinement");
	TGLOG("Converting octree to nodes coordinates");
	ConvertOctreeToNodes();
	TGLOG("Octree converted");
	//timer.stop();

	//timer.start("Retrieving info on centre points");
	TGLOG("Retrieving info on centre points");
	gTextile.GetPointInformation(CentrePoints, m_ElementsInfo);
	//fillMaterialInfo();
	//timer.check("Info retrieved");
	TGLOG("Info retrieved");
	//timer.stop();

	if (m_bSmooth || m_bSurface) {
		map<int, vector<int>> NodeSurf;
		vector<int> AllSurf;
		extractSurfaceNodeSets(NodeSurf, AllSurf);

		if (m_bSmooth) {
			//timer.start("Smoothing starts");
			TGLOG("Smoothing starts");
			smoothing(NodeSurf, AllSurf);
			//timer.check("Smoothing finished");
			TGLOG("Smoothing finished");
			//timer.stop();
		}

		if (m_bSurface) {
			//timer.start("Preparing interface definitions");
			TGLOG("Preparing interface definitions");
			OutputSurfaces(NodeSurf, AllSurf);
			//timer.check("Interfaces defined");
			TGLOG("Interfaces defined");
			//timer.stop();
		}
	}

	//timer.start("Write the nodes");
	TGLOG("Write the nodes");
	map<int,XYZ>::iterator itNodes;
	for (itNodes = AllNodes.begin(); itNodes != AllNodes.end(); ++itNodes) {
		Output << itNodes->first << ", " << itNodes->second.x << ", " << itNodes->second.y << ", " << itNodes->second.z << endl;
	}
	//timer.check("Nodes written");
	TGLOG("Nodes written");
	//timer.check("Octree refinement finished");
	//timer.stop();
}

int COctreeVoxelMesh::checkIndex(int currentElement, vector<int> nodes)
{
	vector<int> elems;

	vector<int>::const_iterator itNodes;
	for(itNodes = nodes.begin(); itNodes != nodes.end(); ++itNodes) {
		vector<int>::iterator itEnc;
		for (itEnc = m_NodesEncounter[*itNodes].begin(); itEnc != m_NodesEncounter[*itNodes].end(); ++itEnc)
			elems.push_back(*itEnc);
			//elems.insert(elems.end(), NodesEncounter[*itNodes].begin(), NodesEncounter[*itNodes].end());
	}


	for (itNodes = elems.begin(); itNodes != elems.end(); ++itNodes) {
		if (currentElement == 18955)
			TGLOG(*itNodes);
	}

	elems.erase(remove(elems.begin(), elems.end(), currentElement), elems.end());
	pair<int, int> p = most_common(elems);
	if (p.second == 4) {
		if ( m_ElementsInfo[currentElement - 1].iYarnIndex == m_ElementsInfo[p.first - 1].iYarnIndex )
			return -1;
	}

	return 0;
}

pair<int, vector<int> > COctreeVoxelMesh::GetFaceIndices2(CMesh::ELEMENT_TYPE ElemType, const set<int> &NodeIndices, int currentElement)
{
	//TGLOG("Checking element " << currentElement);
	vector<int> facesInd;
	int numFaces = 0;
	if (NodeIndices.size() == 5 || NodeIndices.size() == 4)
		numFaces = 1;
	if (NodeIndices.size() == 6)
		numFaces = 2;
	if (NodeIndices.size() == 7)
		numFaces = 6;
	if (NodeIndices.size() == 8)
		numFaces = 6;

	int i = 0, k = 0;
	while (i < numFaces) {
		if (NodeIndices.count(0) && NodeIndices.count(1) && NodeIndices.count(2) && NodeIndices.count(3)) {
			vector<int> n;
			n.push_back(m_AllElements[currentElement-1][0]);
			n.push_back(m_AllElements[currentElement-1][1]);
			n.push_back(m_AllElements[currentElement-1][2]);
			n.push_back(m_AllElements[currentElement-1][3]);

			int check = checkIndex(currentElement, n);
			if ( check != -1 )
				facesInd.push_back(0), i++;
		}

		if (NodeIndices.count(4) && NodeIndices.count(5) && NodeIndices.count(6) && NodeIndices.count(7)) {
			vector<int> n;
			n.push_back(m_AllElements[currentElement-1][4]);
			n.push_back(m_AllElements[currentElement-1][5]);
			n.push_back(m_AllElements[currentElement-1][6]);
			n.push_back(m_AllElements[currentElement-1][7]);

			int check = checkIndex(currentElement, n);
			if ( check != -1 )
				facesInd.push_back(1), i++;
		}

		if (NodeIndices.count(0) && NodeIndices.count(1) && NodeIndices.count(4) && NodeIndices.count(5)) {
			vector<int> n;
			n.push_back(m_AllElements[currentElement-1][0]);
			n.push_back(m_AllElements[currentElement-1][1]);
			n.push_back(m_AllElements[currentElement-1][4]);
			n.push_back(m_AllElements[currentElement-1][5]);

			int check = checkIndex(currentElement, n);
			if ( check != -1 )
				facesInd.push_back(2), i++;
		}

		if (NodeIndices.count(1) && NodeIndices.count(2) && NodeIndices.count(5) && NodeIndices.count(6)) {
			vector<int> n;
			n.push_back(m_AllElements[currentElement-1][1]);
			n.push_back(m_AllElements[currentElement-1][2]);
			n.push_back(m_AllElements[currentElement-1][5]);
			n.push_back(m_AllElements[currentElement-1][6]);

			int check = checkIndex(currentElement, n);
			if ( check != -1 )
				facesInd.push_back(3), i++;
		}

		if (NodeIndices.count(2) && NodeIndices.count(3) && NodeIndices.count(6) && NodeIndices.count(7)) {
			vector<int> n;
			n.push_back(m_AllElements[currentElement-1][2]);
			n.push_back(m_AllElements[currentElement-1][3]);
			n.push_back(m_AllElements[currentElement-1][6]);
			n.push_back(m_AllElements[currentElement-1][7]);

			int check = checkIndex(currentElement, n);
			if ( check != -1 )
				facesInd.push_back(4), i++;
		}

		if (NodeIndices.count(3) && NodeIndices.count(0) && NodeIndices.count(7) && NodeIndices.count(4)) {
			vector<int> n;
			n.push_back(m_AllElements[currentElement-1][3]);
			n.push_back(m_AllElements[currentElement-1][0]);
			n.push_back(m_AllElements[currentElement-1][7]);
			n.push_back(m_AllElements[currentElement-1][4]);

			int check = checkIndex(currentElement, n);
			if ( check != -1 )
				facesInd.push_back(5), i++;
		}

		if (k++ > numFaces) {
			// No faces found
			return make_pair(i,facesInd);
		}
	}

	return make_pair(numFaces, facesInd);
}

// Populate sets of nodes which are at an interface and elements which have at least one onde at an interface
void COctreeVoxelMesh::extractSurfaceNodeSets(map<int, vector<int>> &NodeSurf, vector<int> &AllSurf) {
	int i;
	map<int, vector<int>> NodeSets;
	vector<POINT_INFO>::iterator itData;
	vector<int>::iterator itNodes;
	map<int, vector<int>>::iterator itNodeSurf, itNodeSets;

	NodeSurf.clear();
	AllSurf.clear();

	// Create element and node sets for each material
	for (itData = m_ElementsInfo.begin(), i = 0; itData != m_ElementsInfo.end(); ++itData, i++) {
		NodeSets[itData->iYarnIndex].insert(NodeSets[itData->iYarnIndex].end(), m_AllElements[i].begin(), m_AllElements[i].end());
	}


	// Remove non-unique entries
	for (itNodeSets = NodeSets.begin(); itNodeSets != NodeSets.end(); ++itNodeSets) {
		vector<int> mat = itNodeSets->second;
		sort(mat.begin(), mat.end());
		mat.erase( unique(mat.begin(), mat.end()), mat.end() );
		itNodeSets->second = mat;
	}

	// Create sets of surface nodes for each material (find nodes which fall into both material sets)
	for (itNodeSets = NodeSets.begin(); itNodeSets != NodeSets.end(); ++itNodeSets) {
		map<int,vector<int>>::iterator itTempNodeSet;
		vector<int> currentSet;
		for (itTempNodeSet = NodeSets.begin(); itTempNodeSet != NodeSets.end(); ++itTempNodeSet) {
			if (itNodeSets->first != itTempNodeSet->first) {
				vector<int> v1 = itNodeSets->second;
				vector<int> v2 = itTempNodeSet->second;
				sort(v1.begin(), v1.end());
				sort(v2.begin(), v2.end());

				vector<int> v_intersection;
				set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v_intersection));
				currentSet.insert(currentSet.end(), v_intersection.begin(), v_intersection.end());
			}
		}
		sort(currentSet.begin(), currentSet.end());
		currentSet.erase( unique(currentSet.begin(), currentSet.end()), currentSet.end());
		NodeSurf[itNodeSets->first] = currentSet;
	}

	// Store all surface nodes in one place
	for (itNodeSurf = NodeSurf.begin(); itNodeSurf != NodeSurf.end(); ++itNodeSurf) {
		for (itNodes = itNodeSurf->second.begin(); itNodes != itNodeSurf->second.end(); ++itNodes) {
			// Instead of this if-condition sort/unique/erase sequence can be used
			if ( find(AllSurf.begin(), AllSurf.end(), *itNodes) == AllSurf.end() )
				AllSurf.push_back(*itNodes);
		}
	}

}

void COctreeVoxelMesh::smoothing(const map<int, vector<int>> &NodeSurf, const vector<int> &AllSurf)
{
	vector<POINT_INFO>::iterator itData;
	map<int, vector<int>>::const_iterator itNodeSurf;

	double coef = 0.63;
	map<int,XYZ> AllLapl;
	map<int,XYZ> PrevNodes;
	map<int,XYZ> OldNodes = AllNodes;

	vector<int> v = AllSurf;
	sort(v.begin(),v.end());

	// Prepare the neighbour connections (leave only nodes which are on an interface)
	map<int, vector<int>>::iterator itNeighbourNodes;

	for (itNeighbourNodes = m_NeighbourNodes.begin(); itNeighbourNodes != m_NeighbourNodes.end(); ++itNeighbourNodes) {
		vector<int> temp;
		vector<int> v_intersection;

		temp = itNeighbourNodes->second;
		sort(temp.begin(), temp.end());
		temp.erase( unique(temp.begin(), temp.end()), temp.end());
		set_intersection(v.begin(), v.end(), temp.begin(), temp.end(), back_inserter(v_intersection));
		itNeighbourNodes->second = v_intersection;
	}

	for (int iter_i = 0; iter_i < m_smoothIter; iter_i++) {
		if ( m_smoothCoef2 > 0 || iter_i%2 == 0) {
			PrevNodes = AllNodes;
		}

		coef = (iter_i%2 == 0) ? m_smoothCoef1 : m_smoothCoef2;

		double max_dx = 0.5*(m_DomainAABB.second.x - m_DomainAABB.first.x) / pow(2, static_cast<double>(max_level));
		double max_dy = 0.5*(m_DomainAABB.second.y - m_DomainAABB.first.y) / pow(2, static_cast<double>(max_level));
		double max_dz = 0.5*(m_DomainAABB.second.z - m_DomainAABB.first.z) / pow(2, static_cast<double>(max_level));
		double diag = sqrt( 4*max_dx * max_dx + 4*max_dy * max_dy + 4*max_dz * max_dz );

		for (itNodeSurf = NodeSurf.begin(); itNodeSurf != NodeSurf.end(); ++itNodeSurf) {
			vector<int>::const_iterator itNodes;
			for (itNodes = itNodeSurf->second.begin(); itNodes != itNodeSurf->second.end(); ++itNodes) {
				vector<int>::iterator itNeighbour;
				XYZ laplacian(0.0, 0.0, 0.0);

				int num = m_NeighbourNodes[*itNodes].size();
				XYZ orig = AllNodes[*itNodes];

				for (itNeighbour = m_NeighbourNodes[*itNodes].begin(); itNeighbour != m_NeighbourNodes[*itNodes].end(); ++itNeighbour) {
					laplacian -= coef/num*(orig - AllNodes[*itNeighbour]);

					// If a point is on a boundary, it should stay there
					if ( orig.x == m_DomainAABB.first.x || orig.x == m_DomainAABB.second.x)
						laplacian.x = 0;

					if ( orig.y == m_DomainAABB.first.y || orig.y == m_DomainAABB.second.y)
						laplacian.y = 0;

					if ( orig.z == m_DomainAABB.first.z || orig.z == m_DomainAABB.second.z)
						laplacian.z = 0;
				}

				if ( AllLapl.find(*itNodes) == AllLapl.end() ) {
					AllLapl.insert(make_pair(*itNodes,laplacian));
				}
			}
		}

		map<int, XYZ>::iterator itNodes;
		for (itNodes = AllNodes.begin(); itNodes != AllNodes.end(); ++itNodes) {
			if ( AllLapl.find(itNodes->first) != AllLapl.end() ) {
				AllNodes[itNodes->first] += AllLapl[itNodes->first];
				XYZ node_move = AllNodes[itNodes->first] - OldNodes[itNodes->first];
				// Now let's do the quality check:
				// We do not want to have a node move more than 0.5 of a side length or of element diagonal
				if ( fabs(node_move.x) > max_dx || fabs(node_move.y) > 0.5 * max_dy || fabs(node_move.z) > max_dz ||
					sqrt( node_move.x * node_move.x + node_move.y * node_move.y + node_move.z * node_move.z) > diag)
					AllNodes[itNodes->first] -= AllLapl[itNodes->first];

			}
		}
		AllLapl.clear();
	}
}


// Create a surface between the materials
// Iterate through surface nodes
// * For every node retrieve all elements containing this node
// * Check to how many materials these elements belong to
// * * For elements belonging to element with minimal material number - leave the original node
// * * For elements belonging to other materials - duplicate the node (for every material - one copy)
// * * Replace original node's entries in elements with newly create elements
// * Store original node and newly created in sets for surface definitions
void COctreeVoxelMesh::OutputSurfaces(const map<int, vector<int> > &NodeSurf, const vector<int> &AllSurf) {
	vector<int> AllSurfElems;
	vector<POINT_INFO>::iterator itData;
	vector<int>::const_iterator itNodes;
	map<int, vector<int>>::iterator itNodeSurf, itElemSurf, itYarnElems;
	
	map<int, vector<int>> MyNodeSurf, MyElemSurf, InteriorElems;
	vector<vector<int>> MyElemSurfVector;
	vector<POINT_INFO>::iterator itInfo;

	int extraNodeCount = 4000000;

	// Iterating through surface nodes
	for (itNodes = AllSurf.begin(); itNodes != AllSurf.end(); ++itNodes) {
		// Looking though elements containing this node and finding what material has the minimum index
		// The elements with minimum index are not changed. Others are "disconnected" by copying a node
		vector<int>::iterator itEncounter;
		//int minMaterial = NodeSurf.size() + 1;

		//looping through the elements containing this node
		vector<int> temp;
		for (itEncounter = m_NodesEncounter[*itNodes].begin(); itEncounter != m_NodesEncounter[*itNodes].end(); ++itEncounter) {
			temp.push_back(m_ElementsInfo[*itEncounter - 1].iYarnIndex);
			//if (m_ElementsInfo[*itEncounter - 1].iYarnIndex < minMaterial) {
			//	minMaterial = m_ElementsInfo[*itEncounter - 1].iYarnIndex;
			//}
		}
		int minMaterial = *min_element(temp.begin(), temp.end());

		vector<int> usedMaterial;
		vector<int> usedNodes;

		// Iterating through elements which include this surface node
		// The elements with the minimal material number (usually matrix) are left intact
		// The elements with other material number are amended as follows:
		// * if this is the first appearance of this material number
		// * * copy the node, reassign the original node with copied node to the element definition
		// * if this is not the first appearance of this material number
		// * * find number of the copied node and reassign the original node with the copied node to the element definition
		for (itEncounter = m_NodesEncounter[*itNodes].begin(); itEncounter != m_NodesEncounter[*itNodes].end(); ++itEncounter) {

			MyElemSurf[m_ElementsInfo[*itEncounter-1].iYarnIndex].push_back(*itEncounter);
			MyNodeSurf[m_ElementsInfo[*itEncounter-1].iYarnIndex].push_back(*itNodes);
		}
	}


	for (itNodeSurf = MyNodeSurf.begin(); itNodeSurf != MyNodeSurf.end(); ++itNodeSurf)
	{
		sort( itNodeSurf->second.begin(), itNodeSurf->second.end());
		itNodeSurf->second.erase( unique(itNodeSurf->second.begin(), itNodeSurf->second.end()) , itNodeSurf->second.end() );
	}

	m_SurfaceNodes = MyNodeSurf;

	for (itElemSurf = MyElemSurf.begin(); itElemSurf != MyElemSurf.end(); ++itElemSurf)
	{
		sort( itElemSurf->second.begin(), itElemSurf->second.end());
		itElemSurf->second.erase( unique(itElemSurf->second.begin(), itElemSurf->second.end()) , itElemSurf->second.end() );
	}



	MyElemSurfVector.reserve(MyElemSurf.size());
	for (auto elem : MyElemSurf)
		MyElemSurfVector.push_back(elem.second);

	omp_set_num_threads(2);
	for (vector<vector<int>>::iterator itElemSurfVector = MyElemSurfVector.begin(); itElemSurfVector != MyElemSurfVector.end(); ++itElemSurfVector) {
		vector<int>::iterator itElems;
		vector<vector<int>>::iterator it = find(MyElemSurfVector.begin(), MyElemSurfVector.end(), *itElemSurfVector);
		int index = distance(MyElemSurfVector.begin(), it);
		TGLOG("index is " << index);
		vector<pair<int, int>> surfelemfacesvec;
		#pragma omp parallel
		{
			vector<pair<int, int>> privatevector;
			#pragma omp for
			for (itElems = (*itElemSurfVector).begin(); itElems != (*itElemSurfVector).end(); ++itElems)
			{
				set<int> CommonIndices = GetCommonIndices(MyNodeSurf[index - 1], m_AllElements[*itElems - 1]);

				pair<int, vector<int>> checkFaces = GetFaceIndices2(CMesh::HEX, CommonIndices, *itElems);
				if (checkFaces.first != -1)
				{
					vector<int>::iterator itFace;
					for (itFace = checkFaces.second.begin(); itFace != checkFaces.second.end(); ++itFace) {
						privatevector.push_back(make_pair(*itElems, *itFace + 1));
						//m_SurfaceElementFaces[index - 1].push_back(make_pair(*itElems, *itFace + 1));
					}
				}
			}
			#pragma omp critical
			{
				surfelemfacesvec.insert(surfelemfacesvec.end(), privatevector.begin(), privatevector.end());
				//m_SurfaceElementFaces[index - 1].push_back(surfelemfacesvec);
			}
		}
		TGLOG(surfelemfacesvec.size());
		for (auto elem = surfelemfacesvec.begin(); elem != surfelemfacesvec.end(); elem++)
			m_SurfaceElementFaces[index - 1].push_back(*elem);
	}
	
	TGLOG("finished loop")
	MyNodeSurf.clear();
	MyElemSurf.clear();
	for (itNodes = AllSurf.begin(); itNodes != AllSurf.end(); ++itNodes) {
		// Looking though elements containing this node and finding what material has the minimum index
		// The elements with minimum index are not changed. Others are "disconnected" by copying a node
		vector<int>::iterator itEncounter;
		//int minMaterial = NodeSurf.size() + 1;

		vector<int> temp;
		for (itEncounter = m_NodesEncounter[*itNodes].begin(); itEncounter != m_NodesEncounter[*itNodes].end(); ++itEncounter) {
			temp.push_back(m_ElementsInfo[*itEncounter - 1].iYarnIndex);
			//if (m_ElementsInfo[*itEncounter - 1].iYarnIndex < minMaterial) {
			//	minMaterial = m_ElementsInfo[*itEncounter - 1].iYarnIndex;
			//}
		}
		int minMaterial = *min_element(temp.begin(), temp.end());

		vector<int> usedMaterial;
		vector<int> usedNodes;

		// Iterating through elements which include this surface node
		// The elements with the minimal material number (usually matrix) are left intact
		// The elements with other material number are amended as follows:
		// * if this is the first appearance of this material number
		// * * copy the node, reassign the original node with copied node to the element definition
		// * if this is not the first appearance of this material number
		// * * find number of the copied node and reassign the original node with the copied node to the element definition
		for (itEncounter = m_NodesEncounter[*itNodes].begin(); itEncounter != m_NodesEncounter[*itNodes].end(); ++itEncounter) {

			MyElemSurf[m_ElementsInfo[*itEncounter-1].iYarnIndex].push_back(*itEncounter);

			if (m_ElementsInfo[*itEncounter-1].iYarnIndex == minMaterial) {
				MyNodeSurf[m_ElementsInfo[*itEncounter-1].iYarnIndex].push_back(*itNodes);
			} else {
				vector<int>::iterator it = find(usedMaterial.begin(), usedMaterial.end(), m_ElementsInfo[*itEncounter-1].iYarnIndex);
				if (  usedMaterial.size() > 0 && it != usedMaterial.end() ) {
					// The node has already been copied, replace corresponding nodes in the element
					replace(m_AllElements[*itEncounter-1].begin(), m_AllElements[*itEncounter-1].end(), *itNodes, usedNodes[it - usedMaterial.begin()]);
				} else {
					// Copy the existing node and replace it in the element
					AllNodes.insert(make_pair(extraNodeCount, AllNodes[*itNodes]));
					MyNodeSurf[m_ElementsInfo[*itEncounter-1].iYarnIndex].push_back(extraNodeCount);
					replace(m_AllElements[*itEncounter-1].begin(), m_AllElements[*itEncounter-1].end(), *itNodes, extraNodeCount);

					// Save info about the copied node
					usedMaterial.push_back(m_ElementsInfo[*itEncounter - 1].iYarnIndex);
					usedNodes.push_back(extraNodeCount);
					extraNodeCount++;
				}
			}
		}
	}

	for (itNodeSurf = MyNodeSurf.begin(); itNodeSurf != MyNodeSurf.end(); ++itNodeSurf)
	{
		sort( itNodeSurf->second.begin(), itNodeSurf->second.end());
		itNodeSurf->second.erase( unique(itNodeSurf->second.begin(), itNodeSurf->second.end()) , itNodeSurf->second.end() );
	}

	m_SurfaceNodes = MyNodeSurf;

}