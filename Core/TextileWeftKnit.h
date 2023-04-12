#pragma once
#include "TextileKnit.h"
#include "DomainPlanes.h"

namespace TexGen
{
	using namespace std;

	/*struct nodeCoordinates
	{
		double xCoord;
		double yCoord;
		double zCoord;
	};*/

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

		void AssignDefaultDomain();
		virtual CDomainPlanes GetDefaultDomain();

	protected:

		virtual bool BuildTextile() const;
		virtual void AddOneLoopToYarn() const;
		virtual void AddRepeats() const;
		

		int m_iWales;
		int m_iCourses;
		double m_dWaleHeight;
		double m_dCourseWidth;
		double m_dLoopHeight;
		double m_dYarnThickness;
		int m_iNumSlaveNodes;
		int m_iNumSectionPoints;
		
	};

}



