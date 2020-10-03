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
*/

/*
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
		a3_Stream fs[1] = { 0 };
		a3ui32 fileLength = a3streamLoadContents(&fs[0], resourceFilePath);
		if (fileLength <= 0)
		{
			return -1;
		}
		a3byte* contentsCopy = malloc(fs->length * sizeof(a3byte));
		strncpy(contentsCopy, fs->contents, fs->length);
		//loop 1:
		//count joints, create empty hierarchy. Also cache total number of channels.
		char* token = strtok((char*)contentsCopy, "\n");

		a3ui32 nodeCount = 0;
		a3ui32 channelCount = 0;
		while (token != NULL)
		{
			while (token[0] == ' ' || token[0] == '\t')
			{
				token++;
			}
			//https://stackoverflow.com/a/4771038
			if (strncmp(token, "JOINT", 5) == 0 || strncmp(token, "ROOT", 4) == 0 || strncmp(token, "End", 3) == 0)
			{
				nodeCount++;
			}
			if (strncmp(token, "CHANNELS", 8) == 0)
			{
				channelCount += atoi(token + 9);
			}

			token = strtok(NULL, "\n");
		}
		//loop 1.5:
		//create array specifying the number of channels per joint, used in loop 3
		a3ui32* channelsPerJoint = malloc(nodeCount * sizeof(a3ui32));	//parallel to joint indices

		strncpy(contentsCopy, fs->contents, fs->length); //reset contentsCopy array to master from fs
		token = strtok((char*)contentsCopy, "\n");

		a3ui32 jointIndex = 0;
		a3boolean isEndSite = false;
		while (token != NULL)
		{
			while (token[0] == ' ' || token[0] == '\t')
			{
				token++;
			}
			//https://stackoverflow.com/a/4771038
			if (strncmp(token, "JOINT", 5) == 0 || strncmp(token, "ROOT", 4) == 0)
			{
				jointIndex++;
			}

			if (strncmp(token, "End", 3) == 0)
			{
				jointIndex++;
				isEndSite = true;
			}
			else
			{
				isEndSite = false;
			}

			if (strncmp(token, "CHANNELS", 8) == 0)
			{
				if (!isEndSite)
				{
					channelsPerJoint[jointIndex] = (a3ui32)atoi(token + 9);
				}
			}
			else if (isEndSite)
			{
				channelsPerJoint[jointIndex] = 0;
			}

			token = strtok(NULL, "\n");
		}

		//loop 2:
		//construct hierarchy, track the number of {} to determine hierarchical relationship. "End site" ends a recursive loop. Name end sites "parentNodeName"Off.
		a3hierarchyCreate(hierarchy_out, nodeCount, 0);
		//open bracket => start new recursive loop, UNLESS line before is End Site, then you call a different function that's a singular loop. Continue until close bracket
		//should function return the pointer to the close bracket. May need to add 1 or so but that's fine. Scott needs to research how to process both \r\n and \n
		//offset is local translation, there is no built-in rotation.
		//when last loop terminates, we still have the pointer to the last close bracket.
		//loop 3: motion
		//store frames and frame time (fps is line 268, a3_callbacks.c).
		//find the number of lines to skip to match the fps
		//allocate a LOT of poses in the HPoseGroup
		//for (int i = 0; i < joints; i++)
		//{
		//	for (int j = 0; j < channels[i]; j++)
		//	{
		//		load data, checking the name of the data and going through a 6 part if or switch statement to get each data point loaded
		//	}
		//}


		//full clip has 1280 frames at 120fps
		//set fps to 30 we get 320 frames or 10.6 seconds
		//macarena is 103bpm
		//roughly 4.5 measures (each line is one measure, plus a shout between verses)
		//macarena takes ~0.58 seconds per beat, or 2.33 seconds per measure (4 beats)
		//multiply the seconds per measure by 4.5 measures and we get 10.485
		//backsolving reveals that the actual measures per verse is ~4.577 (repeating 7) seconds, so the BVH data is at approximately real time, potentially with an extra pause at the end.
		//note: this was calculated before we did any parsing.
		return 1;
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
