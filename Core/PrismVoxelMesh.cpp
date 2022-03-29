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

	// Create map of which elements in the xy bounding box are in the prism cross-section - assumes constant cross-section
	GetElementMap(Textile);

	return true;
}

void CPrismVoxelMesh::OutputNodes(ostream &Output, CTextile &Textile, int Filetype)
{
	int x, y, z;
	int iNodeIndex = 1;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;
	XYZ StartPoint = m_StartPoint;

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
}

void CPrismVoxelMesh::GetElementMap(CTextile &Textile)
{
	m_ElementMap.clear();
	m_NumElements = 0;
	CDomainPrism* Domain = Textile.GetDomain()->GetPrismDomain();
	vector<XY> PrismPoints = Domain->GetPoints();

	XY Point, Min, Max;
	GetMinMaxXY( PrismPoints, Min, Max);
	double XSize = (Max.x - Min.x) / m_XVoxels;
	double ZSize = (Max.y - Min.y) / m_ZVoxels;  // y in 2D polygon translates to z coordinate in 3D

	Point.y = Min.y + 0.5*ZSize;
	for (int j = 0; j < m_ZVoxels; ++j)
	{
		Point.x = Min.x + 0.5*XSize;
		for (int i = 0; i < m_XVoxels; ++i)
		{
			if (PointInside(Point, PrismPoints))
			{
				m_ElementMap[make_pair(i, j)] = true;
				m_NumElements++;
			}
			else
				m_ElementMap[make_pair(i, j)] = false;
			Point.x += XSize;
		}
		Point.y += ZSize;
	}
}

int CPrismVoxelMesh::OutputHexElements(ostream &Output, bool bOutputMatrix, bool bOutputYarn, int Filetype)
{
	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int x, y, z;
	vector<POINT_INFO>::iterator itElementInfo = m_ElementsInfo.begin();
	int iElementNumber = 1;

	vector<POINT_INFO> NewElementInfo;

	if (Filetype == SCIRUN_EXPORT)
		Output << m_NumElements * m_YVoxels;

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