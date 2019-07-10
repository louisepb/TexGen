/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2019 Louise Brown

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

#include "PrecompiledHeaders.h"
#include "TexGen.h"



using namespace TexGen;
CSurfaceMeshExport::CSurfaceMeshExport( bool bTrimSurface, bool bExportDomain )
: m_bTrimSurface( bTrimSurface )
, m_bExportDomain( bExportDomain )
{
}

CSurfaceMeshExport::~CSurfaceMeshExport(void)
{
}

bool CSurfaceMeshExport::SaveSurfaceMeshToABAQUS(string Filename, CTextile& Textile )
{
	TGLOG("Replacing spaces in filename with underscore for ABAQUS compatibility");
	Filename = ReplaceFilenameSpaces( Filename );

	int iNumYarns = Textile.GetNumYarns();
	if (iNumYarns == 0)
		return false;
	const CDomain* pDomain = Textile.GetDomain();
	if (!pDomain)
	{
		TGERROR("Unable to create ABAQUS input file: No domain specified");
		return false;
	}

	m_YarnMeshes.clear();
	int i;
	m_YarnMeshes.resize(iNumYarns);

	
	for (i=0; i<iNumYarns; ++i)  // Create surface mesh for each yarn 
	{
		CYarn* pYarn = Textile.GetYarn(i);
		bool bMesh = false;
		if (m_bTrimSurface)
			bMesh = pYarn->AddSurfaceToMesh( m_YarnMeshes[i], *pDomain );
		else
			bMesh = pYarn->AddSurfaceToMesh( m_YarnMeshes[i]);
					
		if ( !bMesh )
		{
			TGERROR("Unable to create ABAQUS input file: Failed to create surface mesh for yarn " << i );
			return false;
		}
	}

	GetElementInfo( Textile );

	for(i=0; i<iNumYarns; ++i)  // Add meshes into single mesh
	{
		if ( !m_YarnMeshes[i].NodesEmpty() )
		{
			m_YarnMeshes[i].RemoveUnreferencedNodes();
			m_SurfaceMesh.InsertMesh(m_YarnMeshes[i]);
		}
	}
	
	// then assemble

	BuildIndexOffsets();

	m_SurfaceMesh.SaveToABAQUS(Filename, &m_ElementInfo, false, false ); 

	ofstream Output(Filename.c_str(), ofstream::app );
	// Output material properties
	m_Materials = new CTextileMaterials;
	m_Materials->SetupMaterials( Textile );
	m_Materials->OutputMaterials( Output, Textile.GetNumYarns(), false );
	delete( m_Materials );

	/*if ( m_iBoundaryConditions != NO_BOUNDARY_CONDITIONS )
	{
		m_PeriodicBoundaries->SetDomainSize( Textile.GetDomain()->GetMesh() );
		if (SaveNodeSets() )
		{
			//ofstream Output(Filename.c_str(), ofstream::app );
			Output << "*****************" << endl;
			Output << "*** NODE SETS ***" << endl;
			Output << "*****************" << endl;
			Output << "** AllNodes - Node set containing all elements" << endl;
			Output << "*NSet, NSet=AllNodes, Generate" << endl;
			Output << "1, " << m_VolumeMesh.GetNumNodes() << ", 1" << endl;
			m_PeriodicBoundaries->CreatePeriodicBoundaries( Output, m_VolumeMesh.GetNumNodes() + 1, Textile, m_iBoundaryConditions, false );
		}
		else
			TGERROR("Unable to generate node sets");
	}*/
	return true;
}

void CSurfaceMeshExport::GetElementInfo( CTextile& Textile )
{	
	TGLOG("Getting point information");
	vector<POINT_INFO> YarnElementInfo;
	for ( int i = 0; i < Textile.GetNumYarns(); ++i )
	{
		if (!m_YarnMeshes[i].GetIndices(CMesh::QUAD).empty() )
		{
			YarnElementInfo.clear();
			Textile.GetPointInformation( m_YarnMeshes[i].GetElementCenters( (CMesh::QUAD) ), YarnElementInfo, i, 0.005, true );		
			m_ElementInfo.insert( m_ElementInfo.end(), YarnElementInfo.begin(), YarnElementInfo.end() );
		}
	}
}

void CSurfaceMeshExport::BuildIndexOffsets()
{
	// Where yarn meshes are combined to create ABAQUS file
	// need to create offsets for node and element numbering
	int i, iNumYarns = (int)m_YarnMeshes.size();
	
	int iElemIndexOffset = 0;
	m_ElementIndexOffsets.clear();
	
	for (i=0; i<iNumYarns; ++i)
	{
		m_ElementIndexOffsets[i] = iElemIndexOffset;
		iElemIndexOffset += m_YarnMeshes[i].GetNumElements(CMesh::QUAD);
	}
	
	m_NodeIndexOffsets.clear();
	int iNodeIndexOffset = 0;
	for (i=0; i<iNumYarns; ++i)
	{
		m_NodeIndexOffsets[i] = iNodeIndexOffset;
		iNodeIndexOffset += m_YarnMeshes[i].GetNumNodes();
	}
}