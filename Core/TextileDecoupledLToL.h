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


#pragma once
#include "TextileLayerToLayer.h"
#include "Textile3DWeave.h"
#include "SectionPowerEllipse.h"

namespace TexGen
{
	using namespace std;


	/// Represents a 3D layer to layer woven textile where there can be multiple binders in any stack
	/// but these are not constrained to follow the same path through the texile
	class CLASS_DECLSPEC CTextileDecoupledLToL : public CTextileLayerToLayer
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
		CTextileDecoupledLToL(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing,
			double dXHeight, double dYHeight, int iNumBinderLayers, bool bShapeBinders = true);
		CTextileDecoupledLToL(TiXmlElement &Element);
		virtual ~CTextileDecoupledLToL(void);

		virtual CTextile* Copy() const { return new CTextileDecoupledLToL(*this); }
		virtual string GetType() const { return "CTextileDecoupledLToL"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Sets the vertical positions of the binder yarns.  
		/**\param zOffset Vector containing the offset of the binder layers from the top of the textile
		*/
		void SetBinderPosition(int x, int y, vector<int> zOffsets);
		/// Add extra nodes to binder layers to conform to shape of adjacent weft yarns
		void ShapeBinderYarns() const;
		/// Ensure that up vectors are not coincident with tangent
		void CheckUpVectors() const;
		/// Find cell index of a given binder yarn given its height
		/**
		\param Cell Vector containing warp,weft, no yarn information for a given x,y position in the textile
		\param Height Position of binder relative to top, eg 0 is top binder yarn, 1 is next binder yarn down etc
		*/
		int FindBinderHeight(const vector<PATTERN3D>& Cell, int Height) const;

		/// Build the textile yarns from the cell data
		bool BuildTextile() const;
	
	protected:

	};
};	// namespace TexGen