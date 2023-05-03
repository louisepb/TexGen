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

