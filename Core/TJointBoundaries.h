#pragma once

#include "materials.h"

namespace TexGen
{

	using namespace std;

	class CTextile;
	class CPrismVoxelMesh;
	class COctreeVoxelMesh;

	///Class used to generate Abaqus output for T-Joint simulations
	class CLASS_DECLSPEC CTJointBoundaries
	{
	public:
		CTJointBoundaries();
		~CTJointBoundaries(void);
		void CreateTJointBoundaries(CPrismVoxelMesh* PrismMesh, ostream &Output, CTextile &Textile, bool bMatrixOnly);
		void CreateTJointBoundaries(COctreeVoxelMesh* OctMesh, ostream &Output, CTextile &Textile, bool bMatrixOnly);
	protected:
		void OutputTJointSets(ostream & Output, CTextile & Textile);
		void OutputLoadCase(ostream & Output);
	};
};	// namespace TexGen