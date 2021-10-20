/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2021 Mikhail Matveev

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
#include "BendingPeriodicBoundaries.h"


using namespace TexGen;
using namespace std;

CBendingPeriodicBoundaries::CBendingPeriodicBoundaries(void)
{
}

CBendingPeriodicBoundaries::~CBendingPeriodicBoundaries(void)
{
}

void CBendingPeriodicBoundaries::OutputEquations( ostream& Output, int iBoundaryConditions )
{
	Output << "*********************************" << endl;
	Output << "***    BOUNDARY CONDITIONS    ***" << endl;
	Output << "*** Requires MPC.f subroutine ***" << endl;
	Output << "*********************************" << endl;

	Output << "*MPC, User, Mode=DOF " << endl;
	Output << "1, FaceA, FaceB, ConstraintsDriver0, ConstraintsDriver3" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "2, FaceA, FaceB, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "3, FaceA, FaceB, ConstraintsDriver3, ConstraintsDriver5" << endl;
	
	Output << "**" << endl;
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "4, FaceC, FaceD, ConstraintsDriver2, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "5, FaceC, FaceD, ConstraintsDriver1, ConstraintsDriver4" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "6, FaceC, FaceD, ConstraintsDriver4, ConstraintsDriver5" << endl;
	
	Output << "**" << endl;
	Output << "** Edges in x-direction" << endl;
	Output << "**" << endl;
	Output << "*MPC,USER,MODE=dof" << endl;
	Output << "1, Edge2, Edge1, ConstraintsDriver0, ConstraintsDriver3" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "2, Edge2, Edge1, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "3, Edge2, Edge1, ConstraintsDriver3, ConstraintsDriver5" << endl;
	
	Output << "*MPC,USER,MODE=dof" << endl;
	Output << "1, Edge6, Edge5, ConstraintsDriver0, ConstraintsDriver3" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "2, Edge6, Edge5, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "3, Edge6, Edge5, ConstraintsDriver3, ConstraintsDriver5" << endl;
	
	Output << "*MPC,USER,MODE=dof" << endl;
	Output << "1, Edge7, Edge8, ConstraintsDriver0, ConstraintsDriver3" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "2, Edge7, Edge8, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "3, Edge7, Edge8, ConstraintsDriver3, ConstraintsDriver5" << endl;

	Output << "**" << endl;
	Output << "** Masternodes in x-direction" << endl;
	Output << "**" << endl;
	Output << "*MPC,USER,MODE=dof" << endl;
	Output << "1, MasterNode2, MasterNode1, ConstraintsDriver0, ConstraintsDriver3" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "2, MasterNode2, MasterNode1, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "3, MasterNode2, MasterNode1, ConstraintsDriver3, ConstraintsDriver5" << endl;
	
	Output << "*MPC,USER,MODE=dof" << endl;
	Output << "1, MasterNode6, MasterNode5, ConstraintsDriver0, ConstraintsDriver3" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "2, MasterNode6, MasterNode5, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "3, MasterNode6, MasterNode5, ConstraintsDriver3, ConstraintsDriver5" << endl;
	Output << "**" << endl;
	Output << "** Edges in y-direction" << endl;
	Output << "**" << endl;
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "4, Edge10, Edge9, ConstraintsDriver2, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "5, Edge10, Edge9, ConstraintsDriver1, ConstraintsDriver4" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "6, Edge10, Edge9, ConstraintsDriver4, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "4, Edge12, Edge11, ConstraintsDriver2, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "5, Edge12, Edge11, ConstraintsDriver1, ConstraintsDriver4" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "6, Edge12, Edge11, ConstraintsDriver4, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "4, Edge4, Edge1, ConstraintsDriver2, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "5, Edge4, Edge1, ConstraintsDriver1, ConstraintsDriver4" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "6, Edge4, Edge1, ConstraintsDriver4, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "4, MasterNode4, MasterNode1, ConstraintsDriver2, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "5, MasterNode4, MasterNode1, ConstraintsDriver1, ConstraintsDriver4" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "6, MasterNode4, MasterNode1, ConstraintsDriver4, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "4, MasterNode8, MasterNode5, ConstraintsDriver2, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "5, MasterNode8, MasterNode5, ConstraintsDriver1, ConstraintsDriver4" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "6, MasterNode8, MasterNode5, ConstraintsDriver4, ConstraintsDriver5" << endl;
	
	Output << "**" << endl;
	Output << "** Diagonal edges" << endl;
	Output << "**" << endl;
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "7, Edge3, Edge1, ConstraintsDriver0, ConstraintsDriver3, ConstraintsDriver2, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "8, Edge3, Edge1, ConstraintsDriver5, ConstraintsDriver1, ConstraintsDriver4" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "9, Edge3, Edge1, ConstraintsDriver3, ConstraintsDriver4, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "7, Edge3, Edge1, ConstraintsDriver0, ConstraintsDriver3, ConstraintsDriver2, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "8, Edge3, Edge1, ConstraintsDriver5, ConstraintsDriver1, ConstraintsDriver4" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "9, Edge3, Edge1, ConstraintsDriver3, ConstraintsDriver4, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "7, MasterNode3, MasterNode1, ConstraintsDriver0, ConstraintsDriver3, ConstraintsDriver2, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "8, MasterNode3, MasterNode1, ConstraintsDriver5, ConstraintsDriver1, ConstraintsDriver4" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "9, MasterNode3, MasterNode1, ConstraintsDriver3, ConstraintsDriver4, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "7, MasterNode7, MasterNode5, ConstraintsDriver0, ConstraintsDriver3, ConstraintsDriver2, ConstraintsDriver5" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "8, MasterNode7, MasterNode5, ConstraintsDriver5, ConstraintsDriver1, ConstraintsDriver4" << endl;
	
	Output << "*MPC, User, Mode=DOF" << endl;
	Output << "9, MasterNode7, MasterNode5, ConstraintsDriver3, ConstraintsDriver4, ConstraintsDriver5" << endl;
	
	Output << "*Parameter" << endl;
	Output << "my_force=" << 1.0 / (m_DomSize.x * m_DomSize.y) << endl;

}

void CBendingPeriodicBoundaries::OutputStep( ostream& Output, int iBoundaryConditions )
{
	Output << "*******************" << endl;
	Output << "*** CREATE STEP ***" << endl;
	Output << "*******************" << endl;
	Output << endl;
	Output << "*Step, Name=In-plane and out-of-plane loading, perturbation" << endl;
	Output << "Elastic material property computation" << endl;
	Output << "*Static" << endl;
	Output << "******************" << endl;
	Output << "*** LOAD CASES ***" << endl;
	Output << "******************" << endl;

	for (int i = 0; i < 6; i++)
	{
		Output << "*Load case, Name=Load" << i << endl;
		Output << "*BOUNDARY" << endl;
		for (int j = 0; j < 6; j++)
		{
			if ( i != j )
				Output << "ConstraintsDriver" << j << ", 1, 1, 0" << endl;
			else
				Output << "ConstraintsDriver" << j << ", 1, 1, <my_force>" << endl;
		}
	}
	Output << endl;

	Output << "*End Step" << endl;
	Output << endl;
}