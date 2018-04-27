

#include "PrecompiledHeaders.h"
#include "TextileLayerToLayerBifurcation.h"

using namespace TexGen;

// Build a weave unit cell of given width, height, yarn spacing and fabric thickness
// Overloaded constructors
CTextileLayerToLayerBifurcation::CTextileLayerToLayerBifurcation(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing, double dXHeight, double dYHeight, int iNumBinderLayers)
	: CTextileLayerToLayer(iNumXYarns, iNumYYarns, dXSpacing, dYSpacing, dXHeight, dYHeight, iNumBinderLayers), m_bWarpRepeat(true)
{
	m_iNumBinderLayers = iNumBinderLayers;
	
}

CTextileLayerToLayerBifurcation::CTextileLayerToLayerBifurcation(TiXmlElement &Element)
	: CTextileLayerToLayer(Element)
{
	Element.Attribute("BinderLayers", &m_iNumBinderLayers);
}

//Destructor
CTextileLayerToLayerBifurcation::~CTextileLayerToLayerBifurcation()
{
}

void CTextileLayerToLayerBifurcation::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile3DWeave::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("BinderLayers", m_iNumBinderLayers);
}

bool CTextileLayerToLayerBifurcation::BuildTextile() const
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
	for (i=0; i<m_iNumXYarns; ++i)
	{
		y += m_XYarnData[i].dSpacing/2.0;
		x = 0;
		Yarns.clear();
		int NumYYarns = m_iNumYYarns;
		if (!m_bWarpRepeat)
			NumYYarns--;
		for (j=0; j<=NumYYarns; ++j)
		{
			const vector<PATTERN3D> &Cell = GetCell(j%m_iNumYYarns, i);
			int NextCellIndex;
			NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex%m_iNumXYarns );
			if (j==0)
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN3D_XYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_XYarns[i] = Yarns;
			iYarn = 0;

			x += m_YYarnData[j%m_iNumYYarns].dSpacing/2.0;
			
			z = 0.0;
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_XYARN)
				{
					double dHalfHeight = m_XYarnData[i].dHeight / 2.0;
					if ( IsBinderYarn(i) )
					{
						if ( k == 0 )
						{
							z -= dHalfHeight + m_dGapSize;
							if ( (z - dHalfHeight) < m_dMinZ )
								m_dMinZ = z - dHalfHeight;
						}
						else
						{
							if ( NextCell[k] == PATTERN3D_XYARN )
							{
								dHalfHeight = m_XYarnData[NextCellIndex].dHeight / 2.0;
							}
							else if ( NextCell[k] == PATTERN3D_YYARN )
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
					if ( z > m_dMaxZ )
						m_dMaxZ = z;
				}
				else if ( Cell[k] == PATTERN3D_YYARN )
				{
					z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
				}
				else if ( k > 0 )// PATTERN3D_NOYARN and not on bottom binder layer
				{
					if ( NextCell[k] == PATTERN3D_XYARN )
					{
						z += m_XYarnData[NextCellIndex].dHeight + m_dGapSize;
					}
					else if ( NextCell[k] == PATTERN3D_YYARN )
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
			if (j<m_iNumYYarns)
				x += m_YYarnData[j].dSpacing/2.0;
		}
		y += m_XYarnData[i].dSpacing/2.0;
	}

	// Add y yarns (yarns parallel to the y axis)
	x = 0;
	for (j=0; j<m_iNumYYarns; ++j)
	{
		y = 0;
		Yarns.clear();
		x += m_YYarnData[j].dSpacing/2.0;
		for (i=0; i<=m_iNumXYarns; ++i)
		{
			const vector<PATTERN3D> &Cell = GetCell(j, i%m_iNumXYarns);
			
			int NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex%m_iNumXYarns);
			if (i==0)
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN3D_YYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_YYarns[j] = Yarns;
			iYarn = 0;
			y += m_XYarnData[i%m_iNumXYarns].dSpacing/2.0;
			z = 0.0;
			
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_YYARN)
				{
					double dHalfHeight = m_YYarnData[j].dHeight / 2.0;
					z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(0, 1, 0)));
					++iYarn;
					z += dHalfHeight + m_dGapSize;
				}
				else if ( Cell[k] == PATTERN3D_XYARN && k > 0 ) // Don't adjust z if it's the bottom binder yarn
				{
					if ( IsBinderYarn(i%m_iNumXYarns) )
					{
						if ( NextCell[k] == PATTERN3D_XYARN )
						{
							z += m_XYarnData[NextCellIndex%m_iNumXYarns].dHeight + m_dGapSize;
						}
						else if ( NextCell[k] == PATTERN3D_YYARN )
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
				else if ( k > 0 ) // PATTERN3D_NOYARN and not on bottom binder layer
				{
					if ( NextCell[k] == PATTERN3D_XYARN )
					{
						z += m_XYarnData[NextCellIndex%m_iNumXYarns].dHeight + m_dGapSize;
					}
					else if ( NextCell[k] == PATTERN3D_YYARN )
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
			if (i<m_iNumXYarns)
				y += m_XYarnData[i].dSpacing/2.0;
		}
		x += m_YYarnData[j].dSpacing/2.0;
	}


	// Assign sections to the yarns
	vector<int>::iterator itpYarn;
	double dWidth, dHeight;
	for (i=0; i<m_iNumXYarns; ++i)
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
	for (i=0; i<m_iNumYYarns; ++i)
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

	ShapeBinderYarns();

	// Add repeats and set interpolation
	dWidth = GetWidth();
	dHeight = GetHeight();
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->AssignInterpolation(CInterpolationBezier());
		itYarn->SetResolution(m_iResolution);
		if (m_bWarpRepeat)
			itYarn->AddRepeat(XYZ(dWidth, 0, 0));
		if (m_bWeftRepeat)
			itYarn->AddRepeat(XYZ(0, dHeight, 0));
	}

	return true;
}