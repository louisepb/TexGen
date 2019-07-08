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
CSurfaceMeshExport::CSurfaceMeshExport( CMesh SurfaceMesh )
: m_SurfaceMesh( SurfaceMesh )
{
}

CSurfaceMeshExport::~CSurfaceMeshExport(void)
{
}

void CSurfaceMeshExport::SaveSurfaceMeshToABAQUS(string Filename, CTextile& Textile )
{
	TGLOG("Replacing spaces in filename with underscore for ABAQUS compatibility");
	Filename = ReplaceFilenameSpaces( Filename );
	GetElementInfo( Textile );
	
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
}

void CSurfaceMeshExport::GetElementInfo( CTextile& Textile )
{
	vector<XYZ> CentrePoints;
	if ( !m_SurfaceMesh.GetIndices(CMesh::QUAD).empty() )
	{
		CentrePoints = m_SurfaceMesh.GetElementCenters(CMesh::QUAD);
		Textile.GetPointInformation( CentrePoints, m_ElementInfo );
	}
}