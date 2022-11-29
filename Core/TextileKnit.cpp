#include "PrecompiledHeaders.h"
#include "TextileKnit.h"

using namespace TexGen;

CTextileKnit::CTextileKnit()
{

}

CTextileKnit::~CTextileKnit(void)
{

}

CTextileKnit::CTextileKnit(TiXmlElement &Element) : CTextile(Element)
{

}

void CTextileKnit::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile::PopulateTiXmlElement(Element, OutputType);
}

bool CTextileKnit::BuildTextile() const
{
	return true;
}

