/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2012 Louise Brown

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
#include "RectangularVoxelMesh.h"
#include "TexGen.h"
#include "PeriodicBoundaries.h"
#include <iterator>
//#define SHINY_PROFILER TRUE

using namespace TexGen;

CRectangularVoxelMesh::CRectangularVoxelMesh(string Type)
:CVoxelMesh(Type)
{
	//m_PeriodicBoundaries = new CPeriodicBoundaries;
}

CRectangularVoxelMesh::~CRectangularVoxelMesh(void)
{
	//delete m_PeriodicBoundaries;
}

void CRectangularVoxelMesh::SaveVoxelMeshg(CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum, bool bOutputMatrix, bool bOutputYarns, bool surfaceOutput, int iBoundaryConditions, int iElementType)
{
	//don't really know what values to put in this function for min and max refinement,
	// the function is needed to setup OctreeMesh class data members otherwise it crashes when the destructor is invoked
	if (OctMesh.CreateP4ESTRefinement(4, 4) == -1)
		return;
	CVoxelMesh::SaveVoxelMesh(Textile, OutputFilename, XVoxNum, YVoxNum, ZVoxNum, bOutputMatrix, bOutputYarns, surfaceOutput, iBoundaryConditions, iElementType);
}

bool CRectangularVoxelMesh::CalculateVoxelSizes(CTextile &Textile)
{
	XYZ DomSize;

	m_DomainAABB = Textile.GetDomain()->GetMesh().GetAABB();
	DomSize = m_DomainAABB.second - m_DomainAABB.first;
	
	m_VoxSize[0] = DomSize.x / m_XVoxels;
	m_VoxSize[1] = DomSize.y / m_YVoxels;
	m_VoxSize[2] = DomSize.z / m_ZVoxels;
	return true;
}

void CRectangularVoxelMesh::OutputNodes(ostream &Output, CTextile &Textile, bool surfaceOutput, bool bAbaqus )
{
	int x,y,z, numx, numy;
	numx = m_XVoxels + 1;
	numy = m_YVoxels + 1;
	int iNodeIndex = 1;
	int ElementCount = 1;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;
	XYZ Point;
	vector<int> elementNodes;
	

	if ( !bAbaqus )  // if outputting in SCIRun format need to output number of voxels
		Output << (m_XVoxels+1)*(m_YVoxels+1)*(m_ZVoxels+1) << endl;
	
	for ( z = 0; z <= m_ZVoxels; ++z )
	{
		for ( y = 0; y <= m_YVoxels; ++y )
		{
			for ( x = 0; x <=m_XVoxels; ++x )
			{
				
				Point.x = m_DomainAABB.first.x + m_VoxSize[0] * x;
				Point.y = m_DomainAABB.first.y + m_VoxSize[1] * y;
				Point.z = m_DomainAABB.first.z + m_VoxSize[2] * z;
				if (!surfaceOutput)
				{
					if ( bAbaqus )
						Output << iNodeIndex << ", ";
					Output << Point << endl;
					
				}

				Nodes.insert(make_pair(iNodeIndex, Point));

				if ( x < m_XVoxels && y < m_YVoxels && z < m_ZVoxels )
				{
					Point.x += 0.5*m_VoxSize[0];
					Point.y += 0.5*m_VoxSize[1];
					Point.z += 0.5*m_VoxSize[2];
					CentrePoints.push_back(Point);

					//George - these are the nodes at the corner of each element
					int c1 = (x+1) +y*numx + z*numx*numy + 1;
					int c2 = (x+1) + (y+1)*numx + z*numx*numy + 1;
					int c3 = x + (y+1)*numx + z*numx*numy + 1;
					int c4 = x + y*numx + z*numx*numy + 1;
					int c5 = (x+1) +y*numx + (z+1)*numx*numy + 1;
					int c6 = (x+1) +(y+1)*numx + (z+1)*numx*numy + 1;
					int c7 = x +(y+1)*numx + (z+1)*numx*numy + 1;
					int c8 = x +y*numx + (z+1)*numx*numy + 1;
					elementNodes.push_back(c1);
					elementNodes.push_back(c2);
					elementNodes.push_back(c3);
					elementNodes.push_back(c4);
					elementNodes.push_back(c5);
					elementNodes.push_back(c6);
					elementNodes.push_back(c7);
					elementNodes.push_back(c8);

					//this needs to happen up until the last node for x<m_xVoxels otherwise get double counting of centrepoints
					AllElements.push_back(elementNodes);
					for (int i=0; i<8; i++)
					{
						NodesEncounter[elementNodes[i]].push_back(ElementCount);
					}
					//NodesEncounter.push_back(iNodeIndex, elementNodes);
					ElementCount++;
					elementNodes.clear();
				}
				++iNodeIndex;
			}
			
		}
		RowInfo.clear();   // Changed to do layer at a time instead of row to optimise
		Textile.GetPointInformation( CentrePoints, RowInfo );
		m_ElementsInfo.insert(m_ElementsInfo.end(), RowInfo.begin(), RowInfo.end() );
		
		CentrePoints.clear();
	}

	if (surfaceOutput)
		OutputInterfaceSurfaces(Output, Textile, bAbaqus);
		return;

	return;
	//Textile.GetPointInformation( CentrePoints, m_ElementsInfo );
}

void CRectangularVoxelMesh::OutputInterfaceSurfaces(ostream& Output, CTextile& Textile, bool bAbaqus)
{
	//George - Using this function to call Octree Mesh functions and set up relevant data members, should work the same as calling OctMesh::OutputNodes

	map<int, vector<int>> NodeSurf;
	vector<int> AllSurf;
	OctMesh.m_bSmooth=false;
	OctMesh.m_bCohesive=true;
	OctMesh.m_smoothCoef1=0.0;
	OctMesh.m_smoothCoef2=0.0;
	OctMesh.m_smoothIter=0;


	//OctMesh.ConvertOctreeToNodes();

	OctMesh.AllNodes=Nodes;
	//George - assign rect mesh version of allelements to the octree mesh class as m_AllElements
	OctMesh.m_AllElements = AllElements;
	OctMesh.m_NodesEncounter = NodesEncounter;
	vector<XYZ> CentrePoints;
	Textile.GetPointInformation(CentrePoints, m_ElementsInfo);
	//OctMesh.gTextile.GetPointInformation(CentrePoints, m_ElementsInfo);
	OctMesh.m_ElementsInfo=m_ElementsInfo;
	OctMesh.extractSurfaceNodeSets(NodeSurf, AllSurf);

	//not OctMesh data members, this works for now
	NodeSurf=NodeSurf;
	AllSurf=AllSurf;

	//if the nodes need to be ordered for this to work then will have to find the correct ordering for them
	
	OctMesh.OutputSurfaces(NodeSurf, AllSurf);

	//octree output surfaces adds the copied nodes
	Nodes=OctMesh.AllNodes;

	//will use this to output all the nodes instead of in RectMesh::OutputNodes()
	map<int,XYZ>::iterator itNodes;
	for (itNodes = Nodes.begin(); itNodes != Nodes.end(); ++itNodes) {
		Output << itNodes->first << ", " << itNodes->second.x << ", " << itNodes->second.y << ", " << itNodes->second.z << endl;
	}

}

int CRectangularVoxelMesh::OutputHexElements(ostream &Output, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, bool bAbaqus )
{
	//want to override the base class function, they should work the same
	int elemnum = OctMesh.OutputHexElements(Output, Textile, bOutputMatrix, bOutputYarn, bAbaqus );
	
	//int elemnum = CVoxelMesh::OutputHexElements(Output, bOutputMatrix, bOutputYarn, bAbaqus );
	return elemnum;
}
