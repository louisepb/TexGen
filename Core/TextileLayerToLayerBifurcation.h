#pragma once
#include "TextileLayerToLayer.h"
#include "SectionPowerEllipse.h"

namespace TexGen
{ 
	using namespace std;

	
	/// Represents a 3D layer to layer woven textile
	class CLASS_DECLSPEC CTextileLayerToLayerBifurcation : public CTextileLayerToLayer
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
		CTextileLayerToLayerBifurcation(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing,
							double dXHeight, double dYHeight, int iNumBinderLayers);
		CTextileLayerToLayerBifurcation(TiXmlElement &Element);
		virtual ~CTextileLayerToLayerBifurcation(void);

		virtual CTextile* Copy() const { return new CTextileLayerToLayerBifurcation(*this); }
		virtual string GetType() const { return "CTextileLayerToLayerBifurcation"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Set the warp repeats
		void SetWarpRepeat(bool WarpRepeat) { m_bWarpRepeat = WarpRepeat; }
		/// Build the textile without repeats in warp direction to allow bifurcation
		virtual bool BuildTextile() const;


	protected:
		/// accessor methods
		bool m_bWarpRepeat;
	};
};	// namespace TexGen