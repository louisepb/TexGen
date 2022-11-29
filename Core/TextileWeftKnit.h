#pragma once
#include "TextileKnit.h"

namespace TexGen
{
	using namespace std;

	class CLASS_DECLSPEC CTextileWeftKnit : public CTextileKnit
	{
	public:

		CTextileWeftKnit();
		CTextileWeftKnit(TiXmlElement &Element);
		virtual ~CTextileWeftKnit(void);

		virtual CTextile* Copy() const { return new CTextileWeftKnit(*this); }
		virtual string GetType() const { return "CTextileWeftKnit"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

	protected:

		virtual bool BuildTextile() const;
	};

}



