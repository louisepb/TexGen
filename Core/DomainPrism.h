/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2020 Louise Brown

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
#include "Domain.h"
#include "Yarn.h"
#include "Plane.h"

namespace TexGen
{
	using namespace std;

	/// Domain implementation described using extrusion of a polygon outline
	/**
	Prism is described by a yarn with two nodes (to give the orientation )
	and a cross-section defined by a polygon section created using the vector of points given as an input parameter
	*/

	class CLASS_DECLSPEC CDomainPrism : public CDomain
	{
	public:
		/// Constructor
		/**
		\param Points Vector of points defining the cross-section of the prism
		\param start Position of start node 
		\return end Position of end node
		*/
		CDomainPrism(const vector<XY> &Points, XYZ &start, XYZ &end);
		CDomainPrism(TiXmlElement &Element);
		~CDomainPrism(void);

		CDomain* Copy() const { return new CDomainPrism(*this); }

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType = OUTPUT_STANDARD) const;

		/// Get a vector of repeat limits for a given yarn. The number of limit pairs is the same
		/// as the number of infinite repeats in the yarn.
		vector<pair<int, int> > GetRepeatLimits(const CYarn &Yarn) const { return vector<pair<int, int> >(); }
		/// Get the translation vectors necessary to fully fill the domain
		vector<XYZ> GetTranslations(const CYarn &Yarn) const { return vector<XYZ>(); }

		/// Clips a mesh passed to the function to the domain by using the PointInsideYarn function (for the domain 'yarn')
		/// Creates intersection mesh of intersecting elements for further testing against each domain plane
		void ClipMeshToDomain(CMesh &Mesh, bool bFillGaps = true) const;

		/// Clip mesh elements which are known to intersect with the domain
		/// Checks each mesh element against each domain plane
		void ClipIntersectMeshToDomain(CMesh &Mesh, bool bFillGaps = true) const;

		bool ClipMeshToDomain(CMesh &Mesh, vector<CMesh> &DomainMeshes, bool bFillGaps = true) const; //{ return false; }
		bool ClipIntersectMeshToDomain(CMesh &Mesh, vector<CMesh> &DomainMeshes, bool bFillGaps) const;
		string GetType() const { return "CDomainPrism"; }

		const vector<XY> &GetPoints() const { return m_Points; }
		
		/// Move all the planes by given distance along their normal
		void Grow(double dDistance) {};
		/// Rotate the domain by given rotation quaternion
		void Rotate(WXYZ Rotation) {};
		/// Translate the domain by given vector
		void Translate(XYZ Vector) {};
		/// Deform the domain by given linear transformation
		void Deform(CLinearTransformation Transformation) {};

		/// Check if a point lies within the domain
		bool PointInDomain(const XYZ &Point) const { return false; }

		/// Generate a set of planes corresponding to the mesh elements
		void GeneratePlanes();
		void GetMeshWithPolygonEnd(CMesh &Mesh);

		void GetPolygonLimits(XYZ &StartPoint, XYZ *SizeVecs);

	protected:
		/// Get the limits for a single given repeat vector and surface mesh
		/**
		The limits are the points where the whole surface mesh lies outside the domain
		The limit is represented as the factor multiplied by the repeat vector that represents
		the translation of the surface mesh to the limiting case.
		\param RepeatVector The repeat vector to get limit for
		\param Mesh The surface mesh that needs to lie behind the plane
		\return A pair of doubles representing the upper and lower limits
		*/
		//pair<double, double> GetLimits(XYZ RepeatVector, const CMesh &Mesh) const;

		/// Determine if a mesh intersects with the domain
		/**
		This is a very crude approximation of whether a mesh intersects the domain or not.
		It is based soley on the axis aligned bounding boxes of the two regions.
		*/
		//bool MeshIntersectsDomain(const CMesh &Mesh) const;

		/// Populate m_PlaneIntersections and m_Mesh, note this only works for closed domains
		void BuildMesh();

		/// Generate a plane from a vector of co-planar points
		bool GetPlane(XYZ *points, PLANE &plane);
		/// Iterate through m_ElementPlanes to remove duplicates
		void RemoveDuplicatePlanes();
		/// Test if both normal and d of two PLANE structures are equal 
		bool PlaneEqual(PLANE Plane1, PLANE Plane2);

		/// Given a mesh and a plane, the holes found in the plane will be filled with triangles
		/// Intersection of mesh and plane returned as closed loop
		static bool FillGaps(CMesh &Mesh, const PLANE &Plane, vector<int> &Polygon, bool bMeshGaps = true);

		/// Create the domain as yarn with constant polygon section and two nodes
		mutable CYarn m_Yarn;

		/// Planes corresponding to mesh elements
		vector<PLANE> m_ElementPlanes;

		/// Prism section points
		mutable vector<XY> m_Points;
	};

};	// namespace TexGen