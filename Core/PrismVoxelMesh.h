/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2020 Louise Brown

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
#include "VoxelMesh.h"
#include "TJointBoundaries.h"

namespace TexGen
{
	using namespace std;

	class CTextile;

	/// Class used to generate voxel mesh of prism domain for output to ABAQUS
	class CLASS_DECLSPEC CPrismVoxelMesh : public CVoxelMesh
	{
	public:
		CPrismVoxelMesh(string Type = "CPrismPeriodicBoundaries");
		virtual ~CPrismVoxelMesh(void);

		/// Outputs hex elements for the elements in the element map
		


		//does this need to be virtual George? Check
		void OutputTJointBoundaries(ostream & Output, CTextile & Textile, int iTJointConditions, bool bMatrixOnly);

		vector<XYZ> GetPrismMeshNodes() { return m_PrismMeshNodes; };

		vector<int> GetWebNodes() { return m_WebNodes; };

		vector<int> GetFlangeANodes() { return m_Flange_A_Nodes; };

		vector<int> GetFlangeBNodes() { return m_Flange_B_Nodes; };

		int GetNumberofNodes() { return m_NumberofNodes; };
	

	protected:
		void SaveToAbaqus(string Filename, CTextile & Textile, bool bOutputMatrix, bool bOutputYarn, int iBoundaryConditions, int iTJointConditions, int iElementType);

		int OutputHexElements(ostream &Output, bool bOutputMatrix, bool bOutputYarn, int Filetype);

		/// Calculate voxel size based on number of voxels on each axis and domain size
		bool CalculateVoxelSizes(CTextile &Textile);



		/// Outputs nodes to .inp file and gets element information
		void OutputNodes(ostream &Output, CTextile &Textile, int Filetype = INP_EXPORT);

		/// x, y, z lengths of rotated voxels
		XYZ				m_RotatedVoxSize[3];
		/// Reference point for generating voxel grid (Point 0 of domain mesh)
		XYZ				m_StartPoint;




		///Vector of prism mesh nodes within the 
		vector<XYZ> m_PrismMeshNodes;

		CTJointBoundaries* m_TJointBoundaries;

		vector<int> m_WebNodes;

		vector<int> m_Flange_A_Nodes;

		vector<int> m_Flange_B_Nodes;

		int m_NumberofNodes;
	};
};	// namespace TexGen