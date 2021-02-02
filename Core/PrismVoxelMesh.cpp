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

#include "PrecompiledHeaders.h"
#include "PrismVoxelMesh.h"
#include "TexGen.h"
#include "DomainPrism.h"
#include "TJointBoundaries.h"
#include <iterator>
//#define SHINY_PROFILER TRUE


using namespace TexGen;

CPrismVoxelMesh::CPrismVoxelMesh(string Type)
	:CVoxelMesh(Type)
{

}

CPrismVoxelMesh::~CPrismVoxelMesh(void)
{

}

bool CPrismVoxelMesh::CalculateVoxelSizes(CTextile &Textile)
{
	XYZ DomSize;
	
	CDomainPrism* Domain = Textile.GetDomain()->GetPrismDomain();

	// Get the XYZ size of each axis of the domain, 
	// taking into account that they may be rotated depending on the orientation of the domain yarn specified
	Domain->GetPolygonLimits( m_StartPoint, m_RotatedVoxSize );

	m_RotatedVoxSize[0] /= m_XVoxels;
	m_RotatedVoxSize[1] /= m_YVoxels;
	m_RotatedVoxSize[2] /= m_ZVoxels;

	GetElementMap(Textile);

	return true;
}

void CPrismVoxelMesh::OutputNodes(ostream &Output, CTextile &Textile, int Filetype)
{
	int x, y, z;
	int iNodeIndex = 1;
	int NumberFlangeNodes = 0;
	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int numz = m_ZVoxels + 1;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;
	XYZ StartPoint = m_StartPoint;

	bool isFirst = true;
	for (z = 0; z <= m_ZVoxels; ++z)
	{
		StartPoint = m_StartPoint + m_RotatedVoxSize[2] * z;

		for (y = 0; y <= m_YVoxels; ++y)
		{
			XYZ YStartPoint;
			YStartPoint = StartPoint + m_RotatedVoxSize[1] * y;

			for (x = 0; x <= m_XVoxels; ++x)
			{
				XYZ Point;
				Point = YStartPoint + m_RotatedVoxSize[0] * x;

				if (Filetype == INP_EXPORT)
				{
					Output << iNodeIndex << ", ";
					Output << Point << "\n";
				}
				else if (Filetype == VTU_EXPORT)
					m_Mesh.AddNode(Point);

				if ( x < m_XVoxels && y < m_YVoxels && z < m_ZVoxels)
				{
					if ( m_ElementMap.at(make_pair(x, z)) )  // Only store centre points for elements within prism
					{
									
						Point.x += 0.5*m_RotatedVoxSize[0].x;
						Point.x += 0.5*m_RotatedVoxSize[1].x;
						Point.x += 0.5*m_RotatedVoxSize[2].x;
						Point.y += 0.5*m_RotatedVoxSize[0].y;
						Point.y += 0.5*m_RotatedVoxSize[1].y;
						Point.y += 0.5*m_RotatedVoxSize[2].y;
						Point.z += 0.5*m_RotatedVoxSize[0].z;
						Point.z += 0.5*m_RotatedVoxSize[1].z;
						Point.z += 0.5*m_RotatedVoxSize[2].z;
						CentrePoints.push_back(Point);
					}
				}
				++iNodeIndex;
			}

		}
		RowInfo.clear();   // Changed to do layer at a time instead of row to optimise
		Textile.GetPointInformation(CentrePoints, RowInfo);
		m_ElementsInfo.insert(m_ElementsInfo.end(), RowInfo.begin(), RowInfo.end());
		CentrePoints.clear();
	}
	m_NumberofNodes = iNodeIndex;
}



int CPrismVoxelMesh::OutputHexElements(ostream &Output, bool bOutputMatrix, bool bOutputYarn, int Filetype)
{

	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int numz = m_ZVoxels + 1;
	int x, y, z;
	vector<POINT_INFO>::iterator itElementInfo = m_ElementsInfo.begin();
	int iElementNumber = 1;
	int iNodeIndex = 1;

	vector<POINT_INFO> NewElementInfo;

	if (Filetype == SCIRUN_EXPORT)
		Output << m_NumElements * m_YVoxels;
	bool isFirst = true;
	for (z = 0; z < m_ZVoxels; ++z)
	{
		for (y = 0; y < m_YVoxels; ++y)
		{
			for (x = 0; x < m_XVoxels; ++x)
			{

				if (m_ElementMap[make_pair(x, z)])  // Only export elements within domain prism outline
				{


					if ((itElementInfo->iYarnIndex == -1 && bOutputMatrix)
						|| (itElementInfo->iYarnIndex >= 0 && bOutputYarn))
					{
						if (Filetype == INP_EXPORT)
						{
							//These are corner nodes  
							Output << iElementNumber << ", ";
							Output << (x + 1) + y*numx + z*numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + z*numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + z*numx*numy + 1 << ", " << x + y*numx + z*numx*numy + 1 << ", ";
							Output << (x + 1) + y*numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", " << x + y*numx + (z + 1)*numx*numy + 1 << "\n";
						}
						else if (Filetype == SCIRUN_EXPORT)
						{
							Output << x + y*numx + z*numx*numy + 1 << ", " << (x + 1) + y*numx + z*numx*numy + 1 << ", ";
							Output << x + y*numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + y*numx + (z + 1)*numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + z*numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + z*numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + (z + 1)*numx*numy + 1 << "\n";
						}
						else  // VTU export
						{
							vector<int> Indices;
							Indices.push_back(x + y*numx + z*numx*numy);
							Indices.push_back((x + 1) + y*numx + z*numx*numy);
							Indices.push_back((x + 1) + y*numx + (z + 1)*numx*numy);
							Indices.push_back(x + y*numx + (z + 1)*numx*numy);
							Indices.push_back(x + (y + 1)*numx + z*numx*numy);
							Indices.push_back((x + 1) + (y + 1)*numx + z*numx*numy);
							Indices.push_back((x + 1) + (y + 1)*numx + (z + 1)*numx*numy);
							Indices.push_back(x + (y + 1)*numx + (z + 1)*numx*numy);
							m_Mesh.AddElement(CMesh::HEX, Indices);
						}


						if (z == 0)
						{

							m_Flange_A_Nodes.push_back((x + 1) + y * numx + z * numx*numy + 1);
							m_Flange_A_Nodes.push_back(x + (y + 1)*numx + z * numx*numy + 1);
							m_Flange_A_Nodes.push_back((x + 1) + (y + 1)*numx + z * numx*numy + 1);
							m_Flange_A_Nodes.push_back(x + y * numx + z * numx*numy + 1);

							//remove duplicates
							std::sort(m_Flange_A_Nodes.begin(), m_Flange_A_Nodes.end());
							m_Flange_A_Nodes.erase(unique(m_Flange_A_Nodes.begin(), m_Flange_A_Nodes.end()), m_Flange_A_Nodes.end());

						}
						else if (z==m_ZVoxels-1)
						{
							m_Flange_B_Nodes.push_back(x + y * numx + (z + 1)*numx*numy + 1);
							m_Flange_B_Nodes.push_back(x + (y + 1)*numx + (z + 1)*numx*numy + 1);
							m_Flange_B_Nodes.push_back((x + 1) + (y + 1)*numx + (z + 1)*numx*numy + 1);
							m_Flange_B_Nodes.push_back((x + 1) + y * numx + (z + 1)*numx*numy + 1);



							std::sort(m_Flange_B_Nodes.begin(), m_Flange_B_Nodes.end());
							m_Flange_B_Nodes.erase(unique(m_Flange_B_Nodes.begin(), m_Flange_B_Nodes.end()), m_Flange_B_Nodes.end());
						}


						if (x == 0)
						{
							m_WebNodes.push_back(x + y * numx + z * numx*numy + 1);
							m_WebNodes.push_back(x + (y + 1)*numx + z * numx*numy + 1);
							m_WebNodes.push_back(x + y * numx + (z + 1)*numx*numy + 1);
							m_WebNodes.push_back(x + (y + 1)*numx + (z + 1)*numx*numy + 1);

							std::sort(m_WebNodes.begin(), m_WebNodes.end());
							m_WebNodes.erase(unique(m_WebNodes.begin(), m_WebNodes.end()), m_WebNodes.end());

						}


						++iElementNumber;
						if (bOutputYarn && !bOutputMatrix) // Just saving yarn so need to make element array with just yarn info
						{
							NewElementInfo.push_back(*itElementInfo);
						}

						

					}
					++itElementInfo;  // Only saved element info for elements within domain outline
				}


			}
		}
	}


	if (bOutputYarn && !bOutputMatrix)
	{
		m_ElementsInfo.clear();
		m_ElementsInfo = NewElementInfo;
	}
	return (iElementNumber - 1);
}

void CPrismVoxelMesh::SaveToAbaqus(string Filename, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, int iBoundaryConditions, int iTJointConditions, int iElementType)
{
	
	//PROFILE_FUNC();
	AddExtensionIfMissing(Filename, ".inp");

	ofstream Output(Filename.c_str());

	if (!Output)
	{
		TGERROR("Unable to output voxel mesh to ABAQUS file format, could not open file: " << Filename);
		return;
	}

	TGLOG("Saving voxel mesh data to " << Filename);

	Output << "*Heading" << "\n";
	Output << "File generated by TexGen v" << TEXGEN.GetVersion() << "\n";

	Output << "************" << "\n";
	Output << "*** MESH ***" << "\n";
	Output << "************" << "\n";
	Output << "*Node" << "\n";
	//PROFILE_BEGIN(OutputNodes);
	OutputNodes(Output, Textile);
	//PROFILE_END();
	TGLOG("Outputting hex elements");
	//Output the voxel HEX elements
	int iNumHexElements = 0;
	if (!iElementType)
	{
		Output << "*Element, Type=C3D8R" << "\n";
	}
	else
	{
		Output << "*Element, Type=C3D8" << "\n";
	}
	//PROFILE_BEGIN(OutputHexElements);
	iNumHexElements = OutputHexElements(Output, bOutputMatrix, bOutputYarn, 0);
	//PROFILE_END();
	bool bMatrixOnly = false;
	if (bOutputMatrix && !bOutputYarn)
		bMatrixOnly = true;

	if (bOutputYarn)
	{
		TGLOG("Outputting orientations & element sets");
		//PROFILE_BEGIN(OutputOrientations);
		OutputOrientationsAndElementSets(Filename, Output);
		//PROFILE_END();
	}
	else if (bMatrixOnly)
	{
		OutputMatrixElementSet(Filename, Output, iNumHexElements, bMatrixOnly);
	}
	//PROFILE_BEGIN(OutputNodeSets);
	OutputAllNodesSet(Filename, Output);

	// Output material properties
	m_Materials.SetupMaterials(Textile);
	m_Materials.OutputMaterials(Output, Textile.GetNumYarns(), bMatrixOnly);

	//PROFILE_END();
	if (iBoundaryConditions != NO_BOUNDARY_CONDITIONS)
	{
		//PROFILE_BEGIN(OutputPBCs);
		OutputPeriodicBoundaries(Output, Textile, iBoundaryConditions, bMatrixOnly);
		//PROFILE_END();
	}

	if (iTJointConditions == TJOINT_CONDITIONS)
	{
		OutputTJointBoundaries(Output, Textile, iTJointConditions, bMatrixOnly);
	}
	TGLOG("Finished saving to Abaqus");
}

void CPrismVoxelMesh::OutputTJointBoundaries(ostream & Output, CTextile & Textile, int iTJointConditions, bool bMatrixOnly)
{
	if (iTJointConditions == 0)
		m_TJointBoundaries->CreateTJointBoundaries(this, Output, Textile, bMatrixOnly);
}