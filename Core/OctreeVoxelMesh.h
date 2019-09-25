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
#pragma once


//#include <inttypes.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <BaseTsd.h>
#include<unordered_map>
typedef SSIZE_T ssize_t;

extern "C" {
#include "p4est_to_p8est.h"
#ifndef P4_TO_P8
#include <p4est_bits.h>
#include <p4est_ghost.h>
#include <p4est_lnodes.h>
#else
#include <p8est_bits.h>
#include <p8est_ghost.h>
#include <p8est_lnodes.h>
#include <p8est_connectivity.h>
#endif
}

#include "VoxelMesh.h"

namespace TexGen
{
	using namespace std;

	class CTextile;

	/// Class used to generate octree-refine voxel mesh for output to ABAQUS
	class CLASS_DECLSPEC COctreeVoxelMesh : public CVoxelMesh
	{
		friend class CRectangularVoxelMesh;
	public:
		/**
		\param Type String giving periodic boundary condition type
		*/
		COctreeVoxelMesh(string Type= "CPeriodicBoundaries");
		virtual ~COctreeVoxelMesh(void);

		static int max_level;

		static vector<XYZ> cornerPoints;
		static vector<XYZ> CentrePoints;

		static vector< vector<int> > FaceX_min;
		static vector< vector<int> > FaceX_max;
		static vector< vector<int> > FaceY_min;
		static vector< vector<int> > FaceY_max;
		static vector< vector<int> > FaceZ_min;
		static vector< vector<int> > FaceZ_max;

		static CTextile gTextile;
		static pair<XYZ, XYZ> g_DomainAABB;
		static vector<char> materialInfo;

		/** Save Octree-refined mesh with an option of surface smoothing
		\param int min_level
		\param int refine_level - maximum refinement level
		\param bool smoothing True to enable surface smoothing
		\param int smoothIter to control number of smoothing iterations
		\param double smooth1 is the first smoothing parameter
		\param double smooth2 is the second smoothing parameter
		*/
		void SaveVoxelMesh(CTextile &Textile, string OutputFilename, int min_level, int refine_level, bool smoothing, int smoothIter, double smooth1, double smooth2, bool surfaceOuput, bool cohesive);


	protected:
		// The operator for comparison can be incorporated into XYZ structure and the all the "Point" can be replaced with "XYZ"
		struct Point {
			int nodeNum;
			double x;
			double y;
			double z;

			Point() {}
			Point(int nodeNum, double x, double y, double z) : nodeNum(nodeNum), x(x), y(y), z(z) {}

			bool operator<(const Point &o) const {
				if (x != o.x) {
					return x < o.x;
				}
				if (y != o.y) {
					return y < o.y;
				}
				return z < o.z;
			}
		};
		/// Performs octree refinement using external P4EST library
		int CreateP4ESTRefinement(int min_level, int refine_level);

		/// Calculate voxel size based on number of voxels on each axis and domain size
		bool CalculateVoxelSizes(CTextile &Textile);

		/// Outputs nodes to .inp file and gets element information
		void OutputNodes(ostream &Output, CTextile &Textile, bool bAbaqus = true, bool surfaceOutput = false );
		/// Output elements to .inp file
		int OutputHexElements(ostream &Output, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, bool bAbaqus = true );
		/// Store info (nodes at boundaries) for periodic BCs
		void OutputPeriodicBoundaries(ostream &Output, CTextile& Textile, int iBoundaryConditions, bool bMatrixOnly);
		/// Decode the P4EST mesh structure and store nodes/elements
		void ConvertOctreeToNodes();
		/// Storing hanging nodes from octree-mesh to a vector for writing the constraints
		int storeHangingNode(int *all_lni, int *hanging_corner, int node_i, int hanging_count);
		/// Smoothing
		void smoothing(const std::map< int, vector<int> > &NodeSurf, const vector<int> &AllSurf);
		void fillMaterialInfo();
		int isBoundary(double p[3]);

		int checkIndex(int currentElement, vector<int> nodes);
		pair<int, vector<int> > GetFaceIndices2(CMesh::ELEMENT_TYPE ElemType, const set<int> &NodeIndices, int currentElement);

		void storePointInfo(int refineLevel);
		static int getPointsInfo(vector<XYZ> myPoints, int refineLevel);

		//int refine_fn_uni(p4est_t * p4est, p4est_topidx_t which_tree, p4est_quadrant_t * quadrant);
		static int refine_fn_uni(p4est_t * p4est, p4est_topidx_t which_tree, p4est_quadrant_t * quadrant);
		static int refine_fn_periodic(p4est_t * p4est, p4est_topidx_t which_tree, p4est_quadrant_t * quadrant);
		static int refine_fn_post(p4est_t * p4est, p4est_topidx_t which_tree, p4est_quadrant_t * quadrant);
		static int refine_fn(p4est_t * p4est, p4est_topidx_t which_tree, p4est_quadrant_t * quadrant);

		static void FindLocMinMax( int& XMin, int& XMax, int& YMin, int& YMax, XYZ& Min, XYZ& Max );

		void extractSurfaceNodeSets(std::map< int, vector<int> > &NodeSurf, std::vector<int> &AllSurf);
		void OutputSurfaces(const std::map<int, vector<int> > &NodeSurf, const std::vector<int> &AllSurf);

		p4est_t *p4est;
		p4est_connectivity_t *conn;

		map<int,XYZ> AllNodes;
		vector< std::vector<int> > m_AllElements;
		unordered_map< int, vector<int> > m_NodeConstraints;
		unordered_map< int, vector<int> > m_NodesEncounter;
		unordered_map< int, vector<int> > m_NeighbourNodes;

		unordered_map< int, vector<int> > m_SurfaceNodes;
		unordered_map<int, vector< pair<int, int> > > m_SurfaceElementFaces;

		//vector< vector<int> > MaterialElements;
		vector<Point> m_boundaryPoints;


		bool m_bSmooth;
		bool m_bSurface;
		bool m_bCohesive;
		double m_smoothCoef1, m_smoothCoef2;
		int m_smoothIter;
	};
};	// namespace TexGen