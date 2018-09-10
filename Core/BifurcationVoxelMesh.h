
#pragma once
#include "VoxelMesh.h"
//#include <set>
#include <algorithm>

namespace TexGen
{
	using namespace std;

	class CTextile;

	/// Class used to generate voxel mesh for output to ABAQUS
	class CLASS_DECLSPEC CBifurcationVoxelMesh : public CVoxelMesh
	{
	public:
		/// 
		/**
		\param Type String giving periodic boundary condition type
		\ a, b, c, d give y dimensions of T profile
		\firstweb, secondweb give z dimensions of the web of T
		*/
		CBifurcationVoxelMesh(string Type); //, double a, double b, double c, double d, double firstweb, double secondweb);
		virtual ~CBifurcationVoxelMesh(void);
		
		void SaveVoxelMesh(CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum, bool bOutputMatrix, bool bOutputYarns, int iBoundaryConditions, int iElementType = 0);
		

		double a;
		double b;
		double c;
		double firstweb;
		double secondweb;

		
	protected:

		bool IsSubset(vector<int> A, vector<int> B);

		void SaveToAbaqus(string Filename, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, int iBoundaryConditions, int iElementType);
		/// Calculate voxel size based on number of voxels on each axis and domain size
		bool CalculateVoxelSizes(CTextile &Textile);

		/// Save To Abaqus and output nodes that fall within desired space
		//void SaveToAbaqus( string Filename, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, int iBoundaryConditions, int iElementType );
		void OutputNodes( ostream &Output, CTextile &Textile, bool bAbaqus = true);

		int OutputHexElements(ostream &Output, bool bOutputMatrix, bool bOutputYarn, bool bAbaqus = true);

		//void CBifurcationVoxelMesh::OutputOrientationsAndElementSets(string Filename);

		//void OutputOrientationsAndElementSets(string Filename, ostream &Output);
		vector<int> OutsidePoints;

		double				m_VoxSize[3];

		

	};
};	// namespace TexGen
