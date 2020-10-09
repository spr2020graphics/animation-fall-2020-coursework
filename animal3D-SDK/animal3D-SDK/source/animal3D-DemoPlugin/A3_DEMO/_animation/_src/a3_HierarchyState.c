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
#include <stdio.h>


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
		size_t memRequirements = sizeof(a3_SpatialPose) * numSpatialPoses + sizeof(a3_HierarchyPose) * poseCount + sizeof(a3_SpatialPoseChannel) * hierarchy->numNodes + sizeof(a3_SpatialPoseEulerOrder) * hierarchy->numNodes;

		// allocate everything (one malloc)
		poseGroup_out->spatialPosePool = malloc(memRequirements);

		// set pointers
		poseGroup_out->hierarchy = hierarchy;
		poseGroup_out->hierarchyPosePool = (a3_HierarchyPose*)(poseGroup_out->spatialPosePool + numSpatialPoses); //shift over by the size of the spatial pool
		poseGroup_out->channels = (a3_SpatialPoseChannel*)(poseGroup_out->hierarchyPosePool + poseCount);
		poseGroup_out->eulerOrders = (a3_SpatialPoseEulerOrder*)(poseGroup_out->channels + hierarchy->numNodes);

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
			poseGroup_out->eulerOrders[i] = a3poseEulerOrder_xyz;
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
		poseGroup->eulerOrders = 0;

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

/// <summary>
/// Parse channels
/// </summary>
/// <param name="line">The line of text to parse into channels</param>
/// <param name="index">The node's index</param>
/// <param name="poseGroup">the poseGroup to store things in</param>
/// <returns></returns>
a3i32 a3BVHParseChannels(a3byte* line, a3i32 index, a3_HierarchyPoseGroup* poseGroup, a3_SpatialPoseChannel** channelData)
{
	if (!(poseGroup && line))
	{
		return -1;
	}
	//skip past "CHANNELS "
	a3byte* parsePos = line + 9;
	a3i32 channelCount = atoi(parsePos);
	//create new poseChannel array
	if (channelCount > 0)
	{
		channelData[index] = calloc(channelCount, sizeof(a3_SpatialPoseChannel));
	}

	size_t len = strlen(parsePos);
	if (*(parsePos + (len - 1)) == '\r') //delete \r if it exists
	{
		*(parsePos + (len - 1)) = '\0';
	}
	parsePos = strchr(parsePos, ' ') + 1; //advance past the number of channels
	a3i32* eulerOrderArr = calloc(3, sizeof(a3_SpatialPoseChannel)); //they're stored in order of rotation, so they need to be flipped
	eulerOrderArr[0] = eulerOrderArr[1] = eulerOrderArr[2] = -1;
	a3i32 eulerIndex = 2;
	//need to advance past the channel count
	for (int i = 0; i < channelCount; i++) //loop through all channels and store the data both as a combined mask and as an array
	{
		if (strncmp(parsePos, "Xpos", 4) == 0)
		{
			poseGroup->channels[index] |= a3poseChannel_translate_x;
			channelData[index][i] = a3poseChannel_translate_x;
		}
		else if (strncmp(parsePos, "Ypos", 4) == 0)
		{
			poseGroup->channels[index] |= a3poseChannel_translate_y;
			channelData[index][i] = a3poseChannel_translate_y;
		}
		else if (strncmp(parsePos, "Zpos", 4) == 0)
		{
			poseGroup->channels[index] |= a3poseChannel_translate_z;
			channelData[index][i] = a3poseChannel_translate_z;
		}
		else if (strncmp(parsePos, "Xrot", 4) == 0)
		{
			poseGroup->channels[index] |= a3poseChannel_orient_x;
			channelData[index][i] = a3poseChannel_orient_x;
			eulerOrderArr[eulerIndex] = a3poseChannel_orient_x;
			eulerIndex--;
		}
		else if (strncmp(parsePos, "Yrot", 4) == 0)
		{
			poseGroup->channels[index] |= a3poseChannel_orient_y;
			channelData[index][i] = a3poseChannel_orient_y;
			eulerOrderArr[eulerIndex] = a3poseChannel_orient_y;
			eulerIndex--;
		}
		else if (strncmp(parsePos, "Zrot", 4) == 0)
		{
			poseGroup->channels[index] |= a3poseChannel_orient_z;
			channelData[index][i] = a3poseChannel_orient_z;
			eulerOrderArr[eulerIndex] = a3poseChannel_orient_z;
			eulerIndex--;
		}
		parsePos = strchr(parsePos, ' ') + 1; //advance past the number of channels. parsePos will be 1 at the end of the loop, but that's fine
	}

	//figure out Euler order from the array. The array construction reverses it, but it's easier to construct the order in the order it's applied
	if (eulerOrderArr[2] == a3poseChannel_orient_z && eulerOrderArr[1] == a3poseChannel_orient_x) //first rotation's z, second is x
	{
		poseGroup->eulerOrders[index] = a3poseEulerOrder_yxz;
	}
	else if (eulerOrderArr[2] == a3poseChannel_orient_z && eulerOrderArr[1] == a3poseChannel_orient_y) //first rotation's z, second is y
	{
		poseGroup->eulerOrders[index] = a3poseEulerOrder_xyz;
	}
	else if (eulerOrderArr[2] == a3poseChannel_orient_y && eulerOrderArr[1] == a3poseChannel_orient_x) //first rotation's y, second is x
	{
		poseGroup->eulerOrders[index] = a3poseEulerOrder_zxy;
	}
	else if (eulerOrderArr[2] == a3poseChannel_orient_y && eulerOrderArr[1] == a3poseChannel_orient_z) //first rotation's y, second is z
	{
		poseGroup->eulerOrders[index] = a3poseEulerOrder_xzy;
	}
	else if (eulerOrderArr[2] == a3poseChannel_orient_x && eulerOrderArr[1] == a3poseChannel_orient_y) //first rotation's x, second is y
	{
		poseGroup->eulerOrders[index] = a3poseEulerOrder_zyx;
	}
	else if (eulerOrderArr[2] == a3poseChannel_orient_x && eulerOrderArr[1] == a3poseChannel_orient_z) //first rotation's x, second is z
	{
		poseGroup->eulerOrders[index] = a3poseEulerOrder_yzx;
	}
	free(eulerOrderArr);
	return 1;
}

/// <summary>
/// 
/// </summary>
/// <param name="currentTextPtr"></param>
/// <param name="source"></param>
/// <param name="hierarchy_out"></param>
/// <param name="poseGroup_out"></param>
/// <param name="parentJoint"></param>
/// <param name="jointIndexPtr"></param>
/// <returns></returns>
a3i32 a3BVHParseJoint(a3byte** currentTextPtr, const a3byte* source, a3_Hierarchy* hierarchy_out, a3_HierarchyPoseGroup* poseGroup_out, a3i32 parentJoint, a3i32* jointIndexPtr, a3boolean isEnd, a3_SpatialPoseChannel** channelData)
{
	if (!(currentTextPtr && hierarchy_out && poseGroup_out))
	{
		return -1;
	}
	a3byte* parsePos = *currentTextPtr + 6;
	size_t parentLen = strlen(hierarchy_out->nodes[parentJoint].name);
	//create end name if we're at an end point
	a3byte* endName = calloc(parentLen + 4, sizeof(a3byte));
	strcpy(endName, hierarchy_out->nodes[parentJoint].name);
	endName[strlen(hierarchy_out->nodes[parentJoint].name)] = 'E';
	endName[strlen(hierarchy_out->nodes[parentJoint].name) + 1] = 'n';
	endName[strlen(hierarchy_out->nodes[parentJoint].name) + 2] = 'd';
	endName[strlen(hierarchy_out->nodes[parentJoint].name) + 3] = '\0';

	a3i32 channelCount = atoi(parsePos);
	size_t len = strlen(parsePos);
	if (*(parsePos + (len - 1)) == '\r') //trim \r
	{
		*(parsePos + (len - 1)) = '\0';
	}
	a3i32 thisIndex = a3hierarchySetNode(hierarchy_out, *jointIndexPtr, parentJoint, isEnd ? endName : parsePos);
	a3_SpatialPose* spatialPose = poseGroup_out->hierarchyPosePool[0].spatialPose + thisIndex;
	*jointIndexPtr += 1;
	*currentTextPtr = strtok(NULL, "\n"); //skip open bracket
	a3boolean offsetParsed = false, channelsParsed = false;
	while (*currentTextPtr != NULL)
	{
		while (*currentTextPtr[0] == ' ' || *currentTextPtr[0] == '\t')
		{
			*currentTextPtr += 1;
		}
		if (strncmp(*currentTextPtr, "OFFSET", 6) == 0 && !offsetParsed)
		{
			//parse offset
			offsetParsed = true;
			//http://www.cplusplus.com/reference/cstdlib/strtof/
			a3vec3 offset;
			a3byte* nextDigit;
			offset.x = strtof(*currentTextPtr + 7, &nextDigit);
			offset.y = strtof(nextDigit, &nextDigit);
			offset.z = strtof(nextDigit, NULL);
			a3spatialPoseSetTranslation(spatialPose, offset.x, offset.y, offset.z);
		}
		if (strncmp(*currentTextPtr, "CHANNELS", 8) == 0 && !channelsParsed)
		{
			//parse channels
			channelsParsed = true;
			a3BVHParseChannels(*currentTextPtr, thisIndex, poseGroup_out, channelData);
		}
		if (strncmp(*currentTextPtr, "JOINT", 5) == 0 || strncmp(*currentTextPtr, "End Site", 8) == 0)
		{
			//passes in the index for the new joint and runs the same function
			a3BVHParseJoint(currentTextPtr, source, hierarchy_out, poseGroup_out, thisIndex, jointIndexPtr, strncmp(*currentTextPtr, "End Site", 8) == 0, channelData);
			continue; //don't want to risk the close bracket triggering early
		}
		//how to exit the function without it breaking. The next strtok() advances the pointer again so it doesn't trip this same line in the parent function
		if (strncmp(*currentTextPtr, "}", 1) == 0)
		{
			*currentTextPtr = strtok(NULL, "\n");
			break;
		}
		*currentTextPtr = strtok(NULL, "\n");
	}
	return 1;
}

/// <summary>
/// 
/// </summary>
/// <param name="currentTextPtr">Takes in a pointer _to_ the current text pointer, allowing it to be modified from within the function</param>
/// <param name="source"></param>
/// <param name="hierarchy_out"></param>
/// <returns></returns>
a3i32 a3BVHParseRoot(a3byte** currentTextPtr, const a3byte* source, a3_Hierarchy* hierarchy_out, a3_HierarchyPoseGroup* poseGroup_out, a3_SpatialPoseChannel** channelData)
{
	//passed throughout parser
	a3i32 jointIndex = 0;
	//get root name
	size_t len = strlen(*currentTextPtr);

	//delete carriage return
	if (*((*currentTextPtr) + (len - 1)) == '\r')
	{
		*((*currentTextPtr) + (len - 1)) = '\0';
	}
	a3i32 rootIndex = a3hierarchySetNode(hierarchy_out, jointIndex++, -1, (*currentTextPtr) + 5); //+5 to remove "ROOT "
	*currentTextPtr = strtok(NULL, "\n"); //skip open bracket

	a3_SpatialPose* spatialPose = poseGroup_out->hierarchyPosePool[0].spatialPose + 0; //root, so we know this value

	a3boolean offsetParsed = false, channelsParsed = false;
	while (*currentTextPtr != NULL)
	{
		//trim tabs and spaces
		while (*currentTextPtr[0] == ' ' || *currentTextPtr[0] == '\t')
		{
			*currentTextPtr += 1;
		}
		if (strncmp(*currentTextPtr, "OFFSET", 6) == 0 && !offsetParsed)
		{
			//parse offset
			offsetParsed = true;
			//http://www.cplusplus.com/reference/cstdlib/strtof/
			a3vec3 offset;
			a3byte* nextDigit;
			offset.x = strtof(*currentTextPtr + 7, &nextDigit);
			offset.y = strtof(nextDigit, &nextDigit);
			offset.z = strtof(nextDigit, NULL);
			a3spatialPoseSetTranslation(spatialPose, offset.x, offset.y, offset.z);
		}
		if (strncmp(*currentTextPtr, "CHANNELS", 8) == 0 && !channelsParsed)
		{
			channelsParsed = true;
			a3BVHParseChannels(*currentTextPtr, 0, poseGroup_out, channelData);
		}
		if (strncmp(*currentTextPtr, "JOINT", 5) == 0)
		{
			//passes in the index for the new joint
			a3BVHParseJoint(currentTextPtr, source, hierarchy_out, poseGroup_out, 0, &jointIndex, false, channelData);
			continue; //don't want to risk the close bracket triggering early
		}
		if (strncmp(*currentTextPtr, "}", 1) == 0)
		{
			*currentTextPtr = strtok(NULL, "\n");
			break;
		}
		*currentTextPtr = strtok(NULL, "\n");
	}
	return 1;
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
		a3ui32* channelsPerJoint = calloc(nodeCount, sizeof(a3ui32));	//parallel to joint indices
		strncpy(contentsCopy, fs->contents, fs->length); //reset contentsCopy array to master from fs
		token = strtok((char*)contentsCopy, "\n");

		a3ui32 jointIndex = -1; //start at -1 so the first time it spots Root it increments to 0.
		a3boolean isEndSite = false;

		a3i32 frameCount = 0;
		a3real frameTime = 0;
		while (token != NULL)
		{
			while (token[0] == ' ' || token[0] == '\t')
			{
				token++;
			}
			//If we find a joint or root, we know we're not at an end site
			if (strncmp(token, "JOINT", 5) == 0 || strncmp(token, "ROOT", 4) == 0)
			{
				jointIndex++;
				isEndSite = false;
			}

			if (strncmp(token, "End", 3) == 0)
			{
				jointIndex++;
				isEndSite = true;
			}

			if (strncmp(token, "Frames", 6) == 0)
			{
				frameCount = atoi(token + 7);
			}

			if (strncmp(token, "Frame Time", 10) == 0)
			{
				a3byte* temp = token + 12;
				frameTime = (a3real)strtof(temp, &temp);
			}

			//if we've found a channel and it's not an end site, store the number of channels
			if (strncmp(token, "CHANNELS", 8) == 0)
			{
				if (!isEndSite)
				{
					channelsPerJoint[jointIndex] = (a3ui32)atoi(token + 9);
				}
			}
			//end site is only true if the last keyword we found was "End". This will run a few times before the joint increments, but it's the same 0-assignment each time.
			else if (isEndSite)
			{
				channelsPerJoint[jointIndex] = 0;
			}

			token = strtok(NULL, "\n");
		}



		//loop 2:
		//construct hierarchy, track the number of {} to determine hierarchical relationship. "End site" ends a recursive loop. Name end sites "parentNodeName"Off.
		a3hierarchyCreate(hierarchy_out, nodeCount, 0);
		a3hierarchyPoseGroupCreate(poseGroup_out, hierarchy_out, frameCount + 2); // 0 = base pose, 1 = zero-delta (no information actually required), 2 = first motion pose
		//open bracket => start new recursive loop, UNLESS line before is End Site, then you call a different function that's a singular loop. Continue until close bracket
		//should function return the pointer to the close bracket. May need to add 1 or so but that's fine. Scott needs to research how to process both \r\n and \n
		//offset is local translation, there is no built-in rotation.
		//when last loop terminates, we still have the pointer to the last close bracket.
		strncpy(contentsCopy, fs->contents, fs->length); //reset data again
		token = strtok((char*)contentsCopy, "\n");
		jointIndex = -1;
		a3_SpatialPoseChannel** channelNames = calloc(nodeCount, sizeof(a3_SpatialPoseChannel*)); //points to first pointer in an array of pointers
		while (token != NULL)
		{
			if (strncmp(token, "ROOT", 4) == 0)
			{
				a3BVHParseRoot(&token, contentsCopy, hierarchy_out, poseGroup_out, channelNames);
				break;
			}
			token = strtok(NULL, "\n");
		}



		//loop 3: motion
		//store frames and frame time (fps is line 268, a3_callbacks.c).
		//find the number of lines to skip to match the fps
		//allocate a LOT of poses in the HPoseGroup
		//while file, track frame index.

		// We also generate a clip file (to use with our parser from project 1) with all keyframes and a clip declaration

		a3i32 currentFrame = 0;
		//skip to "MOTION" in case there's extra lines
		while (strncmp(token, "MOTION", 6) != 0)
		{
			token = strtok(NULL, "\n");
		}
		//next line, skips the frame count and frame time since we have that already
		token = strtok(NULL, "\n");
		token = strtok(NULL, "\n");
		token = strtok(NULL, "\n");

		// Variables used to create a clip file corresponding to BVH data
		a3real clipDuration = frameCount * frameTime;
		const a3byte* clipPath = "../../../../resource/animdata/bvh_clip.txt";
		a3byte* tmpIndex = malloc(sizeof(int));
		a3byte* keyframePrefix = "$ 1.0 ";
		a3byte* clipPrefix = "@ bvh-skel true 0 ";	// default name, we also want to use the clip duration and not individual keyframe durations
		a3byte* clipTransitions = " >| bvh-skel >| bvh-skel";	 //just default for now
		char* space = " ";

		FILE* clipFile = fopen(clipPath, "w+");

		while (currentFrame < frameCount)
		{
			a3byte* tmpPtr = token;
			for (a3ui32 i = 0; i < hierarchy_out->numNodes; i++)
			{
				a3_SpatialPose* poseToEdit = &poseGroup_out->hierarchyPosePool[currentFrame + 2].spatialPose[i];
				a3vec3 pos = a3vec3_zero, rot = a3vec3_zero;
				//for (a3ui32 j = 0; j < channelsPerJoint[j]; j++)
				//{
				//	if (channelNames[i] != NULL)
				//	{
				//		switch (channelNames[i][j])
				//		{
				//		case a3poseChannel_orient_x:
				//			rot.x = strtof(tmpPtr, &tmpPtr);
				//			break;
				//		case a3poseChannel_orient_y:
				//			rot.y = strtof(tmpPtr, &tmpPtr);
				//			break;
				//		case a3poseChannel_orient_z:
				//			rot.z = strtof(tmpPtr, &tmpPtr);
				//			break;
				//		case a3poseChannel_translate_x:
				//			pos.x = strtof(tmpPtr, &tmpPtr);
				//			break;
				//		case a3poseChannel_translate_y:
				//			pos.y = strtof(tmpPtr, &tmpPtr);
				//			break;
				//		case a3poseChannel_translate_z:
				//			pos.z = strtof(tmpPtr, &tmpPtr);
				//			break;
				//		default:
				//			break;
				//		}
				//		if (j != channelsPerJoint[j] - 1)
				//		{
				//			tmpPtr = strchr(tmpPtr, ' ') + 1;
				//		}
				//	}
				//}
				//a3spatialPoseSetTranslation(poseToEdit, pos.x, pos.y, pos.z);
				//a3spatialPoseSetRotation(poseToEdit, rot.x, rot.y, rot.z);
				if (channelsPerJoint[i] == 6)	//This is the root node
				{
					pos.x = strtof(tmpPtr, &tmpPtr);
					tmpPtr = strchr(tmpPtr, ' ') + 1;
					pos.y = strtof(tmpPtr, &tmpPtr);
					tmpPtr = strchr(tmpPtr, ' ') + 1;
					pos.z = strtof(tmpPtr, &tmpPtr);
					tmpPtr = strchr(tmpPtr, ' ') + 1;
					a3spatialPoseSetTranslation(poseToEdit, pos.x, pos.y, pos.z);
					
					//root has a different xyz order
					rot.z = strtof(tmpPtr, &tmpPtr);
					tmpPtr = strchr(tmpPtr, ' ') + 1;
					rot.y = strtof(tmpPtr, &tmpPtr);
					tmpPtr = strchr(tmpPtr, ' ') + 1;
					rot.x = strtof(tmpPtr, &tmpPtr);
					a3spatialPoseSetRotation(poseToEdit, rot.x, rot.y, rot.z);
				}
				else if (channelsPerJoint[i] != 0)	// this is NOT a root node (i.e joint), we don't need to handle end-sites
				{
					rot.z = strtof(tmpPtr, &tmpPtr);
					tmpPtr = strchr(tmpPtr, ' ') + 1;
					rot.x = strtof(tmpPtr, &tmpPtr);
					tmpPtr = strchr(tmpPtr, ' ') + 1;
					rot.y = strtof(tmpPtr, &tmpPtr);
					a3spatialPoseSetRotation(poseToEdit, rot.x, rot.y, rot.z);
				}
					//load data, checking the name of the dataand going through a 6 part if or switch statement to get each data point loaded
					
					//according to the doc we were given, it's pretty much exclusively 0, 3, or 6, with set orders for data. We could cache the order earlier and allow for nonstandard configurations,
					//but that doesn't seem necessary.
					
					//check for next space
					
			}

			//create new keyframe line
			a3byte* result = calloc(12, sizeof(char));

			strcat(result, keyframePrefix);
			_itoa(currentFrame, tmpIndex, 10);
			strcat(result, tmpIndex);
			strcat(result, space);
			strcat(result, tmpIndex);
			strcat(result, "\n");

			// Write new keyframe to file
			if (fputs(result, clipFile) == EOF)
			{
				fputs("Failed", clipFile);
			}

			currentFrame++;
			token = strtok(NULL, "\n");
		}

		// create the clip definition line
		a3byte* clipLine = calloc(25, sizeof(char));
		a3byte* strFloat = malloc(sizeof(float));


		fputs("#---------------------------------------------------------#\n\n", clipFile);


		strcat(clipLine, clipPrefix);
		_itoa(frameCount - 1, tmpIndex, 10);	//convert int to string, last param is numeric base
		strcat(clipLine, tmpIndex);
		strcat(clipLine, clipTransitions);
		_gcvt(clipDuration, 5, strFloat);	//convert float to string, middle param is # of digits
		strcat(clipLine, space);
		strcat(clipLine, strFloat);
		strcat(clipLine, "\n");

		// write clip declaration to file
		fputs(clipLine, clipFile);

		fclose(clipFile);


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
