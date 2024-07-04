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
#include "TextileWeftKnit.h"
#include "SectionEllipse.h"
#include "DomainPlanes.h"

using namespace TexGen;

CTextileWeftKnit::CTextileWeftKnit(int iWales, int iCourses, double dWaleHeight, double dLoopHeight , double dCourseWidth, double dYarnThickness)
: CTextileKnit(iWales, iCourses, dYarnThickness)
, m_dWaleHeight(dWaleHeight)
, m_dLoopHeight(dLoopHeight)
, m_dCourseWidth(dCourseWidth)
, m_iLoopModel(RAVANDI_2021)
{
	
}

CTextileWeftKnit::~CTextileWeftKnit(void)
{
}

CTextileWeftKnit::CTextileWeftKnit(TiXmlElement &Element)
: CTextileKnit(Element)
{	
	Element.Attribute("WaleHeight", &m_dWaleHeight);
	Element.Attribute("LoopHeight", &m_dLoopHeight);
	Element.Attribute("CourseWidth", &m_dCourseWidth);	
}

void CTextileWeftKnit::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextileKnit::PopulateTiXmlElement(Element, OutputType);
	
	Element.SetAttribute("WaleHeight", stringify(m_dWaleHeight));
	Element.SetAttribute("LoopHeight", stringify(m_dLoopHeight));
	Element.SetAttribute("CourseWidth", stringify(m_dCourseWidth));	
}

bool CTextileWeftKnit::BuildTextile() const
{
	if (m_iLoopModel == RAVANDI_2021)
	{
		BuildTextileUsingRavandiLoopModel();
	}

	return true;
}


void CTextileWeftKnit::BuildTextileUsingRavandiLoopModel() const
{
	m_Yarns.clear();

	TGLOGINDENT("Building textile weft knit \"" << GetName() << "\"");

	vector<int> Yarns;

	Yarns.push_back(AddYarn(CYarn()));

	AddOneLoopToYarn();

	CSectionEllipse Section(m_dYarnThickness, m_dYarnThickness);
	m_Yarns[0].AssignSection(CYarnSectionConstant(Section));

	m_Yarns[0].SetResolution(40);  // Initialise with default value. If being created with wizard will then set resolution to selected value
	m_Yarns[0].AssignInterpolation(CInterpolationCubic());

	AddRepeats();
}

string CTextileWeftKnit::GetDefaultName() const
{
	return "WeftKnit(W:" + stringify(m_iWales) + ",C:" + stringify(m_iCourses) + ")";
}

void CTextileWeftKnit::RefineTextile(bool bCorrectWidths, bool bCorrectInterference, bool bPeriodic)
{

}

void CTextileWeftKnit::AddOneLoopToYarn() const
{
	double x, y, z;

	// Node 1
	x = 0.0;
	y = 0.0;
	z = m_dYarnThickness / 2.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 2
	x = ((m_dCourseWidth + (2.0 * m_dYarnThickness)) / 4.0);
	y = (m_dLoopHeight - m_dWaleHeight) / 2.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 3
	x = m_dCourseWidth / 4.0;
	y = m_dLoopHeight / 2.0;
	z = -m_dYarnThickness / 2.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 4
	x = ((m_dCourseWidth - (2.0 * m_dYarnThickness)) / 4.0);
	y = (m_dLoopHeight + m_dWaleHeight) / 2.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 5
	x = m_dCourseWidth / 2.0;
	y = m_dLoopHeight;
	z = m_dYarnThickness / 2.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 6
	x = ((3.0 * m_dCourseWidth) + (2.0 * m_dYarnThickness)) / 4.0;
	y = (m_dLoopHeight + m_dWaleHeight) / 2.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 7
	x = (3.0 * m_dCourseWidth) / 4.0;
	y = m_dLoopHeight / 2.0;
	z = -m_dYarnThickness / 2.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 8
	x = ((3.0 * m_dCourseWidth) - (2.0 * m_dYarnThickness)) / 4.0;
	y = (m_dLoopHeight - m_dWaleHeight) / 2.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 9
	x = m_dCourseWidth;
	y = 0.0;
	z = m_dYarnThickness / 2.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));
}

void CTextileWeftKnit::AddRepeats() const
{
	m_Yarns[0].AddRepeat(XYZ(m_dCourseWidth, 0.0, 0.0));
	m_Yarns[0].AddRepeat(XYZ(0.0, m_dWaleHeight, 0.0));
}


CDomainPlanes CTextileWeftKnit::GetDefaultDomain()
{
	XYZ Min, Max;

	Min.x = 0.0;
	Min.y = m_Yarns[0].GetNode(2)->GetPosition().y;
	Min.z = -m_dYarnThickness;

	Max.x = m_dCourseWidth * m_iCourses;
	Max.y = Min.y + (double)m_iWales * m_dWaleHeight;
	Max.z = m_dYarnThickness;

	return CDomainPlanes(Min, Max);
}

void CTextileWeftKnit::AssignDefaultDomain()
{
	CDomainPlanes Domain = GetDefaultDomain();
	AssignDomain(Domain);
}

void CTextileWeftKnit::SetLoopModel(LoopModel iLoopModel)
{
	m_iLoopModel = iLoopModel;
}