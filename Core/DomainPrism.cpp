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
#include "DomainPrism.h"
#include "Yarn.h"
#include "SectionPolygon.h"
using namespace TexGen;

CDomainPrism::CDomainPrism(const vector<XY> &Points, XYZ &start, XYZ &end)
{
	CNode Node = CNode(start);
	m_Yarn.AddNode(Node);
	Node = CNode(end);
	m_Yarn.AddNode(Node);
	m_Yarn.AssignSection(CYarnSectionConstant(CSectionPolygon(Points)));
	m_Yarn.SetResolution(2, 8);
	BuildMesh();
}

CDomainPrism::~CDomainPrism(void)
{
}

CDomainPrism::CDomainPrism(TiXmlElement &Element)
	: CDomain(Element)
{
	TiXmlElement* pYarn = Element.FirstChildElement("Yarn");
	m_Yarn = CYarn(*pYarn);

	if (m_Mesh.GetNumNodes() == 0)
	{
		BuildMesh();
	}
}

void CDomainPrism::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CDomain::PopulateTiXmlElement(Element, OutputType);
	TiXmlElement Yarn("Yarn");
	m_Yarn.PopulateTiXmlElement(Yarn, OutputType);
	Element.InsertEndChild(Yarn);
}

void CDomainPrism::BuildMesh()
{
	m_Mesh.Clear();
	m_Yarn.AddSurfaceToMesh( m_Mesh );
	m_Mesh.MergeNodes();
	m_Mesh.SaveToVTK("DomainMesh");
}

void CDomainPrism::GeneratePlanes()
{
	if (m_Mesh.GetNumNodes() == 0)
	{
		BuildMesh();
	}

	list<int>::iterator itInt;
	list<int>::iterator itStart;
	XYZ points[4];
	m_ElementPlanes.clear();

	list<int> &QuadIndices = m_Mesh.GetIndices(CMesh::QUAD);
	for (itInt = QuadIndices.begin(); itInt != QuadIndices.end(); )
	{
		itStart = itInt;

		points[0] = m_Mesh.GetNode(*(itInt++));
		points[1] = m_Mesh.GetNode(*(itInt++));
		points[2] = m_Mesh.GetNode(*(itInt++));
		points[3] = m_Mesh.GetNode(*(itInt++));

		PLANE ElementPlane;
		if (GetPlane( points, ElementPlane))
			m_ElementPlanes.push_back(ElementPlane);
	}

	list<int> &TriIndices = m_Mesh.GetIndices(CMesh::TRI);
	for (itInt = TriIndices.begin(); itInt != TriIndices.end(); )
	{
		itStart = itInt;

		points[0] = m_Mesh.GetNode(*(itInt++));
		points[1] = m_Mesh.GetNode(*(itInt++));
		points[2] = m_Mesh.GetNode(*(itInt++));

		PLANE ElementPlane;
		if (GetPlane(points, ElementPlane))
			m_ElementPlanes.push_back(ElementPlane);
	}

	list<int> &Indices = m_Mesh.GetIndices(CMesh::POLYGON);
	list<int>::iterator itIndices;
	int StartIndex;
	list<int>::iterator itStartIndex;

	for (itIndices = Indices.begin(); itIndices != Indices.end(); )
	{
		StartIndex = (*(itIndices));
		itStartIndex = itIndices;
		vector<int> PolygonIndex;
		XYZ p;

		int i = 0, iPoints = 0;
		
		do {
			if (i < 3)
			{
				points[i] = m_Mesh.GetNode(*(itIndices));
			}
			++i;
			++itIndices;
			if (i == 3)
			{
				PLANE ElementPlane;
				if (GetPlane(points, ElementPlane))
					m_ElementPlanes.push_back(ElementPlane);
			}
		} while ((*itIndices) != StartIndex);
		++itIndices;
	}
	RemoveDuplicatePlanes();
}

bool CDomainPrism::GetPlane(XYZ *p, PLANE &plane)
{
	plane.Normal = CrossProduct(p[2] - p[0], p[1] - p[0]);

	if (GetLength(plane.Normal))
	{
		Normalise(plane.Normal);
		plane.d = DotProduct(plane.Normal, p[0] - XYZ(0, 0, 0));
		return(true);
	}
	return(false);
}

void CDomainPrism::RemoveDuplicatePlanes()
{
	vector<PLANE>::iterator itPlanes1, itPlanes2;

	for (itPlanes1 = m_ElementPlanes.begin(); itPlanes1 != m_ElementPlanes.end(); ++itPlanes1)
	{
		for (itPlanes2 = itPlanes1 + 1; itPlanes2 != m_ElementPlanes.end(); )
		{
			if (PlaneEqual(*itPlanes1, *itPlanes2))
			{
				itPlanes2 = m_ElementPlanes.erase(itPlanes2);
			}
			else
				++itPlanes2;
		}
	}
}

bool CDomainPrism::PlaneEqual(PLANE Plane1, PLANE Plane2)
{
	if (fabs(GetLength(Plane1.Normal, Plane2.Normal)) > 1e-9)
		return false;
	if (Plane1.d != Plane2.d)
		return false;
	return true;
}