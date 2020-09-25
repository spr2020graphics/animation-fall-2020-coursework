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

	a3_HierarchyState.c
	Implementation of transform hierarchy state.

	Animation Framework Addons by Scott Dagen and Cameron Schneider
*/

#include "../a3_HierarchyState.h"

#include <stdlib.h>
#include <string.h>


//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count (Scott Dagen)
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup* poseGroup_out, const a3_Hierarchy* hierarchy, const a3ui32 poseCount)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (poseGroup_out && hierarchy && !poseGroup_out->hierarchy && hierarchy->nodes)
	{
		a3ui32 numSpatialPoses = poseCount * hierarchy->numNodes;

		// determine memory requirements. Hierarchy Pose, Spatial Pose, and Channels
		size_t memRequirements = sizeof(a3_SpatialPose) * numSpatialPoses + sizeof(a3_HierarchyPose) * poseCount + sizeof(a3_SpatialPoseChannel) * hierarchy->numNodes;

		// allocate everything (one malloc)
		poseGroup_out->spatialPosePool = malloc(memRequirements);

		// set pointers
		poseGroup_out->hierarchy = hierarchy;
		poseGroup_out->hierarchyPosePool = (a3_HierarchyPose*)(poseGroup_out->spatialPosePool + numSpatialPoses); //shift over by the size of the spatial pool
		poseGroup_out->channels = (a3_SpatialPoseChannel*)(poseGroup_out->hierarchyPosePool + poseCount);

		// reset all data
		poseGroup_out->poseCount = poseCount;

		for (a3ui32 i = 0; i < poseCount; i++)
		{
			poseGroup_out->hierarchyPosePool[i].spatialPose = poseGroup_out->spatialPosePool + hierarchy->numNodes * i; //offset by the number of spatial nodes allocated to the last hPose
			a3hierarchyPoseReset(&poseGroup_out->hierarchyPosePool[i], hierarchy->numNodes);
		}

		for (a3ui32 i = 0; i < hierarchy->numNodes; i++)
		{
			poseGroup_out->channels[i] = a3poseChannel_none;
		}
		// done
		return 1;
	}
	return -1;
}

// release pose set (Scott Dagen)
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup* poseGroup)
{
	// validate param exists and is initialized
	if (poseGroup && poseGroup->hierarchy)
	{
		// release everything (one free)
		free(poseGroup->spatialPosePool);

		// reset pointers
		poseGroup->hierarchy = 0;
		poseGroup->spatialPosePool = 0;
		poseGroup->hierarchyPosePool = 0;
		poseGroup->channels = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy (Scott Dagen)
a3i32 a3hierarchyStateCreate(a3_HierarchyState* state_out, const a3_Hierarchy* hierarchy)
{
	// validate params and initialization states
	//	(output is not yet initialized, hierarchy is initialized)
	if (state_out && hierarchy && !state_out->hierarchy && hierarchy->nodes)
	{
		// determine memory requirements
		size_t memReqs = sizeof(a3_SpatialPose) * 3 * hierarchy->numNodes;
		// allocate everything (one malloc)
		state_out->sampleHPose->spatialPose = malloc(memReqs);

		state_out->localHPose->spatialPose = state_out->sampleHPose->spatialPose + hierarchy->numNodes;
		state_out->objectHPose->spatialPose = state_out->localHPose->spatialPose + hierarchy->numNodes;
		// set pointers
		state_out->hierarchy = hierarchy;

		// reset all data
		a3hierarchyPoseInit(state_out->localHPose, state_out->hierarchy->numNodes);
		a3hierarchyPoseInit(state_out->objectHPose, state_out->hierarchy->numNodes);
		a3hierarchyPoseInit(state_out->sampleHPose, state_out->hierarchy->numNodes);
		// done
		return 1;
	}
	return -1;
}

// release hierarchy state (Scott Dagen)
a3i32 a3hierarchyStateRelease(a3_HierarchyState* state)
{
	// validate param exists and is initialized
	if (state && state->hierarchy)
	{
		// release everything (one free)
		free(state->sampleHPose->spatialPose);

		// reset pointers
		state->hierarchy = 0;
		state->sampleHPose->spatialPose = 0;
		state->localHPose->spatialPose = 0;
		state->objectHPose->spatialPose = 0;

		// done
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->poseCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (poseGroup_out && !poseGroup_out->poseCount && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{

	}
	return -1;
}

//@author: Scott Dagen
//takes in a pose and a number of poses. If pose doesn't exist, calloc it. If it now exists, calloc the spatial pose and init/reset it.
a3i32 a3hierarchyPoseInit(a3_HierarchyPose* pose, a3ui32 nodeCount)
{
	if (!pose)
	{
		pose = calloc(1, sizeof(a3_HierarchyPose));
	}
	if (pose)
	{
		pose->spatialPose = calloc(nodeCount, sizeof(a3_SpatialPose));
		a3hierarchyPoseReset(pose, nodeCount);
	}
	if (!pose)
	{
		return -1;
	}
	return 1;
}


//-----------------------------------------------------------------------------
