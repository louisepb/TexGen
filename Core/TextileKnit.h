/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2024 Louise Brown

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
#include "Textile.h"

namespace TexGen
{
	using namespace std;

	/// Represents a knitted textile class
	class CLASS_DECLSPEC CTextileKnit : public CTextile
	{
	public:

		/// Build a knit unit cell giving number of wales, courses and yarn thickness
		CTextileKnit(int iWales, int iCourses, double dYarnThickness);
		CTextileKnit(TiXmlElement &Element);
		virtual ~CTextileKnit(void);

		virtual CTextile* Copy() const { return new CTextileKnit(*this); }
		virtual string GetType() const { return "CTextileKnit"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

	protected:

		/// Build the textile
		/** Generate the yarns to create the knitted textile.
		The class is virtual so that this can be overridden for different types of knitted textile.
		**/
		virtual bool BuildTextile() const;

		/// The wales are the loops which stack up vertically
		int m_iWales;
		/// The courses are the stitches across a horizontal row
		int m_iCourses;
		double m_dYarnThickness;

	};

}

