#include "PrecompiledHeaders.h"
#include "TextileWeftKnit.h"
#include "SectionEllipse.h"
#include "DomainPlanes.h"

using namespace TexGen;

CTextileWeftKnit::CTextileWeftKnit(int iWales, int iCourses, double dWaleWidth, double dCourseHeight, double dYarnThickness) : CTextileKnit()
{


	/*double r = 1;
	double sx = r * 2.5;
	double sy = r * 10;
	double ly = 0.75 * (sx + r);

	CYarn* Yarn = new CYarn();

	CNode* Node1 = new CNode(XYZ(0, 0, r));
	CNode* Node2 = new CNode(XYZ(sx + r, ly, 0));
	CNode* Node3 = new CNode(XYZ(sx, ly + 0.5*sy, -r));
	CNode* Node4 = new CNode(XYZ(sx - r, ly + sy, 0));
	CNode* Node5 = new CNode(XYZ(2 * sx, 2 * ly + sy, r));
	CNode* Node6 = new CNode(XYZ(3 * sx + r, ly + sy, 0));
	CNode* Node7 = new CNode(XYZ(3 * sx, ly + 0.5*sy, -r));
	CNode* Node8 = new CNode(XYZ(3 * sx - r, ly, 0));
	CNode* Node9 = new CNode(XYZ(4 * sx, 0, r));

	Yarn->AddNode(*Node1);
	Yarn->AddNode(*Node2);
	Yarn->AddNode(*Node3);
	Yarn->AddNode(*Node4);
	Yarn->AddNode(*Node5);
	Yarn->AddNode(*Node6);
	Yarn->AddNode(*Node7);
	Yarn->AddNode(*Node8);
	Yarn->AddNode(*Node9);

	CSectionEllipse* Section = new CSectionEllipse(2 * r, 2 * r);

	CYarnSectionConstant* SectionConstant = new CYarnSectionConstant(*Section);

	Yarn->AssignSection(*SectionConstant);

	Yarn->AddRepeat(XYZ(4 * sx, 0, 0));
	Yarn->AddRepeat(XYZ(0, 2 * sy, 0));

	Yarn->SetResolution(20);

	this->AddYarn(*Yarn);

	Yarn->Translate(XYZ(0, sy, 0));

	this->AddYarn(*Yarn);

	this->AssignDomain(CDomainPlanes(XYZ(0, -ly, -2 * r), XYZ(4 * (r*sx), 4 * sy - ly, 2 * r)));*/

}

CTextileWeftKnit::~CTextileWeftKnit(void)
{
}

CTextileWeftKnit::CTextileWeftKnit(TiXmlElement &Element)
: CTextileKnit(Element)
{

}

void CTextileWeftKnit::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextileKnit::PopulateTiXmlElement(Element, OutputType);
}

bool CTextileWeftKnit::BuildTextile() const
{
	m_Yarns.clear();
	

	return true;
}
