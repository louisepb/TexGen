
#pragma once
#include "VoxelMesh.h"
#include "Materials.h"
#include "PeriodicBoundaries.h"
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
		CBifurcationVoxelMesh(string Type); 
		virtual ~CBifurcationVoxelMesh(void);

		void InitialiseBifurcationVoxelMesh(double a, double b, double c, double firstweb, double secondweb, double flangelength);

		void SaveVoxelMesh(CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum, bool bOutputMatrix, bool bOutputYarns, bool surfaceOutput, int iBoundaryConditions, int iElementType = 0);

		CTextileMaterials& GetMaterials() { return m_Materials; }

		//CPeriodicBoundaries& GetPeriodicBoundaries() {return m_Boundaries; }
		
	protected:

		CTextileMaterials m_Materials;

		//CPeriodicBoundaries m_Boundaries;

		double m_a;
		double m_b;
		double m_c;
		double m_firstweb;
		double m_secondweb;
		double m_flangelength;

		double geta();
		double getb();
		double getc();
		double getfirstweb();
		double getsecondweb();
		double getflangelength();
		bool IsSubset(vector<int> A, vector<int> B);

		void SaveToAbaqus(string Filename, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, bool surfaceOutput, int iBoundaryConditions, int iElementType);
		/// Calculate voxel size based on number of voxels on each axis and domain size
		bool CalculateVoxelSizes(CTextile &Textile);

		/// Save To Abaqus and output nodes that fall within desired space
		void OutputNodes( ostream &Output, CTextile &Textile, bool surfaceOutput, bool bAbaqus = true);

		int OutputHexElements(ostream &Output, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, bool bAbaqus = true);

		void CreateBifurcatedMaterials(ostream& Output, string Filename, CTextile& Textile, bool bMatrixOnly);

		

		vector<int> OutsidePoints;

		double				m_VoxSize[3];

		

	};
};	// namespace TexGen
