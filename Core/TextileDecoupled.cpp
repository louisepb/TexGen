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
	

	for (itzOffsets = zOffsets.begin(); itzOffsets != zOffsets.end(); itzOffsets++)
	{
		BinderPos = (size - 1) - *itzOffsets * 2;
		m_BinderPositions.push_back(BinderPos);
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
		if (count(m_BinderPositions.begin(), m_BinderPositions.end(), j ))
		{
			Cell[j] = PATTERN3D_XYARN;
		}
		else
		{
			Cell[j] = PATTERN3D_NOYARN;
		}
	}

	m_BinderPositionsVector.push_back(m_BinderPositions);
}

void CTextileDecoupled::ShapeBinderYarns() const
{
	vector<vector<int>>::iterator itVector;
	

	for (int j = 0; j < m_iNumXYarns; ++j)
	{
		if (IsBinderYarn(j))
		{
			for (auto & itHeight : m_BinderPositions ) //need a different way of finding binder heights for this type of textile here or in FindBinderHeight()
			{
				int CurrentNode = 0;
				for (int i = 0; i < m_iNumYYarns; ++i)
				{
					CurrentNode = AddBinderNodes(CurrentNode, i, j, itHeight);
					CurrentNode++;
				}
				CheckUpVectors(j);
			}
			++itVector;
		}
	}
}


int CTextileDecoupled::FindBinderHeight(const vector<PATTERN3D>& Cell, int Height) const
{
	int i = Cell.size() - 1;
	/*while (i > 0)
	{
		//finds top binder
		if (Cell[i] == PATTERN3D_XYARN)
			return i - Height * 2; //George - This correctly assumes the binders are next to each other for L2L weaves
		--i;
	}
	return i;
	*/

	return i - Height * 2;
}

int CTextileDecoupled::AddBinderNodes(int CurrentNode, int i, int j, int Height) const
{
	const vector<PATTERN3D> &Cell = GetCell(i, j);

	int NextCellIndex = (i + 1) % m_iNumYYarns;
	int PrevCellIndex = (i + m_iNumYYarns - 1) % m_iNumYYarns;
	const vector<PATTERN3D> &NextCell = GetCell((i + 1) % m_iNumYYarns, j);
	const vector<PATTERN3D> &PrevCell = GetCell((i + m_iNumYYarns - 1) % m_iNumYYarns, j);

	int iIndex = FindBinderHeight(Cell, Height);
	int iPrevIndex = FindBinderHeight(PrevCell, Height);
	int iNextIndex = FindBinderHeight(NextCell, Height);
	int iMaxIndex = Cell.size() - 1;

	XY SectionPoint;
	XYZ WeftBelowNode, WeftAboveNode;
	int iStartNode = CurrentNode;

	if (iIndex < 0)
		return CurrentNode;

	double dBinderOffset = m_XYarnData[j].dHeight / 2.0;

	int BinderYarnIndex = GetYarnIndex(i, j, iIndex);
	if (BinderYarnIndex == -1)
		return CurrentNode;

	int WeftAboveIndex, WeftBelowIndex;

	// Get indices of weft yarns above and below binder
	if (iIndex < (int)Cell.size() - 1)
	{
		WeftAboveIndex = GetYarnIndex(i, j, iIndex + 1);
		WeftAboveNode = m_Yarns[WeftAboveIndex].GetNode(j)->GetPosition();
	}
	else
		WeftAboveIndex = -1;

	if (iIndex > 0)
	{
		WeftBelowIndex = GetYarnIndex(i, j, iIndex - 1);
		WeftBelowNode = m_Yarns[WeftBelowIndex].GetNode(j)->GetPosition();
	}
	else
		WeftBelowIndex = -1;

	XYZ BinderNode = m_Yarns[BinderYarnIndex].GetNode(CurrentNode)->GetPosition();

	if (iIndex > 0 && iPrevIndex < iIndex && PrevCell[iPrevIndex] == PATTERN3D_XYARN
		&& GetYarnIndex(PrevCellIndex, j, iPrevIndex) == BinderYarnIndex)
	{
		// Get cross section of weft yarn below
		CSectionPowerEllipse* YarnSection = GetWeftCrossSection(WeftBelowIndex);
		if (YarnSection == NULL)
			return CurrentNode;

		// Insert points around Top left quadrant of weft yarn
		XYZ NewNode = WeftBelowNode;


		int PrevWeftAboveIndex = GetYarnIndex(PrevCellIndex, j, iPrevIndex + 1);
		XYZ PrevWeftAboveNode = m_Yarns[PrevWeftAboveIndex].GetNode(j)->GetPosition();
		CSectionPowerEllipse* PrevYarnSection = GetWeftCrossSection(PrevWeftAboveIndex);
		XY PrevSectionPoint = PrevYarnSection->GetPoint(0.0);

		// Check whether need to add node at the end of the section.
		// Compare the angles between the two end nodes and the nodes inserted at 0.45 and 0.95 of the current weft position and the previous one
		if (i == 0)
			PrevWeftAboveNode.x -= GetWidth();  // Need position of the yarn before the current node
		XYZ PrevEndPoint = PrevWeftAboveNode;
		PrevEndPoint.x = PrevWeftAboveNode.x + PrevSectionPoint.x + dBinderOffset + m_dGapSize;
		XYZ PrevPoint = PrevWeftAboveNode;
		PrevSectionPoint = PrevYarnSection->GetPoint(0.95);
		PrevPoint.x = PrevWeftAboveNode.x + PrevSectionPoint.x;
		PrevPoint.z = PrevWeftAboveNode.z + PrevSectionPoint.y - (dBinderOffset + m_dGapSize);

		SectionPoint = YarnSection->GetPoint(0.5);
		NewNode.x = NewNode.x + SectionPoint.x - dBinderOffset - m_dGapSize;
		NewNode.z = NewNode.z + SectionPoint.y;
		SectionPoint = YarnSection->GetPoint(0.45);
		XYZ NewNodePoint = WeftBelowNode;
		NewNodePoint.x = NewNodePoint.x + SectionPoint.x;
		NewNodePoint.z = NewNodePoint.z + SectionPoint.y + dBinderOffset + m_dGapSize;

		double EndBinderAngle = atan2(NewNode.z - PrevEndPoint.z, NewNode.x - PrevEndPoint.x);
		double dBinderAngle = atan2(NewNodePoint.z - PrevPoint.z, NewNodePoint.x - PrevPoint.x);

		if (EndBinderAngle > dBinderAngle)
		{
			m_Yarns[BinderYarnIndex].InsertNode(NewNode, CurrentNode);  // Add node at position 0.5
			CurrentNode++;
		}

		InsertBinderNode(YarnSection, 0.45, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize);
		InsertBinderNode(YarnSection, 0.4, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize);
		if (!(iIndex < iMaxIndex && iNextIndex > iIndex && NextCell[iNextIndex] == PATTERN3D_XYARN
			&& GetYarnIndex(NextCellIndex, j, iNextIndex) == BinderYarnIndex))
		{
			InsertBinderNode(YarnSection, 0.35, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize);
			InsertBinderNode(YarnSection, 0.3, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize);
		}

		if (iNextIndex <= iIndex && NextCell[iNextIndex] == PATTERN3D_XYARN
			&& GetYarnIndex(NextCellIndex, j, iNextIndex) == BinderYarnIndex)
		{
			InsertBinderNode(YarnSection, 0.25, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize, false);
		}
		delete YarnSection;
	}
	else if (iIndex < iMaxIndex && iPrevIndex > iIndex && PrevCell[iPrevIndex] == PATTERN3D_XYARN
		&& GetYarnIndex(PrevCellIndex, j, iPrevIndex) == BinderYarnIndex)
	{
		// Get cross section of weft yarn below
		CSectionPowerEllipse* YarnSection = GetWeftCrossSection(WeftAboveIndex);
		if (YarnSection == NULL)
			return CurrentNode;
		// Insert points around lower left quadrant of weft yarn
		XYZ NewNode = WeftAboveNode;

		int PrevWeftBelowIndex = GetYarnIndex(PrevCellIndex, j, iPrevIndex - 1);
		XYZ PrevWeftBelowNode = m_Yarns[PrevWeftBelowIndex].GetNode(j)->GetPosition();
		CSectionPowerEllipse* PrevYarnSection = GetWeftCrossSection(PrevWeftBelowIndex);
		XY PrevSectionPoint = PrevYarnSection->GetPoint(0.0);

		// Check whether need to add node at the end of the section.
		// Compare the angles between the two end nodes and the nodes inserted at 0.45 and 0.95 of the current weft position and the previous one
		if (i == 0)
			PrevWeftBelowNode.x -= GetWidth();  // Need position of the yarn before the current node
		XYZ PrevEndPoint = PrevWeftBelowNode;
		PrevEndPoint.x = PrevWeftBelowNode.x + PrevSectionPoint.x + dBinderOffset + m_dGapSize;
		XYZ PrevPoint = PrevWeftBelowNode;
		PrevSectionPoint = PrevYarnSection->GetPoint(0.05);
		PrevPoint.x = PrevWeftBelowNode.x + PrevSectionPoint.x;
		PrevPoint.z = PrevWeftBelowNode.z + PrevSectionPoint.y + dBinderOffset + m_dGapSize;

		SectionPoint = YarnSection->GetPoint(0.5);
		NewNode.x = NewNode.x + SectionPoint.x - dBinderOffset - m_dGapSize;
		NewNode.z = NewNode.z + SectionPoint.y;
		SectionPoint = YarnSection->GetPoint(0.55);
		XYZ NewNodePoint = WeftAboveNode;
		NewNodePoint.x = NewNodePoint.x + SectionPoint.x;
		NewNodePoint.z = NewNodePoint.z + SectionPoint.y - (dBinderOffset + m_dGapSize);

		double EndBinderAngle = atan2(NewNode.z - PrevEndPoint.z, NewNode.x - PrevEndPoint.x);
		double dBinderAngle = atan2(NewNodePoint.z - PrevPoint.z, NewNodePoint.x - PrevPoint.x);

		if (EndBinderAngle < dBinderAngle)
		{
			m_Yarns[BinderYarnIndex].InsertNode(NewNode, CurrentNode);  // Add node at position 0.5
			CurrentNode++;
		}


		NewNode = WeftAboveNode;

		InsertBinderNode(YarnSection, 0.55, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize));
		InsertBinderNode(YarnSection, 0.6, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize));

		if (!(iIndex > 0 && iNextIndex < iIndex && NextCell[iNextIndex] == PATTERN3D_XYARN
			&& GetYarnIndex(NextCellIndex, j, iNextIndex) == BinderYarnIndex))
		{
			InsertBinderNode(YarnSection, 0.65, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize));
			InsertBinderNode(YarnSection, 0.7, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize));
		}

		if (iNextIndex >= iIndex && NextCell[iNextIndex] == PATTERN3D_XYARN
			&& GetYarnIndex(NextCellIndex, j, iNextIndex) == BinderYarnIndex)
		{
			InsertBinderNode(YarnSection, 0.75, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize), false);
		}
		delete YarnSection;
	}



	if (iIndex > 0 && iNextIndex < iIndex && NextCell[iNextIndex] == PATTERN3D_XYARN
		&& GetYarnIndex(NextCellIndex, j, iNextIndex) == BinderYarnIndex)
	{

		// Get cross section of weft yarn below
		CSectionPowerEllipse* YarnSection = GetWeftCrossSection(WeftBelowIndex);
		if (YarnSection == NULL)
			return CurrentNode;

		if (iPrevIndex <= iIndex && PrevCell[iPrevIndex] == PATTERN3D_XYARN
			&& GetYarnIndex(PrevCellIndex, j, iPrevIndex) == BinderYarnIndex)
		{
			InsertBinderNode(YarnSection, 0.25, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize, false);
		}
		// Insert points around upper right quadrant of weft

		XYZ NewNode = WeftBelowNode;
		CurrentNode++; // Need to insert after node (ie before next node)
		if (!(iIndex < iMaxIndex && iPrevIndex > iIndex && PrevCell[iPrevIndex] == PATTERN3D_XYARN
			&& GetYarnIndex(PrevCellIndex, j, iPrevIndex) == BinderYarnIndex))
		{
			InsertBinderNode(YarnSection, 0.2, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize);
			InsertBinderNode(YarnSection, 0.15, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize);
		}
		InsertBinderNode(YarnSection, 0.1, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize);
		InsertBinderNode(YarnSection, 0.05, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize);
		CurrentNode--;

		int NextWeftAboveIndex = GetYarnIndex(NextCellIndex, j, iNextIndex + 1);
		XYZ NextWeftAboveNode = m_Yarns[NextWeftAboveIndex].GetNode(j)->GetPosition();
		CSectionPowerEllipse* NextYarnSection = GetWeftCrossSection(NextWeftAboveIndex);
		XY NextSectionPoint = NextYarnSection->GetPoint(0.5);

		// Check whether need to add node at the end of the section.
		// Compare the angles between the two end nodes and the nodes inserted at 0.45 and 0.95 of the current weft position and the previous one
		if (i == m_iNumYYarns - 1)
			NextWeftAboveNode.x += GetWidth();  // Need position of the yarn before the current node
		XYZ NextEndPoint = NextWeftAboveNode;
		NextEndPoint.x = NextWeftAboveNode.x + NextSectionPoint.x - (dBinderOffset + m_dGapSize);
		XYZ NextPoint = NextWeftAboveNode;
		NextSectionPoint = NextYarnSection->GetPoint(0.55);
		NextPoint.x = NextWeftAboveNode.x + NextSectionPoint.x;
		NextPoint.z = NextWeftAboveNode.z + NextSectionPoint.y - (dBinderOffset + m_dGapSize);

		SectionPoint = YarnSection->GetPoint(0.0);
		NewNode.x = NewNode.x + SectionPoint.x + dBinderOffset + m_dGapSize;
		NewNode.z = NewNode.z + SectionPoint.y;
		SectionPoint = YarnSection->GetPoint(0.05);
		XYZ NewNodePoint = WeftBelowNode;
		NewNodePoint.x = NewNodePoint.x + SectionPoint.x;
		NewNodePoint.z = NewNodePoint.z + SectionPoint.y + dBinderOffset + m_dGapSize;

		double EndBinderAngle = atan2(NewNode.z - NextEndPoint.z, NewNode.x - NextEndPoint.x);
		double dBinderAngle = atan2(NewNodePoint.z - NextPoint.z, NewNodePoint.x - NextPoint.x);

		if (EndBinderAngle < dBinderAngle)
		{
			m_Yarns[BinderYarnIndex].InsertNode(NewNode, CurrentNode + 1);  // Add node at position 0.5
			CurrentNode++;
		}

		delete YarnSection;
	}
	else if (iIndex < iMaxIndex && iNextIndex > iIndex && NextCell[iNextIndex] == PATTERN3D_XYARN
		&& GetYarnIndex(NextCellIndex, j, iNextIndex) == BinderYarnIndex)
	{
		// Get cross section of weft yarn below
		CSectionPowerEllipse* YarnSection = GetWeftCrossSection(WeftAboveIndex);
		if (YarnSection == NULL)
			return CurrentNode;

		if (iPrevIndex >= iIndex && PrevCell[iPrevIndex] == PATTERN3D_XYARN
			&& GetYarnIndex(PrevCellIndex, j, iPrevIndex) == BinderYarnIndex)
		{
			InsertBinderNode(YarnSection, 0.75, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize), false);
		}
		// Insert points around lower right quadrant of weft yarn
		{
			XYZ NewNode = WeftAboveNode;
			CurrentNode++;  // Need to insert after node (ie before next node)
			if (!(iIndex > 0 && iPrevIndex < iIndex && PrevCell[iPrevIndex] == PATTERN3D_XYARN
				&& GetYarnIndex(PrevCellIndex, j, iPrevIndex) == BinderYarnIndex))
			{
				InsertBinderNode(YarnSection, 0.8, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize));
				InsertBinderNode(YarnSection, 0.85, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize));
			}
			InsertBinderNode(YarnSection, 0.9, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize));
			InsertBinderNode(YarnSection, 0.95, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize));
			CurrentNode--;

			int NextWeftBelowIndex = GetYarnIndex(NextCellIndex, j, iNextIndex - 1);
			XYZ NextWeftBelowNode = m_Yarns[NextWeftBelowIndex].GetNode(j)->GetPosition();
			CSectionPowerEllipse* NextYarnSection = GetWeftCrossSection(NextWeftBelowIndex);
			XY NextSectionPoint = NextYarnSection->GetPoint(0.5);

			// Check whether need to add node at the end of the section.
			// Compare the angles between the two end nodes and the nodes inserted at 0.45 and 0.95 of the current weft position and the previous one
			if (i == m_iNumYYarns - 1)
				NextWeftBelowNode.x += GetWidth();  // Need position of the yarn before the current node
			XYZ NextEndPoint = NextWeftBelowNode;
			NextEndPoint.x = NextWeftBelowNode.x + NextSectionPoint.x - (dBinderOffset + m_dGapSize);
			XYZ NextPoint = NextWeftBelowNode;
			NextSectionPoint = NextYarnSection->GetPoint(0.45);
			NextPoint.x = NextWeftBelowNode.x + NextSectionPoint.x;
			NextPoint.z = NextWeftBelowNode.z + NextSectionPoint.y + dBinderOffset + m_dGapSize;

			SectionPoint = YarnSection->GetPoint(0.0);
			NewNode.x = NewNode.x + SectionPoint.x + dBinderOffset + m_dGapSize;
			NewNode.z = NewNode.z + SectionPoint.y;
			SectionPoint = YarnSection->GetPoint(0.95);
			XYZ NewNodePoint = WeftAboveNode;
			NewNodePoint.x = NewNodePoint.x + SectionPoint.x;
			NewNodePoint.z = NewNodePoint.z + SectionPoint.y - (dBinderOffset + m_dGapSize);

			double EndBinderAngle = atan2(NextEndPoint.z - NewNode.z, NextEndPoint.x - NewNode.x);
			double dBinderAngle = atan2(NextPoint.z - NewNodePoint.z, NextPoint.x - NewNodePoint.x);

			if (EndBinderAngle > dBinderAngle)
			{
				m_Yarns[BinderYarnIndex].InsertNode(NewNode, CurrentNode + 1);  // Add node at position 0.5
				CurrentNode++;
			}
		}

		delete YarnSection;
	}

	if (iStartNode == 0)
	{
		XYZ NewNode = m_Yarns[BinderYarnIndex].GetNode(0)->GetPosition();
		ReplaceLastNode(BinderYarnIndex, NewNode, BinderNode);
	}

	return CurrentNode;
}

bool CTextileDecoupled::BuildTextile() const
{
	m_Yarns.clear();
	m_YYarns.clear();
	m_XYarns.clear();

	m_YYarns.resize(m_iNumYYarns);
	m_XYarns.resize(m_iNumXYarns);

	m_dMinZ = 0.0;
	m_dMaxZ = 0.0;

	if (!Valid() || !CheckCells())
	{
		TGERROR("Cannot build textile - incorrect yarn setup");
		return false;
	}

	TGLOGINDENT("Building textile weave \"" << GetName() << "\"");

	vector<int> Yarns;

	double x, y, z;

	// Add x yarns (yarns parallel to the x axis)
	int i, j, k, iYarn;
	y = 0;
	for (i = 0; i < m_iNumXYarns; ++i)
	{
		y += m_XYarnData[i].dSpacing / 2.0;
		x = 0;
		Yarns.clear();
		for (j = 0; j <= m_iNumYYarns; ++j)
		{
			const vector<PATTERN3D> &Cell = GetCell(j%m_iNumYYarns, i);
			int NextCellIndex;
			NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex%m_iNumXYarns);
			if (j == 0)
			{
				for (k = 0; k < (int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN3D_XYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_XYarns[i] = Yarns;
			iYarn = 0;

			x += m_YYarnData[j%m_iNumYYarns].dSpacing / 2.0;

			z = 0.0;
			for (k = 0; k < (int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_XYARN)
				{
					double dHalfHeight = m_XYarnData[i].dHeight / 2.0;
					if (IsBinderYarn(i))
					{
						if (k == 0)
						{
							z -= dHalfHeight + m_dGapSize;
							if ((z - dHalfHeight) < m_dMinZ)
								m_dMinZ = z - dHalfHeight;
						}
						else
						{
							if (NextCell[k] == PATTERN3D_XYARN)
							{
								dHalfHeight = m_XYarnData[NextCellIndex].dHeight / 2.0;
							}
							else if (NextCell[k] == PATTERN3D_YYARN)
							{
								dHalfHeight = m_YYarnData[j%m_iNumYYarns].dHeight / 2.0;
							}
							else // PATTERN3D_NOYARN
							{
								// Does this ever happen?
							}
							z += dHalfHeight;
						}
					}
					else
					{
						z += dHalfHeight;
					}
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(1, 0, 0)));
					++iYarn;
					z += dHalfHeight + m_dGapSize;
					if (z > m_dMaxZ)
						m_dMaxZ = z;
				}
				else if (Cell[k] == PATTERN3D_YYARN)
				{
					z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
				}
				else if (k > 0)// PATTERN3D_NOYARN and not on bottom binder layer
				{
					if (NextCell[k] == PATTERN3D_XYARN)
					{
						z += m_XYarnData[NextCellIndex].dHeight + m_dGapSize;
					}
					else if (NextCell[k] == PATTERN3D_YYARN)
					{
						z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
					}
					else // PATTERN3D_NOYARN
					{
						// Will get here if all x yarns are binder yarns so just use binder yarn height to give spacing
						z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
					}
				}
			}
			if (j < m_iNumYYarns)
				x += m_YYarnData[j].dSpacing / 2.0;
		}
		y += m_XYarnData[i].dSpacing / 2.0;
	}

	// Add y yarns (yarns parallel to the y axis)
	x = 0;
	for (j = 0; j < m_iNumYYarns; ++j)
	{
		y = 0;
		Yarns.clear();
		x += m_YYarnData[j].dSpacing / 2.0;
		for (i = 0; i <= m_iNumXYarns; ++i)
		{
			const vector<PATTERN3D> &Cell = GetCell(j, i%m_iNumXYarns);

			int NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex%m_iNumXYarns);
			if (i == 0)
			{
				for (k = 0; k < (int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN3D_YYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_YYarns[j] = Yarns;
			iYarn = 0;
			y += m_XYarnData[i%m_iNumXYarns].dSpacing / 2.0;
			z = 0.0;

			for (k = 0; k < (int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_YYARN)
				{
					double dHalfHeight = m_YYarnData[j].dHeight / 2.0;
					z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(0, 1, 0)));
					++iYarn;
					z += dHalfHeight + m_dGapSize;
				}
				else if (Cell[k] == PATTERN3D_XYARN && k > 0) // Don't adjust z if it's the bottom binder yarn
				{
					if (IsBinderYarn(i%m_iNumXYarns))
					{
						if (NextCell[k] == PATTERN3D_XYARN)
						{
							z += m_XYarnData[NextCellIndex%m_iNumXYarns].dHeight + m_dGapSize;
						}
						else if (NextCell[k] == PATTERN3D_YYARN)
						{
							z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
						}
						else // PATTERN3D_NOYARN
						{
							// Does this ever happen?
						}
					}
					else
					{
						z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
					}
				}
				else if (k > 0) // PATTERN3D_NOYARN and not on bottom binder layer
				{
					if (NextCell[k] == PATTERN3D_XYARN)
					{
						z += m_XYarnData[NextCellIndex%m_iNumXYarns].dHeight + m_dGapSize;
					}
					else if (NextCell[k] == PATTERN3D_YYARN)
					{
						z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
					}
					else // PATTERN3D_NOYARN
					{
						// Will get here if all x yarns are binder yarns so just use binder yarn height to give spacing
						z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
					}
				}
			}
			if (i < m_iNumXYarns)
				y += m_XYarnData[i].dSpacing / 2.0;
		}
		x += m_YYarnData[j].dSpacing / 2.0;
	}


	// Assign sections to the yarns
	vector<int>::iterator itpYarn;
	double dWidth, dHeight;
	for (i = 0; i < m_iNumXYarns; ++i)
	{
		dWidth = m_XYarnData[i].dWidth;
		dHeight = m_XYarnData[i].dHeight;
		CSectionPowerEllipse Section(dWidth, dHeight, IsBinderYarn(i) ? m_dBinderPower : m_dWarpPower);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_XYarns[i].begin(); itpYarn != m_XYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
		}
	}
	for (i = 0; i < m_iNumYYarns; ++i)
	{
		dWidth = m_YYarnData[i].dWidth;
		dHeight = m_YYarnData[i].dHeight;
		CSectionPowerEllipse Section(dWidth, dHeight, m_dWeftPower);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_YYarns[i].begin(); itpYarn != m_YYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
		}
	}

	if (m_bShapeBinders)
		ShapeBinderYarns();

	// Add repeats and set interpolation
	dWidth = GetWidth();
	dHeight = GetHeight();
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->AssignInterpolation(CInterpolationBezier());
		itYarn->SetResolution(m_iResolution);
		itYarn->AddRepeat(XYZ(dWidth, 0, 0));
		itYarn->AddRepeat(XYZ(0, dHeight, 0));
	}

	return true;
}