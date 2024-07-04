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
#include "TextileKnit.h"
#include "DomainPlanes.h"

namespace TexGen
{
	using namespace std;

	enum LoopModel
	{
		RAVANDI_2021
	};

	class CLASS_DECLSPEC CTextileWeftKnit : public CTextileKnit
	{
	public:

		CTextileWeftKnit(int iWales, int iCourses, double dWaleHeight, double dLoopHeight, double dCourseWidth, double dYarnThickness);
		CTextileWeftKnit(TiXmlElement &Element);
		virtual ~CTextileWeftKnit(void);

		virtual CTextile* Copy() const { return new CTextileWeftKnit(*this); }
		virtual string GetType() const { return "CTextileWeftKnit"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);
		
		virtual string GetDefaultName() const;
		virtual void RefineTextile(bool bCorrectWidths = true, bool bCorrectInterference = true, bool bPeriodic = true);

		void AssignDefaultDomain();
		virtual CDomainPlanes GetDefaultDomain();
		void SetLoopModel(LoopModel iLoopModel);

	protected:

		virtual bool BuildTextile() const;
		virtual void CTextileWeftKnit::BuildTextileUsingRavandiLoopModel() const;
		virtual void AddOneLoopToYarn() const;
		virtual void AddRepeats() const;
		

		double m_dWaleHeight;
		double m_dCourseWidth;
		double m_dLoopHeight;	
		LoopModel m_iLoopModel;
		
	};

}



