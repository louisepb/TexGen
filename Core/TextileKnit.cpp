/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2024 Louise Brown

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
#include "TextileKnit.h"

using namespace TexGen;

CTextileKnit::CTextileKnit(int iWales, int iCourses, double dYarnThickness) :
m_iWales(iWales)
, m_iCourses(iCourses)
, m_dYarnThickness(dYarnThickness)
{

}

CTextileKnit::~CTextileKnit(void)
{

}


CTextileKnit::CTextileKnit(TiXmlElement &Element) : CTextile(Element)
{
	Element.Attribute("Wales", &m_iWales);
	Element.Attribute("Courses", &m_iCourses);
	Element.Attribute("YarnThickness", &m_dYarnThickness);
}

void CTextileKnit::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile::PopulateTiXmlElement(Element, OutputType);

	Element.SetAttribute("Wales", m_iWales);
	Element.SetAttribute("Courses", m_iCourses);
	Element.SetAttribute("YarnThickness", stringify(m_dYarnThickness));
}

bool CTextileKnit::BuildTextile() const
{
	return true;
}

