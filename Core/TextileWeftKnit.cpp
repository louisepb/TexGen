#include "PrecompiledHeaders.h"
#include "TextileWeftKnit.h"
#include "SectionEllipse.h"
#include "DomainPlanes.h"

using namespace TexGen;

CTextileWeftKnit::CTextileWeftKnit(int iWales, int iCourses, double dWaleHeight, double dCourseWidth, double dLoopHeight, double dYarnThickness) : CTextileKnit()
, m_iWales(iWales)
, m_iCourses(iCourses)
, m_dWaleHeight(dWaleHeight)
, m_dCourseWidth(dCourseWidth)
, m_dLoopHeight(dLoopHeight)
, m_dYarnThickness(dYarnThickness)
{
	BuildTextile();

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

	std::vector<nodeCoordinates*> loopNodeCoordinates = CalculateNodeCoordinatesForSingleLoop();

	CreateNodesForSingleLoop(loopNodeCoordinates);

	for (nodeCoordinates* nodeCoords : loopNodeCoordinates)
		delete nodeCoords;
	loopNodeCoordinates.clear();

	for (CNode* node : m_Nodes)
		delete node;
	m_Nodes.clear();

	return true;
}

string CTextileWeftKnit::GetDefaultName() const
{
	return "WeftKnit(W:" + stringify(m_iWales) + ",C:" + stringify(m_iCourses) + ")";
}

void CTextileWeftKnit::RefineTextile(bool bCorrectWidths, bool bCorrectInterference, bool bPeriodic) {}


std::vector<nodeCoordinates*> CTextileWeftKnit::CalculateNodeCoordinatesForSingleLoop() const
{
	// Currently using model proposed by Ravandi et al. 
	// in "Numerical Simulation of the Mechanical Behavior of a Weft-Knitted Carbon Fiber Composite under Tensile Loading" (2022)
	
	std::vector<nodeCoordinates*> loopNodeCoordinates;

	nodeCoordinates *Node1 = new nodeCoordinates;
	nodeCoordinates *Node2 = new nodeCoordinates;
	nodeCoordinates *Node3 = new nodeCoordinates;
	nodeCoordinates *Node4 = new nodeCoordinates;
	nodeCoordinates *Node5 = new nodeCoordinates;
	nodeCoordinates *Node6 = new nodeCoordinates;
	nodeCoordinates *Node7 = new nodeCoordinates;
	nodeCoordinates *Node8 = new nodeCoordinates;
	nodeCoordinates *Node9 = new nodeCoordinates;

	Node1->xCoord = double(0);
	Node1->yCoord = double(0);
	Node1->zCoord = double(m_dYarnThickness / double(2));

	Node2->xCoord = double((m_dCourseWidth + (double(2) * m_dYarnThickness)) / double(4));
	Node2->yCoord = double((m_dLoopHeight - m_dWaleHeight) / double(2));
	Node2->zCoord = double(0);

	Node3->xCoord = double(m_dCourseWidth / double(4));
	Node3->yCoord = double(m_dLoopHeight / double(2));
	Node3->zCoord = double(double(-1) * m_dYarnThickness / double(2));

	Node4->xCoord = double((m_dCourseWidth - (double(2) * m_dYarnThickness)) / double(4));
	Node4->yCoord = double((m_dLoopHeight + m_dWaleHeight) / double(2));
	Node4->zCoord = double(0);

	Node5->xCoord = double(m_dCourseWidth / double(2));
	Node5->yCoord = double(m_dLoopHeight);
	Node5->zCoord = double(m_dYarnThickness / double(2));

	Node6->xCoord = double(((double(3) * m_dCourseWidth) + (double(2) * m_dYarnThickness)) / double(4));
	Node6->yCoord = double((m_dLoopHeight + m_dWaleHeight) / double(2));
	Node6->zCoord = double(0);

	Node7->xCoord = double((double(3) * m_dCourseWidth) / double(4));
	Node7->yCoord = double(m_dLoopHeight / double(2));
	Node7->zCoord = double(double(-1) * m_dYarnThickness / double(2));

	Node8->xCoord = double(((double(3) * m_dCourseWidth) - (double(2) * m_dYarnThickness)) / double(4));
	Node8->yCoord = double((m_dLoopHeight - m_dWaleHeight) / double(2));
	Node8->zCoord = double(0);

	Node9->xCoord = m_dCourseWidth;
	Node9->yCoord = double(0);
	Node9->zCoord = double(m_dYarnThickness / double(2));

	loopNodeCoordinates.push_back(Node1);
	loopNodeCoordinates.push_back(Node2);
	loopNodeCoordinates.push_back(Node3);
	loopNodeCoordinates.push_back(Node4);
	loopNodeCoordinates.push_back(Node5);
	loopNodeCoordinates.push_back(Node6);
	loopNodeCoordinates.push_back(Node7);
	loopNodeCoordinates.push_back(Node8);
	loopNodeCoordinates.push_back(Node9);

	return loopNodeCoordinates;
	
}

void CTextileWeftKnit::CreateNodesForSingleLoop(std::vector<nodeCoordinates*> nodeCoords) const
{
	CNode* newNode = nullptr;
	nodeCoordinates* thisNodeCoordinate = nullptr;

	for (std::vector<nodeCoordinates*>::const_iterator nodeCoordinateIter = nodeCoords.begin() ; nodeCoordinateIter != nodeCoords.end() ; nodeCoordinateIter++)
	{
		thisNodeCoordinate = (*nodeCoordinateIter);
		newNode = new CNode(XYZ(thisNodeCoordinate->xCoord, thisNodeCoordinate->yCoord, thisNodeCoordinate->zCoord));
		m_Nodes.push_back(newNode);
	}
}