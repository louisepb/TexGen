#include "PrecompiledHeaders.h"
#include "TextileDecoupled.h"

using namespace TexGen;

CTextileDecoupled::CTextileDecoupled(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing, double dXHeight, double dYHeight, int iNumBinderLayers, bool bShapeBinders)
	: CTextileLayerToLayer(iNumXYarns, iNumYYarns, dXSpacing, dYSpacing, dXHeight, dYHeight, iNumBinderLayers, bShapeBinders)
{
	m_iNumBinderLayers = iNumBinderLayers;
	m_bShapeBinders = bShapeBinders;
}

CTextileDecoupled::CTextileDecoupled(TiXmlElement &Element)
	: CTextileLayerToLayer(Element)
{
	Element.Attribute("BinderLayers", &m_iNumBinderLayers);
}

CTextileDecoupled::~CTextileDecoupled()
{
}

void CTextileDecoupled::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile3DWeave::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("BinderLayers", m_iNumBinderLayers);
}

void CTextileDecoupled::SetBinderPosition(int x, int y, vector<int> zOffsets)
{
}
