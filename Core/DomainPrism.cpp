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
	m_Yarn.SetResolution(2, 40);
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
}