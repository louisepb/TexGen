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

#pragma once

#include "Mesh.h"
#include "MeshDomainPlane.h"

namespace TexGen
{
	/** Class which generates a surface mesh with triangulated domain boundaries (This assumes that
	the domain is specified to be a whole number of textile repeats).
	*/

	using namespace std;

	class CLASS_DECLSPEC CSurfaceMesh : public CMeshDomainPlane
	{
	public:
		CSurfaceMesh(double Seed, bool bYarnHoles = false, bool bBinary = false );
		virtual ~CSurfaceMesh(void);
		/// Save a textile as a surface mesh with triangulated domain faces
		/**
		\param Textile Textile to be meshed
		\param Filename for surface output files
		\param bPeriodic If set true, opposite faces of mesh will be replicated
		*/
		void SaveSurfaceMesh(CTextile &Textile, string OutputFilename, bool bPeriodic, bool bSaveYarns = true);

	protected:
		///	Mesh used to store node points and elements
		CMesh			m_Mesh;

		/// True if file to be saved as binary, false if ASCII
		bool m_bBinary;

		/// Save surface mesh data to STL file
		void SaveToSTL(string Filename, CTextile &Textile, bool bSaveYarns);
	};
};  // namespace TexGen