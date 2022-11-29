#include "PrecompiledHeaders.h"
#include "TextileWeftKnit.h"

using namespace TexGen;

CTextileWeftKnit::CTextileWeftKnit()
{

}

CTextileWeftKnit::~CTextileWeftKnit(void)
{

}

CTextileWeftKnit::CTextileWeftKnit(TiXmlElement &Element) : CTextileKnit(Element)
{

}

void CTextileWeftKnit::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextileKnit::PopulateTiXmlElement(Element, OutputType);
}

bool CTextileWeftKnit::BuildTextile() const
{
	return true;
}
