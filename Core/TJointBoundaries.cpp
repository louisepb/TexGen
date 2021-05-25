#include "PrecompiledHeaders.h"
#include "TJointBoundaries.h"
#include "TexGen.h"

using namespace TexGen;
using namespace std;

CTJointBoundaries::CTJointBoundaries()
{
}

CTJointBoundaries::~CTJointBoundaries(void)
{
}

void CTJointBoundaries::CreateTJointBoundaries(CPrismVoxelMesh* PrismMesh, ostream& Output, CTextile & Textile, bool bMatrixOnly)
{
	return;
}


void CTJointBoundaries::CreateTJointBoundaries(COctreeVoxelMesh* OctMesh, ostream& Output, CTextile& Textile, bool bMatrixOnly)
{
	return;
}


void CTJointBoundaries::OutputTJointSets(ostream& Output, CTextile& Textile)
{
	//Get Domain points
	CDomainPrism* Domain = Textile.GetDomain()->GetPrismDomain();
	vector<XY> PrismPoints = Domain->GetPoints();

	vector<double> Points;
	//Separate the XY into an X and Y (transpose onto y and z co-ods for T)
	for (auto i = begin(PrismPoints); i != PrismPoints.end(); i++)
	{
		Points.push_back(i->x);
	}
	auto Min_yPoint = *min_element(Points.begin(), Points.end());
	int  Min_yPoint_Pos = distance(Points.begin(), min_element(Points.begin(), Points.end()));

	//Nodes = Textile.Ge

		//Need to find nodes closest to this y value
	Output << "*NSet, NSet= WebNodes";
	Output << ", Unsorted" << endl;

	//WriteValues(Output, m_, 16)
}

void CTJointBoundaries::OutputLoadCase(ostream& Output)
{
	return;
}