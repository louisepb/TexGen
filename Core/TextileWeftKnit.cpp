#include "PrecompiledHeaders.h"
#include "TextileWeftKnit.h"
#include "SectionEllipse.h"
#include "DomainPlanes.h"

using namespace TexGen;

CTextileWeftKnit::CTextileWeftKnit(int iWales, int iCourses, double dWaleHeight, double dLoopHeight , double dCourseWidth, double dYarnThickness) : CTextileKnit()
, m_iWales(iWales)
, m_iCourses(iCourses)
, m_dWaleHeight(dWaleHeight)
, m_dLoopHeight(dLoopHeight)
, m_dCourseWidth(dCourseWidth)
, m_dYarnThickness(dYarnThickness)
, m_iNumSectionPoints(20)
, m_iNumSlaveNodes(50)
{
	
}

CTextileWeftKnit::~CTextileWeftKnit(void)
{
}

CTextileWeftKnit::CTextileWeftKnit(TiXmlElement &Element)
: CTextileKnit(Element)
{
	Element.Attribute("Wales", &m_iWales);
	Element.Attribute("Courses", &m_iCourses);
	Element.Attribute("WaleHeight", &m_dWaleHeight);
	Element.Attribute("LoopHeight", &m_dLoopHeight);
	Element.Attribute("CourseWidth", &m_dCourseWidth);
	Element.Attribute("YarnThickness", &m_dYarnThickness);
	Element.Attribute("NumSectionPoints", &m_iNumSectionPoints);
	Element.Attribute("NumSlaveNodes", &m_iNumSlaveNodes);
}

void CTextileWeftKnit::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextileKnit::PopulateTiXmlElement(Element, OutputType);

	Element.SetAttribute("Wales", m_iWales);
	Element.SetAttribute("Courses", m_iCourses);
	Element.SetAttribute("WaleHeight", stringify(m_dWaleHeight));
	Element.SetAttribute("LoopHeight", stringify(m_dLoopHeight));
	Element.SetAttribute("CourseWidth", stringify(m_dCourseWidth));
	Element.SetAttribute("YarnThickness", stringify(m_dYarnThickness));
	Element.SetAttribute("NumSectionPoints", m_iNumSectionPoints);
	Element.SetAttribute("NumSlaveNodes", m_iNumSlaveNodes);
}

bool CTextileWeftKnit::BuildTextile() const
{
	m_Yarns.clear();

	TGLOGINDENT("Building textile weft knit \"" << GetName() << "\"");

	vector<int> Yarns;

	Yarns.push_back(AddYarn(CYarn()));

	AddOneLoopToYarn();

	CSectionEllipse Section(m_dYarnThickness, m_dYarnThickness);
	m_Yarns[0].AssignSection(CYarnSectionConstant(Section));

	m_Yarns[0].SetResolution(m_iNumSlaveNodes, m_iNumSectionPoints);
	m_Yarns[0].AssignInterpolation(CInterpolationBezier());
	
	AddRepeats();

	return true;
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
	Min.y = -m_dYarnThickness / 2.0;
	Min.z = -m_dYarnThickness;

	Max.x = m_dCourseWidth * m_iCourses;
	Max.y = (m_dLoopHeight * double(m_iWales)) - ((m_dLoopHeight - m_dWaleHeight) * double(m_iWales - 1)) + (m_dYarnThickness / 2.0);
	Max.z = m_dYarnThickness;

	return CDomainPlanes(Min, Max);
}

void CTextileWeftKnit::AssignDefaultDomain()
{
	CDomainPlanes Domain = GetDefaultDomain();
	AssignDomain(Domain);
}