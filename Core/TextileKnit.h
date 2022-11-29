#pragma once
#include "Textile.h"

namespace TexGen
{
	using namespace std;

	class CLASS_DECLSPEC CTextileKnit : public CTextile
	{
	public:

		CTextileKnit();
		CTextileKnit(TiXmlElement &Element);
		virtual ~CTextileKnit(void);

		virtual CTextile* Copy() const { return new CTextileKnit(*this); }
		virtual string GetType() const { return "CTextileKnit"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

	protected:

		virtual bool BuildTextile() const;


	};

}

