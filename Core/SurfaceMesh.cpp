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
#include "SurfaceMesh.h"

using namespace TexGen;
using namespace std;

CSurfaceMesh::CSurfaceMesh(double Seed, bool bYarnHoles, bool bBinary, bool bPeriodic ) : CMeshDomainPlane(Seed, bYarnHoles)
{
	m_bBinary = bBinary;
	m_bPeriodic = bPeriodic;
}

CSurfaceMesh::~CSurfaceMesh(void)
{
}

void CSurfaceMesh::SaveSurfaceMesh(CTextile &Textile, string OutputFilename)
{
	XYZ P;

	if (!Textile.AddSurfaceToMesh(m_Mesh, m_DomainMeshes, true))
	{
		TGERROR("Error creating surface mesh. Cannot generate tetgen mesh");
		return;
	}
	m_Mesh.ConvertQuadstoTriangles(true);

	MeshDomainPlanes(m_bPeriodic);

	SaveToSTL(OutputFilename, Textile);
}

void CSurfaceMesh::SaveToSTL(string Filename, CTextile &Textile)
{
	vector<CMesh>::iterator itTriangulatedMeshes;
	for (itTriangulatedMeshes = m_TriangulatedMeshes.begin(); itTriangulatedMeshes != m_TriangulatedMeshes.end(); ++itTriangulatedMeshes)
	{
		m_Mesh.InsertMesh(*itTriangulatedMeshes);
	}

	m_Mesh.SaveToSTL(Filename, m_bBinary);
}
