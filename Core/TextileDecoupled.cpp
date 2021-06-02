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

/*void CTextileDecoupled::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile3DWeave::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("BinderLayers", m_iNumBinderLayers);
}*/

void CTextileDecoupled::SetBinderPosition(int x, int y, vector<int> zOffsets)
{
	vector<PATTERN3D> &Cell = GetCell(x, y);
	int size = Cell.size();
	vector<int>::iterator itzOffsets;
	int BinderPos;
	vector<int> BinderPositions;

	for (itzOffsets = zOffsets.begin(); itzOffsets != zOffsets.end(); itzOffsets++)
	{
		BinderPos = (size - 1) - *itzOffsets * 2;
		BinderPositions.push_back(BinderPos);
	}

	/*int MaxBinderPos = (size - 1) - (zOffsets * 2);   // *2 because each offset shifts by two cell positions
	int MinBinderPos = (size - 1) - (zOffsets + m_iNumBinderLayers - 1) * 2;
	if (MinBinderPos < 0)
	{
		TGERROR("Unable to set binder positions, lowest binder position too low for number of weft yarns");
		return;
	}
	if (MaxBinderPos > size - 1)
	{
		TGERROR("Unable to set binder positions, upper binder position too high for number of weft yarns");
		return;
	}*/

	// Set x yarn in each of positions between Max and Min binder pos
	for (int j = 0; j < size; j += 2)
	{
		if (count(BinderPositions.begin(), BinderPositions.end(), j ))
		{
			Cell[j] = PATTERN3D_XYARN;
		}
		else
		{
			Cell[j] = PATTERN3D_NOYARN;
		}
	}
}
