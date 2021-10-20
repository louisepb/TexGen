/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2021 Louise Brown
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
#include "TextileDecoupledLToL.h"

using namespace TexGen;

CTextileDecoupledLToL::CTextileDecoupledLToL(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing, double dXHeight, double dYHeight, int iNumBinderLayers, bool bShapeBinders)
	: CTextileLayerToLayer(iNumXYarns, iNumYYarns, dXSpacing, dYSpacing, dXHeight, dYHeight, iNumBinderLayers, bShapeBinders)
{
	
}

CTextileDecoupledLToL::CTextileDecoupledLToL(TiXmlElement &Element)
	: CTextileLayerToLayer(Element)
{
	//Element.Attribute("BinderLayers", &m_iNumBinderLayers);
}

CTextileDecoupledLToL::~CTextileDecoupledLToL()
{
}

void CTextileDecoupledLToL::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextileLayerToLayer::PopulateTiXmlElement(Element, OutputType);
}

void CTextileDecoupledLToL::SetBinderPosition(int x, int y, vector<int> zOffsets)
{
	vector<PATTERN3D> &Cell = GetCell(x, y);
	
	vector<int> BinderPositions;
	int size = Cell.size();
	vector<int>::iterator itzOffsets;
	int BinderPos;
	int LastPos = size;
	
	// Convert offsets into cell indices (cells numbered from bottom)
	for (itzOffsets = zOffsets.begin(); itzOffsets != zOffsets.end(); itzOffsets++)
	{
		BinderPos = (size - 1) - *itzOffsets * 2;
		if (BinderPos >= LastPos)
		{
			TGERROR("Binder offsets incorrectly ordered at position " << x <<", " << y << ", binder yarns positions not set");
			return;
		}	
		BinderPositions.push_back(BinderPos);
		LastPos = BinderPos;
	}

	// Set cells to binder yarn to correspond to offsets
	int BinderNum = 0;
	for (int j = size-1; j >= 0; j -= 2)
	{
		if (BinderNum < m_iNumBinderLayers && BinderPositions[BinderNum] == j)
		{
			Cell[j] = PATTERN3D_XYARN;
			BinderNum++;
		}
		else
		{
			Cell[j] = PATTERN3D_NOYARN;
		}
	}
}

bool CTextileDecoupledLToL::BuildTextile() const
{
	if (!BuildLayerToLayerTextile())
		return false;

	if (m_bShapeBinders)
		ShapeBinderYarns();
	CheckUpVectors();
	return true;
}

void CTextileDecoupledLToL::ShapeBinderYarns() const
{
	for (int j = 0; j < m_iNumXYarns; ++j)
	{
		if (IsBinderYarn(j))
		{
			for (int Height = 0; Height < m_iNumBinderLayers; ++Height)
			{
				int CurrentNode = 0;
				for (int i = 0; i < m_iNumYYarns; ++i)
				{
					CurrentNode = AddBinderNodes(CurrentNode, i, j, Height);
					CurrentNode++;
				}

			}
		}
	}
}

void CTextileDecoupledLToL::CheckUpVectors() const
{
	vector<int> BinderIndices;
	vector<int>::iterator itBinder;
	int BinderIndex;

	for (int j = 0; j < m_iNumXYarns; ++j)
	{
		if (IsBinderYarn(j))
		{
			for (int Height = 0; Height < m_iNumBinderLayers; ++Height)
			{
				const vector<PATTERN3D> &Cell = GetCell(0, j);
				int iIndex = FindBinderHeight(Cell, Height);
				BinderIndex = GetYarnIndex(0, j, iIndex);
				BinderIndices.push_back(BinderIndex);
			}
		}
	}

	for (itBinder = BinderIndices.begin(); itBinder != BinderIndices.end(); itBinder++)
	{
		CTextileLayerToLayer::CheckUpVectors(*itBinder, true, true);
	}
}

int CTextileDecoupledLToL::FindBinderHeight(const vector<PATTERN3D>& Cell, int Height) const
{
	int i = Cell.size() - 1;
	int CurrentHeight = 0;
	while (i > 0)
	{
		//finds top binder
		if (Cell[i] == PATTERN3D_XYARN)
		{
			if (CurrentHeight == Height)
				return i;
			CurrentHeight++;
		}
		--i;
	}
	return i;
}