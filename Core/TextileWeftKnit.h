#pragma once
#include "TextileKnit.h"

namespace TexGen
{
	using namespace std;

	struct nodeCoordinates
	{
		double xCoord;
		double yCoord;
		double zCoord;
	};

	class CLASS_DECLSPEC CTextileWeftKnit : public CTextileKnit
	{
	public:

		CTextileWeftKnit(int iWales, int iCourses, double dWaleHeight, double dCourseWidth, double dLoopHeight, double dYarnThickness);
		CTextileWeftKnit(TiXmlElement &Element);
		virtual ~CTextileWeftKnit(void);

		virtual CTextile* Copy() const { return new CTextileWeftKnit(*this); }
		virtual string GetType() const { return "CTextileWeftKnit"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);
		
		virtual string GetDefaultName() const;
		virtual void RefineTextile(bool bCorrectWidths = true, bool bCorrectInterference = true, bool bPeriodic = true);

	protected:

		virtual bool BuildTextile() const;
		std::vector<nodeCoordinates*> CalculateNodeCoordinatesForSingleLoop() const;

		

		int m_iWales;
		int m_iCourses;
		double m_dWaleHeight;
		double m_dCourseWidth;
		double m_dLoopHeight;
		double m_dYarnThickness;
	};

}



