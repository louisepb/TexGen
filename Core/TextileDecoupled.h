
#pragma once
#include "TextileLayerToLayer.h"
#include "Textile3DWeave.h"
#include "SectionPowerEllipse.h"

namespace TexGen
{
	using namespace std;


	/// Represents a 3D layer to layer woven textile
	class CLASS_DECLSPEC CTextileDecoupled : public CTextileLayerToLayer
	{
	public:

		/// Build a weave unit cell of given width, height, yarn spacing and fabric thickness
		/**
		\param iNumXYarns Total number of warp and binder yarns parallel to x-axis
		\param iNumYYarns Total number of weft yarns parallel to y-axis
		\param dXSpacing Spacing of x yarns, centred on yarn
		\param dYSpacing Spacing of y yarns, centred on yarn
		\param dXHeight Warp yarn height
		\param dYHeight Weft yarn height
		\param iNumBinderLayers  The number of binder layers alternating with wefts in the stack
		*/
		CTextileDecoupled(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing,
			double dXHeight, double dYHeight, int iNumBinderLayers, bool bShapeBinders = true);
		CTextileDecoupled(TiXmlElement &Element);
		virtual ~CTextileDecoupled(void);

		virtual CTextile* Copy() const { return new CTextileDecoupled(*this); }
		virtual string GetType() const { return "CTextileDecoupled"; }
		//virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Sets the vertical positions of the binder yarns.  
		/**\param zOffset The number of binder positions offset from the top of the textile
		*/
		void SetBinderPosition(int x, int y, vector<int> zOffsets);
		void ShapeBinderYarns() const;

		int FindBinderHeight(const vector<PATTERN3D>& Cell, int Height) const;

		int AddBinderNodes(int CurrentNode, int i, int j, int Height) const;

		bool BuildTextile() const;
	
	protected:

		vector<int> m_BinderPositions;
		vector<vector<int>> m_BinderPositionsVector;
	};
};	// namespace TexGen