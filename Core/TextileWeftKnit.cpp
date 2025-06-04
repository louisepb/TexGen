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
#include "TextileWeftKnit.h"
#include "SectionEllipse.h"
#include "DomainPlanes.h"


//This Program is called TextileWeftKnit but uses the TextileWarpKnit values (Not Permanent)
using namespace TexGen;

CTextileWeftKnit::CTextileWeftKnit(int iWales, int iCourses, double dWaleHeight, double dNeckWidth , double dCourseWidth, double dYarnThickness)
: CTextileKnit(iWales, iCourses, dYarnThickness)
, m_dWaleHeight(dWaleHeight)
, m_dNeckWidth(dNeckWidth)
, m_dCourseWidth(dCourseWidth)
, m_iLoopModel(DEFAULT_2021)
{
	
}

CTextileWeftKnit::~CTextileWeftKnit(void)
{
}

CTextileWeftKnit::CTextileWeftKnit(TiXmlElement &Element)
: CTextileKnit(Element)
{	
	Element.Attribute("WaleHeight", &m_dWaleHeight);
	Element.Attribute("NeckWidth", &m_dNeckWidth);
	Element.Attribute("CourseWidth", &m_dCourseWidth);	
}

void CTextileWeftKnit::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextileKnit::PopulateTiXmlElement(Element, OutputType);
	
	Element.SetAttribute("WaleHeight", stringify(m_dWaleHeight));
	Element.SetAttribute("NeckWidth", stringify(m_dNeckWidth));
	Element.SetAttribute("CourseWidth", stringify(m_dCourseWidth));	
}

bool CTextileWeftKnit::BuildTextile() const
{
	if (m_iLoopModel == DEFAULT_2021)
	{
		BuildTextileUsingDefaultLoopModel();
	}
	else if (m_iLoopModel == JI_2020)
	{
		BuildTextileUsingJiLoopModel();
	}

	return true;
}


void CTextileWeftKnit::BuildTextileUsingDefaultLoopModel() const
{
	m_Yarns.clear();

	TGLOGINDENT("Building textile weft knit \"" << GetName() << "\"");

	vector<int> Yarns;

	Yarns.push_back(AddYarn(CYarn()));

	AddOneDefaultLoopToYarn();

	CSectionEllipse Section(m_dYarnThickness, m_dYarnThickness);
	m_Yarns[0].AssignSection(CYarnSectionConstant(Section));

	m_Yarns[0].SetResolution(20);  // Initialise with default value. If being created with wizard will then set resolution to selected value
	m_Yarns[0].AssignInterpolation(CInterpolationBezier());

	AddRepeats();
	//Yarns.push_back(AddYarn(CYarn()));
	//AddTranslate();
}

void CTextileWeftKnit::BuildTextileUsingJiLoopModel() const
{
	m_Yarns.clear();

	TGLOGINDENT("Building textile weft knit \"" << GetName() << "\"");

	vector<int> Yarns;

	Yarns.push_back(AddYarn(CYarn()));

	AddOneJiLoopToYarn();

	CSectionEllipse Section(m_dYarnThickness, m_dYarnThickness);
	m_Yarns[0].AssignSection(CYarnSectionConstant(Section));

	m_Yarns[0].SetResolution(20);  // Initialise with default value. If being created with wizard will then set resolution to selected value
	m_Yarns[0].AssignInterpolation(CInterpolationBezier());

	AddRepeats();
	//Yarns.push_back(AddYarn(CYarn()));
	//AddTranslate();
}

string CTextileWeftKnit::GetDefaultName() const
{
	return "WeftKnit(W:" + stringify(m_iWales) + ",C:" + stringify(m_iCourses) + ")";
}

void CTextileWeftKnit::RefineTextile(bool bCorrectWidths, bool bCorrectInterference, bool bPeriodic)
{

}

void CTextileWeftKnit::AddOneDefaultLoopToYarn() const
{
	double x, y, z;

	// Constants to help calculate node positions
	double dXDelta = - (m_dCourseWidth + 5.0 * m_dYarnThickness);
	double dYDelta = m_dWaleHeight-2.0*m_dYarnThickness;
	double dZDelta = 0;
	

	// Node 1
	x = m_dNeckWidth * 0.5 - m_dCourseWidth / 2.0;
	y = (m_dCourseWidth / 2.0) - m_dWaleHeight - 1.5*m_dYarnThickness;
	z = 2 * m_dYarnThickness;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 2
	x = m_dNeckWidth * 0.5 - m_dCourseWidth / 2.0;
	y = (m_dCourseWidth / 2.0) - m_dWaleHeight;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 3
	x = -m_dCourseWidth / 2.0;
	y = 0.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 4
	x = -0.866*m_dCourseWidth / 2.0;
	y = 0.5*m_dCourseWidth / 2.0;
	z = m_dYarnThickness;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 5
	x = -0.5*m_dCourseWidth / 2.0;
	y = 0.866*m_dCourseWidth / 2.0;
	z = m_dYarnThickness;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 6
	x = 0.0;
	y = m_dCourseWidth / 2.0;
	z = m_dYarnThickness;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 7
	x = 0.5*m_dCourseWidth / 2.0;
	y = 0.866*m_dCourseWidth / 2.0;
	z = m_dYarnThickness;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 8
	x = 0.866*m_dCourseWidth / 2.0;
	y = 0.5*m_dCourseWidth / 2.0;
	z = m_dYarnThickness;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 9
	x = m_dCourseWidth / 2.0;
	y = 0.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 10
	x = 0.866*m_dCourseWidth / 2.0;
	y = -0.5*m_dCourseWidth / 2.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 11
	x = m_dNeckWidth * 0.5 - (m_dCourseWidth / 2.0) + m_dNeckWidth;
	y = (m_dCourseWidth / 2.0) - m_dWaleHeight;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 12
	x = m_dNeckWidth * 0.5 - (m_dCourseWidth / 2.0) + m_dNeckWidth - 0.35*m_dNeckWidth;
	y = (m_dCourseWidth / 2.0) - m_dWaleHeight - 0.35*m_dNeckWidth;
	z = 2 * m_dYarnThickness;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 13
	x = dXDelta - (m_dNeckWidth*0.5 - m_dCourseWidth / 2.0);
	y = dYDelta + ((m_dCourseWidth / 2) - m_dWaleHeight - 1.5*m_dYarnThickness);
	z = dZDelta + (2 * m_dYarnThickness);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 14
	x = dXDelta - (m_dNeckWidth*0.5 - m_dCourseWidth / 2.0);
	y = dYDelta + ((m_dCourseWidth / 2.0) - m_dWaleHeight);
	z = dZDelta + (0);
	m_Yarns[0].AddNode(XYZ(x, y, z));


	// Node 15
	x = dXDelta - (-m_dCourseWidth / 2.0);
	y = dYDelta + (0);
	z = dZDelta + (0);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 16
	x = dXDelta - (-0.866*m_dCourseWidth / 2.0);
	y = dYDelta + (0.5*m_dCourseWidth / 2.0);
	z = dZDelta + (m_dYarnThickness);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 17
	x = dXDelta - (-0.5*m_dCourseWidth / 2.0);
	y = dYDelta + (0.866*m_dCourseWidth / 2.0);
	z = dZDelta + (m_dYarnThickness);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 18
	x = dXDelta - (0);
	y = dYDelta + (m_dCourseWidth / 2.0);
	z = dZDelta + (m_dYarnThickness);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 19
	x = dXDelta - (0.5*m_dCourseWidth / 2.0);
	y = dYDelta + (0.866*m_dCourseWidth / 2.0);
	z = dZDelta + (m_dYarnThickness);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 20
	x = dXDelta - (0.866*m_dCourseWidth / 2.0);
	y = dYDelta + (0.5*m_dCourseWidth / 2.0);
	z = dZDelta + (m_dYarnThickness);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 21
	x = dXDelta - (m_dCourseWidth / 2.0);
	y = dYDelta + (0);
	z = dZDelta + (m_dYarnThickness);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 22
	x = dXDelta - (0.866*m_dCourseWidth / 2.0);
	y = dYDelta + (-0.5*m_dCourseWidth / 2.0);
	z = dZDelta + (0);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 23
	x = dXDelta - (m_dNeckWidth*0.5 - (m_dCourseWidth / 2.0) + m_dNeckWidth);
	y = dYDelta + ((m_dCourseWidth / 2.0) - m_dWaleHeight);
	z = dZDelta + (0);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 24
	x = dXDelta - (m_dNeckWidth*0.5 - (m_dCourseWidth / 2.0) + m_dNeckWidth - 0.35*m_dNeckWidth);
	y = dYDelta + ((m_dCourseWidth / 2.0) - m_dWaleHeight - 0.35*m_dNeckWidth);
	z = dZDelta + (2 * m_dYarnThickness);
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 25
	x = m_dNeckWidth * 0.5 - m_dCourseWidth / 2.0;
	y = 2.0 * dYDelta + ((m_dCourseWidth / 2.0) - m_dWaleHeight - 1.5*m_dYarnThickness);
	z = 2.0 * m_dYarnThickness;
	m_Yarns[0].AddNode(XYZ(x, y, z));
}


void CTextileWeftKnit::AddOneJiLoopToYarn() const
{
	double x, y, z;

	// Constants to help calculate node positions
	double dXDelta = m_dCourseWidth + 2.0 * m_dYarnThickness;
	double dYDelta = m_dWaleHeight;
	double dZDelta = m_dYarnThickness;
	double dTotalHeight = m_dWaleHeight + m_dCourseWidth / 6.0;
	//double d = m_dYarnThickness;
	//double C = m_dCourseWidth;
	//double D = m_dWaleHeight;
	//double e = m_dCourseWidth/2.0;
	//double f = m_dNeckWidth;
	//double g = m_dNeckWidth*1.2;


	// Node 1
	x = -m_dNeckWidth;
	y = (m_dCourseWidth / 2.0) - dTotalHeight;
	z = m_dYarnThickness;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 2
	x = -(m_dNeckWidth * 1.2);
	y = (m_dCourseWidth / 2.0) - m_dWaleHeight;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 3
	x = (-0.78*m_dCourseWidth / 2);
	y = ((m_dCourseWidth / 2.0) - dTotalHeight) / 2.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 4
	x = -(m_dCourseWidth / 2.0);
	y = 0.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 5
	x = (-0.866*(m_dCourseWidth / 2.0));
	y = (0.5*(m_dCourseWidth / 2.0));
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 6
	x = (-0.5*(m_dCourseWidth / 2.0));
	y = (0.866*(m_dCourseWidth / 2.0));
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 7
	x = 0.0;
	y = (m_dCourseWidth / 2.0);
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 8
	x = (0.5*(m_dCourseWidth / 2.0));
	y = (0.866*(m_dCourseWidth / 2.0));
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 9
	x = (0.866*(m_dCourseWidth / 2.0));
	y = (0.5*(m_dCourseWidth / 2.0));
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 10
	x = (m_dCourseWidth / 2.0);
	y = 0.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 11
	x = 0.78*(m_dCourseWidth / 2.0);
	y = ((m_dCourseWidth / 2.0) - dTotalHeight) / 2.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 12
	x = (m_dNeckWidth * 1.2);
	y = (m_dCourseWidth / 2.0) - m_dWaleHeight;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 13
	x = m_dNeckWidth;
	y = (m_dCourseWidth / 2.0) - dTotalHeight;
	z = 2.0*m_dYarnThickness;
	m_Yarns[0].AddNode(XYZ(x, y, z));


	// Node 14
	x = m_dNeckWidth - dXDelta;
	y = (m_dCourseWidth / 2.0) - dTotalHeight + dYDelta;
	z = m_dYarnThickness - dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 15
	x = (m_dNeckWidth * 1.2) - dXDelta;
	y = (m_dCourseWidth / 2.0) - m_dWaleHeight + dYDelta;
	z = -dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 16
	x = (0.78*m_dCourseWidth / 2.0) - dXDelta;
	y = ((m_dCourseWidth / 2.0) - dTotalHeight) / 2.0 + dYDelta;
	z = 0.0 - dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 17
	x = m_dCourseWidth / 2.0 - dXDelta;
	y = dYDelta;
	z = -dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 18
	x = 0.866*m_dCourseWidth / 2.0 - dXDelta;
	y = 0.5*m_dCourseWidth / 2.0 + dYDelta;
	z = -dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 19
	x = 0.5*m_dCourseWidth / 2.0 - dXDelta;
	y = 0.866*m_dCourseWidth / 2.0 + dYDelta;
	z = -dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 20
	x = -dXDelta;
	y = (m_dCourseWidth / 2.0) + dYDelta;
	z = 0.0 - dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 21
	x = -0.5*m_dCourseWidth / 2.0 - dXDelta;
	y = 0.866*m_dCourseWidth / 2.0 + dYDelta;
	z = -dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 22
	x = -0.866*m_dCourseWidth / 2.0 - dXDelta;
	y = (0.5*m_dCourseWidth / 2.0) + dYDelta;
	z = -dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 23
	x = -m_dCourseWidth / 2.0 - dXDelta;
	y = dYDelta;
	z = -dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 24
	x = -0.78*m_dCourseWidth / 2.0 - dXDelta;
	y = ((m_dCourseWidth / 2.0) - dTotalHeight) / 2.0 + dYDelta;
	z = -dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 25
	x = -(m_dNeckWidth * 1.2) - dXDelta;
	y = (m_dCourseWidth / 2.0) - m_dWaleHeight + dYDelta;
	z = -dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 26
	x = -m_dNeckWidth - dXDelta;
	y = ((m_dCourseWidth / 2.0) - dTotalHeight) + dYDelta;
	z = 2 * m_dYarnThickness - dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 27
	x = -m_dNeckWidth;
	y = ((m_dCourseWidth / 2.0) - dTotalHeight) + 2 * dYDelta;
	z = m_dYarnThickness - 2 * dZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));
}

void CTextileWeftKnit::AddRepeats() const
{

	if (m_iLoopModel == DEFAULT_2021)
	{
		m_Yarns[0].AddRepeat(XYZ(0.0, 2.0*(m_dWaleHeight - 2.0 * m_dYarnThickness), 0.0));
		m_Yarns[0].AddRepeat(XYZ(2.0*(-(m_dCourseWidth + 5.0 * m_dYarnThickness)) , 0.0, 0.0));
	}
	else if (m_iLoopModel == JI_2020)
	{
		m_Yarns[0].AddRepeat(XYZ(0.0, 2.0*m_dWaleHeight, -2.0*m_dYarnThickness));
		m_Yarns[0].AddRepeat(XYZ(2.0*(m_dCourseWidth + 2.0 * m_dYarnThickness), 0.0, 0.0));
	}
}

void CTextileWeftKnit::AddTranslate() const
{
	// m_Yarns[0].Translate(XYZ(m_dCourseWidth + 2.0 * m_dYarnThickness, 0.0, 0.0)); Not working for some reason?
	//m_Yarns[0].AddRepeat(XYZ(m_dCourseWidth + 2.0 * m_dYarnThickness, 0.0, 0.0));
}


CDomainPlanes CTextileWeftKnit::GetDefaultDomain()
{
	XYZ Min, Max;

	Min.x = -(m_dCourseWidth + 2.0 * m_dYarnThickness + m_dCourseWidth / 2.0);
	Min.y = -((m_dWaleHeight + m_dCourseWidth / 6.0) - (m_dCourseWidth / 2.0) + m_dYarnThickness);
	Min.z = - ((m_iWales-2)*m_dYarnThickness + 2.5*m_dYarnThickness);

	Max.x = (m_iCourses-2)*(m_dCourseWidth + 2.0 * m_dYarnThickness)+(m_dCourseWidth / 2.0) + m_dYarnThickness/2.0;
	Max.y = (m_iWales - 2) * m_dWaleHeight + m_dWaleHeight + (m_dCourseWidth / 2.0) + (m_dYarnThickness / 2.0);
	Max.z = 3.0 * m_dYarnThickness;

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