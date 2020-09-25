/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_Kinematics.c
	Implementation of kinematics solvers.
*/

/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
*/

#include "../a3_Kinematics.h"
#include <stdlib.h>

//-----------------------------------------------------------------------------

// partial FK solver
a3i32 a3kinematicsSolveForwardPartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount)
{
	if (hierarchyState && hierarchyState->hierarchy && 
		firstIndex < hierarchyState->hierarchy->numNodes && nodeCount)
	{
		const a3_Hierarchy* hierarchy = hierarchyState->hierarchy;

		//loop from part of the way through the hierarchy (or from the beginning), till the end.
		for (a3ui32 i = firstIndex; i < hierarchy->numNodes; i++)
		{
			a3_HierarchyNode * currentNode = hierarchy->nodes + i;

			if (currentNode->parentIndex != -1)
			{
				// if we aren't the root node, our object space transform = parent object space * our local space
				a3real4x4Product(hierarchyState->objectHPose->spatialPose[currentNode->index].transform.m, hierarchyState->objectHPose->spatialPose[currentNode->parentIndex].transform.m, hierarchyState->localHPose->spatialPose[currentNode->index].transform.m);
			}
			else
			{
				// If we ARE the root, our object space transform is the same as our local space
				a3real4x4SetReal4x4(hierarchyState->objectHPose->spatialPose[currentNode->index].transform.m, hierarchyState->localHPose->spatialPose[currentNode->index].transform.m);
			}
		}
	}
	return -1;
}


//-----------------------------------------------------------------------------

// partial IK solver
a3i32 a3kinematicsSolveInversePartial(const a3_HierarchyState *hierarchyState, const a3ui32 firstIndex, const a3ui32 nodeCount)
{
	if (hierarchyState && hierarchyState->hierarchy &&
		firstIndex < hierarchyState->hierarchy->numNodes && nodeCount)
	{
		// ****TO-DO: implement inverse kinematics algorithm
		//	- for all nodes starting at first index
		//		- if node is not root (has parent node)
		//			- local matrix = inverse parent object matrix * object matrix
		//		- else
		//			- copy object matrix to local matrix
	}
	return -1;
}


//-----------------------------------------------------------------------------
