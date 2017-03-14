/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

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
#include "SectionMeshTriangulate.h"
#include "Section.h"

extern "C"
{
#include "../Triangle/triangle.h"
}

using namespace TexGen;

CSectionMeshTriangulate::CSectionMeshTriangulate(double dMinAngle, double dMaxArea)
: m_dMinAngle(dMinAngle)
, m_dMaxArea(dMaxArea)
{
}

CSectionMeshTriangulate::~CSectionMeshTriangulate(void)
{
}

CSectionMeshTriangulate::CSectionMeshTriangulate(TiXmlElement &Element)
{
	Element.Attribute("MinAngle", &m_dMinAngle);
	Element.Attribute("MaxArea", &m_dMaxArea);
}

void CSectionMeshTriangulate::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CSectionMesh::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("MinAngle", stringify(m_dMinAngle));
	Element.SetAttribute("MaxArea", stringify(m_dMaxArea));
}

bool CSectionMeshTriangulate::CreateMesh(const vector<XY> &Section) const
{
	char szSwitches[128];

	double dSectionArea = CSection::GetArea(Section);

	double dMaxArea = dSectionArea*m_dMaxArea;

#ifdef _DEBUG
	sprintf(szSwitches, "pzPBq%fa%f", m_dMinAngle, dMaxArea);
#else // _DEBUG
	sprintf(szSwitches, "pzQPBq%fa%f", m_dMinAngle, dMaxArea);
#endif // _DEBUG

	triangulateio TriangleInput;
	triangulateio TriangleOutput;
	memset(&TriangleInput, 0, sizeof(TriangleInput));
	memset(&TriangleOutput, 0, sizeof(TriangleOutput));

	TriangleInput.pointlist = new REAL [Section.size()*2];
	TriangleInput.numberofpoints = (int)Section.size();

	int i;
	for (i=0; i<(int)Section.size(); ++i)
	{
		TriangleInput.pointlist[i*2] = Section[i].x;
		TriangleInput.pointlist[i*2+1] = Section[i].y;
	}

	TriangleInput.segmentlist = new int [Section.size()*2];
	TriangleInput.numberofsegments = (int)Section.size();

	for (i=0; i<(int)Section.size(); ++i)
	{
		TriangleInput.segmentlist[i*2] = i;
		TriangleInput.segmentlist[i*2+1] = (i+1)%Section.size();
	}

	triangulate(szSwitches, &TriangleInput, &TriangleOutput, NULL);

	delete [] TriangleInput.pointlist;
	delete [] TriangleInput.segmentlist;

	m_Mesh.Clear();

	XYZ Point;
	for (i=0; i<TriangleOutput.numberofpoints; ++i)
	{
		Point.x = TriangleOutput.pointlist[i*2];
		Point.y = TriangleOutput.pointlist[i*2+1];
		m_Mesh.AddNode(Point);
	}

	for (i=0; i<TriangleOutput.numberoftriangles; ++i)
	{
		m_Mesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i*3]);
		m_Mesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i*3+1]);
		m_Mesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i*3+2]);
	}

	trifree(TriangleOutput.pointlist);
	trifree(TriangleOutput.trianglelist);
	return true;
}

CMesh CSectionMeshTriangulate::GetSimpleMesh(const vector<XY> &Section)
{
	CMesh Mesh;

	vector<int> ClosedLoop;

	vector<XY>::const_iterator itPoint;
	int i;
	for (i=0, itPoint = Section.begin(); itPoint != Section.end(); ++itPoint, ++i)
	{
		Mesh.AddNode(XYZ(itPoint->x, itPoint->y, 0));
		ClosedLoop.push_back(i);
	}

	Mesh.MeshClosedLoop(XYZ(0, 0, -1), ClosedLoop);

	return Mesh;
}


