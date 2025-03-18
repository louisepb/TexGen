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


//This Program uses is called TextileWeftKnit but uses the TextileWarpKnit values (Not Permanent)
using namespace TexGen;

//For Now:
//iWales = f = lower loop width
//iCourses = g = upper loop width
//dWaleHeight = D = wale height
//dLoopHeight = e = added loop height (not same)
//dCourseWidth = C = course w
//dYarnThickness = d = thickness


CTextileWeftKnit::CTextileWeftKnit(double iWales, double iCourses, double dWaleHeight, double dLoopHeight , double dCourseWidth, double dYarnThickness)
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

	double XDelta = m_dCourseWidth + 2.0 * m_dYarnThickness;
	double YDelta = m_dWaleHeight;
	double ZDelta = m_dYarnThickness;
	double d = m_dYarnThickness;
	double C = m_dCourseWidth;
	double D = m_dWaleHeight;
	double e = m_dLoopHeight;
	double f = m_iWales;
	double g = m_iCourses;
	double H = (m_dWaleHeight - m_dLoopHeight + (1.0 / 3.0)*m_dLoopHeight);

	// Node 1
	x = -f;
	y = -H;
	z = d;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 2
	x = -g;
	y = e-D;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 3
	x = (-0.78*C/2);
	y = -H/2.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 4
	x = -(C / 2.0);
	y = 0.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 5
	x = (-0.866*(C / 2.0));
	y = (0.5*(C / 2.0));
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 6
	x = (-0.5*(C / 2.0));
	y = (0.866*(C / 2.0));
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 7
	x = 0.0;
	y = e;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 8
	x = (0.5*(C / 2.0));
	y = (0.866*(C / 2.0));
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 9
	x = (0.866*(C / 2.0));
	y = (0.5*(C / 2.0));
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 10
	x = (C / 2.0);
	y = 0.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 11
	x = 0.78*(C / 2.0);
	y = -H/2.0;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 12
	x = g;
	y = e-D;
	z = 0.0;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 13
	x = f;
	y = -H;
	z = 2.0*d;
	m_Yarns[0].AddNode(XYZ(x, y, z));


	// Node 14
	x = f-XDelta;
	y = -H+YDelta;
	z = d-ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 15
	x = g-XDelta;
	y = e-D+YDelta;
	z = -ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 16
	x = (0.78*C/2.0)-XDelta;
	y = -H/2.0+YDelta;
	z = 0.0-ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 17
	x = C/2.0-XDelta;
	y = YDelta;
	z = -ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 18
	x = 0.866*C/2.0-XDelta;
	y = 0.5*C/2.0+YDelta;
	z = -ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 19
	x = 0.5*C/2.0-XDelta;
	y = 0.866*C/2.0+YDelta;
	z = -ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 20
	x = -XDelta;
	y = e+YDelta;
	z = 0.0-ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 21
	x = -0.5*C/2.0-XDelta;
	y = 0.866*C/2.0+YDelta;
	z = -ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 22
	x = -0.866*C/2.0-XDelta;
	y = -0.5*C/2.0+YDelta;
	z = -ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 23
	x = -C/2.0-XDelta;
	y = YDelta;
	z = -ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 24
	x = -0.78*C/2.0-XDelta;
	y = -H/2.0+YDelta;
	z = -ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 25
	x = -g - XDelta;
	y = e-D + YDelta;
	z = -ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 26
	x = -f - XDelta;
	y = -H+YDelta;
	z = 2*d-ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));

	// Node 27
	x = -f;
	y = -H+2*YDelta;
	z = d-2*ZDelta;
	m_Yarns[0].AddNode(XYZ(x, y, z));
}

void CTextileWeftKnit::AddRepeats() const
{
	m_Yarns[0].AddRepeat(XYZ(0.0, 2.0*m_dWaleHeight, -2.0*m_dYarnThickness));
	m_Yarns[0].AddRepeat(XYZ(2.0*(m_dCourseWidth + 2 * m_dYarnThickness), 0.0, 0.0));
}


CDomainPlanes CTextileWeftKnit::GetDefaultDomain()
{
	XYZ Min, Max;

	Min.x = -(m_dCourseWidth + 2.0 * m_dYarnThickness + m_dCourseWidth / 2.0) - m_dYarnThickness /2.0;
	Min.y = -((m_dWaleHeight - ((2.0 * m_dLoopHeight) / 3.0)) + m_dCourseWidth / 2.0 /2);
	Min.z = -4.0 * m_dYarnThickness;

	Max.x = m_dCourseWidth + 2.0 * m_dYarnThickness + m_dCourseWidth / 2.0 + m_dYarnThickness / 2.0;
	Max.y = (m_dWaleHeight - ((2.0 * m_dLoopHeight) / 3.0)) + m_dCourseWidth;
	Max.z = 4.0 * m_dYarnThickness;

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