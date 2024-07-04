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



