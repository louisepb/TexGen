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
==============================================================================*/

//#pragma once
//#include "TextileKnit.h"
//#include "DomainPlanes.h"
//
//namespace TexGen
//{
//	using namespace std;
//
//	enum LoopModel
//	{
//		JI_2020
//	};
//
//	/// Class to generate a warp knit textile
//	class CLASS_DECLSPEC CTextileWarpKnit : public CTextileKnit
//	{
//	public:
//
//		/// Build a warp knit unit cell 
//		CTextileWarpKnit(int iWales, int iCourses, double dWaleHeight, double dLoopHeight, double dCourseWidth, double dYarnThickness);
//		CTextileWarpKnit(TiXmlElement &Element);
//		virtual ~CTextileWarpKnit(void);
//
//		virtual CTextile* Copy() const { return new CTextileWarpKnit(*this); }
//		virtual string GetType() const { return "CTextileWarpKnit"; }
//		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);
//
//		virtual string GetDefaultName() const;
//		/// Refine for warp knit is not implemented yet
//		virtual void RefineTextile(bool bCorrectWidths = true, bool bCorrectInterference = true, bool bPeriodic = true);
//
//		void AssignDefaultDomain();
//		virtual CDomainPlanes GetDefaultDomain();
//		void SetLoopModel(LoopModel iLoopModel);
//
//	protected:
//
//		/// Build the textile
//		/** Generate the yarns to create the warp knitted textile.
//		**/
//		virtual bool BuildTextile() const;
//		/// Builds the warp knit textile using the Ravandi model for calculating loop geometry
//		virtual void BuildTextileUsingRavandiLoopModel() const;
//		virtual void AddOneLoopToYarn() const;
//		virtual void AddRepeats() const;
//
//
//		double m_dWaleHeight;
//		double m_dCourseWidth;
//		double m_dLoopHeight;
//		LoopModel m_iLoopModel;
//
//	};
//
//}




