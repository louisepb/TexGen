/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

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
#include "Domain.h"
#include "TexGen.h"
#include "Yarn.h"

using namespace TexGen;
CDomain::CDomain(void)
{
}

CDomain::~CDomain(void)
{
}

CDomain::CDomain(TiXmlElement &Element)
{
	TiXmlElement* pMesh = Element.FirstChildElement("Mesh");
	if (pMesh)
	{
		m_Mesh = CMesh(*pMesh);
	}
}

void CDomain::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	Element.SetAttribute("type", GetType());
	if (OutputType == OUTPUT_FULL)
	{
		TiXmlElement Mesh("Mesh");
		m_Mesh.PopulateTiXmlElement(Mesh, OutputType);
		Element.InsertEndChild(Mesh);
	}
}

CDomainPrism* CDomain::GetPrismDomain()
{
	return dynamic_cast<CDomainPrism*>(this);
}

vector<pair<int, int> > CDomain::ConvertLimitsToInt(const vector<pair<double, double> > &RepeatLimits)
{
	vector<pair<int, int> > IntRepeatLimits;
	vector<pair<double, double> >::const_iterator itRepeatLimits;
	for (itRepeatLimits = RepeatLimits.begin(); itRepeatLimits != RepeatLimits.end(); ++itRepeatLimits)
	{
		IntRepeatLimits.push_back(pair<int, int>((int)ceil(itRepeatLimits->first), (int)floor(itRepeatLimits->second)));
	}
	return IntRepeatLimits;
}

double CDomain::GetVolume() const
{
	return m_Mesh.CalculateVolume();
}

bool CDomain::MeshIntersectsDomain(const CMesh &Mesh) const
{
	pair<XYZ, XYZ> DomainAABB = m_Mesh.GetAABB();
	pair<XYZ, XYZ> MeshAABB = Mesh.GetAABB();

	return BoundingBoxIntersect(DomainAABB.first, DomainAABB.second, MeshAABB.first, MeshAABB.second);
}

vector<XYZ> CDomain::GetTranslations(const CYarn &Yarn) const
{
	vector<XYZ> AllRepeats;
	vector<XYZ> FiniteRepeats;
	const vector<XYZ> &YarnRepeats = Yarn.GetRepeats();
	vector<pair<int, int> > RepeatLimits = GetRepeatLimits(Yarn);  // How many times to repeat from original yarn
	vector<XYZ>::const_iterator itRepeat;
	vector<pair<int, int> >::const_iterator itLimits;
	AllRepeats.push_back(XYZ());
	for (itRepeat = YarnRepeats.begin(), itLimits = RepeatLimits.begin(); itRepeat != YarnRepeats.end() && itLimits != RepeatLimits.end(); ++itRepeat, ++itLimits)
	{
		CopyToRange(AllRepeats, *itRepeat, itLimits->first, itLimits->second);
	}
	CMesh Mesh;	// Create an empty mesh
	Yarn.AddAABBToMesh(Mesh); // Adds bounding box of yarn surfaces to mesh (?)
	for (itRepeat = AllRepeats.begin(); itRepeat != AllRepeats.end(); ++itRepeat)
	{
		CMesh RepeatedMesh = Mesh;
		RepeatedMesh.Translate(*itRepeat);
		if (MeshIntersectsDomain(RepeatedMesh))
			FiniteRepeats.push_back(*itRepeat);
	}
	return FiniteRepeats;
}

pair<double, double> CDomain::GetLimits(XYZ RepeatVector, const CMesh &Mesh) const
{
	pair<double, double> DomainLimits = make_pair(0.0, 0.0);
	pair<double, double> MeshLimits = make_pair(0.0, 0.0);
	pair<double, double> Limits = make_pair(0.0, 0.0);
	double dPointDist;//, dFurthestPointDist = 0;
	vector<XYZ>::const_iterator itNode;
	const vector<XYZ> MeshNodes = m_Mesh.GetNodes();

	double dRepeatLength = GetLength(RepeatVector);
	RepeatVector /= dRepeatLength;
	//for (itNode = m_Mesh.NodesBegin(); itNode != m_Mesh.NodesEnd(); ++itNode)
	for (itNode = MeshNodes.begin(); itNode != MeshNodes.end(); ++itNode)
	{
		dPointDist = DotProduct(*itNode, RepeatVector);

		if (itNode == MeshNodes.begin())
			DomainLimits = make_pair(dPointDist, dPointDist);
		else if (DomainLimits.first > dPointDist)
			DomainLimits.first = dPointDist;
		else if (DomainLimits.second < dPointDist)
			DomainLimits.second = dPointDist;
	}

	for (itNode = Mesh.NodesBegin(); itNode != Mesh.NodesEnd(); ++itNode)
	{
		dPointDist = DotProduct(*itNode, RepeatVector);

		if (itNode == Mesh.NodesBegin())
			MeshLimits = make_pair(dPointDist, dPointDist);
		else if (MeshLimits.first > dPointDist)
			MeshLimits.first = dPointDist;
		else if (MeshLimits.second < dPointDist)
			MeshLimits.second = dPointDist;
	}
	Limits.first = (DomainLimits.first - MeshLimits.second) / dRepeatLength;
	Limits.second = (DomainLimits.second - MeshLimits.first) / dRepeatLength;
	return Limits;
}

vector<pair<int, int> > CDomain::GetRepeatLimits(const CYarn &Yarn) const
{
	//TGLOGINDENT("Getting yarn repeat limits");

	vector<pair<double, double> > AllRepeatLimitsPrevious;
	vector<pair<double, double> > AllRepeatLimits;
	AllRepeatLimits.resize(Yarn.GetRepeats().size(), pair<double, double>(0, 0));
	vector<XYZ>::const_iterator itRepeat;
	vector<PLANE>::const_iterator itPlane;
	int i;
	int iIterations = 0, iMaxIterations = 100;

	// Check for all repeats being set to zero
	int j = 0;
	for (itRepeat = Yarn.GetRepeats().begin(); itRepeat != Yarn.GetRepeats().end(); ++itRepeat)
	{
		if (!(*itRepeat))
			++j;
	}
	if (j == Yarn.GetRepeats().size())
		return vector<pair<int, int> >();

	// Use an iterative method to find the repeat limits
	do
	{
		++iIterations;
		AllRepeatLimitsPrevious = AllRepeatLimits;
		// Loop over all the repeats
		for (itRepeat = Yarn.GetRepeats().begin(), i = 0; itRepeat != Yarn.GetRepeats().end(); ++itRepeat, ++i)
		{
			AllRepeatLimits[i] = pair<double, double>(0, 0);
			CMesh Mesh;	// Create an empty mesh
						// Get a surface mesh of the yarns with the repeats calculated so far
			if (!Yarn.AddAABBToMesh(Mesh, ConvertLimitsToInt(AllRepeatLimits)))
			{
				TGERROR("Unable to calculate repeat limits");
				assert(false);
				return ConvertLimitsToInt(AllRepeatLimits);
			}

			AllRepeatLimits[i] = GetLimits(*itRepeat, Mesh);
		}
		// Keep going until the repeats from the previous iteration are the same as the repeats for the current
		// iteration. Or until the operation fails due to too many iterations.
	} while (AllRepeatLimitsPrevious != AllRepeatLimits && iIterations < iMaxIterations);
	if (iIterations >= iMaxIterations)
	{
		TGERROR("Unable to find yarn repeat limits, stopped after " << iIterations << " iterations");
		return vector<pair<int, int> >();
	}
	//TGLOG("Found yarn repeat limits after " << iIterations << " iterations");
	return ConvertLimitsToInt(AllRepeatLimits);
}

