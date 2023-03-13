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

	BuildDomain();
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

	std::vector<nodeCoordinates*> loopNodeCoordinates = CalculateNodeCoordinatesForWidthwiseYarn();

	CreateNodesForWidthwiseYarn(loopNodeCoordinates);

	BuildYarns();
	

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

void CTextileWeftKnit::RefineTextile(bool bCorrectWidths, bool bCorrectInterference, bool bPeriodic)
{

}


std::vector<nodeCoordinates*> CTextileWeftKnit::CalculateNodeCoordinatesForWidthwiseYarn() const
{
	// Currently using model proposed by Ravandi et al. 
	// in "Numerical Simulation of the Mechanical Behavior of a Weft-Knitted Carbon Fiber Composite under Tensile Loading" (2022)
	
	std::vector<nodeCoordinates*> loopNodeCoordinates;

	// The nodeCoordinate structs are safely deleted at the end of BuildTextile() to avoid memory leak

	nodeCoordinates *Node1 = new nodeCoordinates;
	Node1->xCoord = double(0);
	Node1->yCoord = double(0);
	Node1->zCoord = double(m_dYarnThickness / double(2));
	loopNodeCoordinates.push_back(Node1);

	for (int i = 0; i < m_iCourses; i++)
	{
		nodeCoordinates *Node2 = new nodeCoordinates;
		Node2->xCoord = double(((m_dCourseWidth + (double(2) * m_dYarnThickness)) / double(4)) + (m_dCourseWidth * double(i)));
		Node2->yCoord = double((m_dLoopHeight - m_dWaleHeight) / double(2));
		Node2->zCoord = double(0);
		loopNodeCoordinates.push_back(Node2);

		nodeCoordinates *Node3 = new nodeCoordinates;
		Node3->xCoord = double((m_dCourseWidth / double(4)) + (m_dCourseWidth * double(i)));
		Node3->yCoord = double(m_dLoopHeight / double(2));
		Node3->zCoord = double(double(-1) * m_dYarnThickness / double(2));
		loopNodeCoordinates.push_back(Node3);

		nodeCoordinates *Node4 = new nodeCoordinates;
		Node4->xCoord = double(((m_dCourseWidth - (double(2) * m_dYarnThickness)) / double(4)) + (m_dCourseWidth * double(i)));
		Node4->yCoord = double((m_dLoopHeight + m_dWaleHeight) / double(2));
		Node4->zCoord = double(0);
		loopNodeCoordinates.push_back(Node4);

		nodeCoordinates *Node5 = new nodeCoordinates;
		Node5->xCoord = double((m_dCourseWidth / double(2)) + (m_dCourseWidth * double(i)));
		Node5->yCoord = double(m_dLoopHeight);
		Node5->zCoord = double(m_dYarnThickness / double(2));
		loopNodeCoordinates.push_back(Node5);

		nodeCoordinates *Node6 = new nodeCoordinates;
		Node6->xCoord = double((((double(3) * m_dCourseWidth) + (double(2) * m_dYarnThickness)) / double(4)) + (m_dCourseWidth * double(i)));
		Node6->yCoord = double((m_dLoopHeight + m_dWaleHeight) / double(2));
		Node6->zCoord = double(0);
		loopNodeCoordinates.push_back(Node6);

		nodeCoordinates *Node7 = new nodeCoordinates;
		Node7->xCoord = double(((double(3) * m_dCourseWidth) / double(4)) + (m_dCourseWidth * double(i)));
		Node7->yCoord = double(m_dLoopHeight / double(2));
		Node7->zCoord = double(double(-1) * m_dYarnThickness / double(2));
		loopNodeCoordinates.push_back(Node7);

		nodeCoordinates *Node8 = new nodeCoordinates;
		Node8->xCoord = double((((double(3) * m_dCourseWidth) - (double(2) * m_dYarnThickness)) / double(4)) + (m_dCourseWidth * double(i)));
		Node8->yCoord = double((m_dLoopHeight - m_dWaleHeight) / double(2));
		Node8->zCoord = double(0);
		loopNodeCoordinates.push_back(Node8);

		nodeCoordinates *Node9 = new nodeCoordinates;
		Node9->xCoord = m_dCourseWidth + (m_dCourseWidth * double(i));
		Node9->yCoord = double(0);
		Node9->zCoord = double(m_dYarnThickness / double(2));
		loopNodeCoordinates.push_back(Node9);
		
	}

	return loopNodeCoordinates;
}

void CTextileWeftKnit::CreateNodesForWidthwiseYarn(std::vector<nodeCoordinates*> nodeCoords) const
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


void CTextileWeftKnit::BuildYarns() const
{
	CYarn* newYarn = new CYarn();
	CSectionEllipse* section = new CSectionEllipse(m_dYarnThickness, m_dYarnThickness);
	CYarnSectionConstant* sectionConstant = new CYarnSectionConstant(*section);


	CNode* thisNode = nullptr;

	for (std::vector<CNode*>::const_iterator nodeIter = m_Nodes.begin(); nodeIter != m_Nodes.end(); nodeIter++)
	{
		thisNode = *nodeIter;
		newYarn->AddNode(*thisNode);
	}

	newYarn->AssignSection(*sectionConstant);
	newYarn->SetResolution(20);
	this->AddYarn(*newYarn);

	for (int i = 1; i < m_iWales; i++)
	{
		newYarn->Translate(XYZ(0, m_dWaleHeight, 0));
		this->AddYarn(*newYarn);
	}
}


void CTextileWeftKnit::BuildDomain()
{
	CDomainPlanes domain = CDomainPlanes(XYZ(0, -m_dYarnThickness / double(2) , -m_dYarnThickness), XYZ(m_dCourseWidth * m_iCourses, (m_dLoopHeight * double(m_iWales)) - ((m_dLoopHeight - m_dWaleHeight) * double(m_iWales - 1)) + (m_dYarnThickness / double(2)), m_dYarnThickness));
	AssignDomain(domain);
}