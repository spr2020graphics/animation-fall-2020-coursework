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

	a3_DemoMode1_Animation-load.c
	Demo mode implementations: animation scene.

	********************************************
	*** LOADING FOR ANIMATION SCENE MODE     ***
	********************************************
*/

/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_Animation.h"

#include "../a3_DemoState.h"

#include "../_animation/a3_HierarchyBlendNode.h"


//-----------------------------------------------------------------------------

/// <summary>
/// Pass in the tree, the demo state, and a pose group, and the tree will be configured appropriately.
/// </summary>
/// <param name="tree_in"></param>
/// <param name="demoState"></param>
/// <param name="selectedHierarchy"></param>
/// <returns></returns>
a3i32 a3hierarchyBlendTreeBind(a3_HierarchyBlendTree* tree_in, a3_DemoMode1_Animation* const demoMode, const a3_DemoState* demoState, a3_HierarchyPoseGroup* poseGroup)
{
	if (tree_in && demoMode)
	{
		//get total number of clip controllers to allocate
		a3i32 clipCtrlCount = 0;
		for (a3ui32 i = 0; i < tree_in->leafCount; i++)
		{
			clipCtrlCount += tree_in->blendNodes[tree_in->leafIndices[i]].clipCount;
		}

		demoMode->bTreeControllerCount = clipCtrlCount;
		if (demoMode->bTreeClipControllers)
		{
			free(demoMode->bTreeClipControllers); //clear out any old data
		}
		demoMode->bTreeClipControllers = malloc(clipCtrlCount * sizeof(a3_ClipController));

		a3ui32 controllerIndex = 0;

		//bind base inputs and init controllers
		for (a3ui32 leafIndex = 0; leafIndex < tree_in->leafCount; leafIndex++)
		{
			a3_HierarchyBlendNode* node = &tree_in->blendNodes[tree_in->leafIndices[leafIndex]];
			for (a3ui32 nodeClipIndex = 0; nodeClipIndex < node->clipCount; nodeClipIndex++)
			{
				//assign a clip controller and increment the controller index. Also assign the clip itself
				node->clipControllers[nodeClipIndex] = &demoMode->bTreeClipControllers[controllerIndex];
				a3ui32 clipPoolIndex = a3clipGetIndexInPool(demoState->clipPool, node->clipNames[nodeClipIndex]);
				a3clipControllerInit(node->clipControllers[nodeClipIndex], "CTRL", demoState->clipPool, clipPoolIndex);
				controllerIndex++;
			}
		}

		//next is binding hstates. Total HStates = all clip inputs + all node outputs, or more conveniently, controllerCount + nodeCount
		demoMode->bTreeHStateCount = clipCtrlCount + tree_in->bt_hierarchy->numNodes;

		//free hstates if necessary
		if (demoState->demoMode1_animation->bTreeHStates)
		{
			free(demoState->demoMode1_animation->bTreeHStates);
		}

		//create new hstates
		demoMode->bTreeHStates = calloc(demoState->demoMode1_animation->bTreeHStateCount, sizeof(a3_HierarchyState));
		//init hstates
		for (int hStateIndex = 0; hStateIndex < demoState->demoMode1_animation->bTreeHStateCount; hStateIndex++)
		{
			a3hierarchyStateCreate(&demoState->demoMode1_animation->bTreeHStates[hStateIndex], poseGroup->hierarchy);
		}

		//bind hState outputs
		a3ui32 hStateBindIndex = 0;
		for (a3ui32 nodeIndex = 0; nodeIndex < tree_in->bt_hierarchy->numNodes; nodeIndex++)
		{
			//clip nodes have to also load data into their input states
			a3_HierarchyBlendNode* node = &tree_in->blendNodes[nodeIndex];
			node->poseGroup = poseGroup;
			if (node->nodeType < identity) //clip node
			{
				for (a3ui32 clipIndex = 0; clipIndex < node->clipCount; clipIndex++) //all clip counts combined = clipCtrlCount, then we add in one per node
				{
					node->controlStates[clipIndex] = &demoMode->bTreeHStates[hStateBindIndex];
					hStateBindIndex++;
				}
			}
			node->state_out = &demoMode->bTreeHStates[hStateBindIndex];
			hStateBindIndex++;
		}

		//bind child poses!!
		for (a3ui32 nodeIndex = 0; nodeIndex < tree_in->bt_hierarchy->numNodes; nodeIndex++)
		{
			a3_HierarchyBlendNode* node = &tree_in->blendNodes[nodeIndex];
			if (node->nodeType >= identity) //internal node
			{
				//node index within node's control _node_index array
				for (a3ui32 controlIndex = 0; controlIndex < node->controlNodeCount; controlIndex++) //all clip counts combined = clipCtrlCount, then we add in one per node
				{
					a3ui32 controlNodeIndex = node->controlNodeIndices[controlIndex];
					node->controlStates[controlIndex] = tree_in->blendNodes[controlNodeIndex].state_out;
				}
			}
		}
		demoMode->blendTree = tree_in;
		return 1;
	}
	return -1;
}

// utility to load animation
void a3animation_init_animation(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode)
{
	// general counters
	//a3ui32 j, p;

	// object pointers
	a3_Hierarchy* hierarchy = 0;
	a3_HierarchyState* hierarchyState = 0;
	a3_HierarchyPoseGroup* hierarchyPoseGroup = 0;
	a3_SpatialPose* spatialPose = 0;
	//a3_SpatialPoseChannel spatialPoseChannel[32] = { a3poseChannel_none };

	a3_FileStream fileStream[1] = { 0 };
	const a3byte* const geometryStream = "./data/anim_data_gpro_base.dat";
	const a3byte* const animationfilePath = "../../../../resource/animdata/mixamo/multiclip.htr";
	const a3byte* const blendfilePath = "../../../../resource/animdata/blend_input.txt";

	// Assign hierarchyPoseGroup and hierarchy to new variables in demoState
	hierarchy = demoMode->hierarchy_skel; 
	hierarchyPoseGroup = demoMode->hierarchyPoseGroup_skel; //hierarchy_bvh

	//create BVH
	a3_HierarchyBlendTree* blendTree = calloc(1, sizeof(a3_HierarchyBlendTree));
	//a3hierarchyPoseGroupLoadBVH(hierarchyPoseGroup, hierarchy, animationfilePath);
	hierarchyPoseGroup->hierarchy = 0;
	a3hierarchyPoseGroupLoadHTR(hierarchyPoseGroup, hierarchy, animationfilePath);

	// stream animation assets
	if (demoState->streaming && a3fileStreamOpenRead(fileStream, geometryStream))
	{
		// load hierarchy assets
		hierarchy = demoMode->hierarchy_skel;
		a3hierarchyLoadBinary(hierarchy, fileStream);

		// done
		a3fileStreamClose(fileStream);
	}
	// not streaming or stream doesn't exist
	else if (false && (!demoState->streaming || a3fileStreamOpenWrite(fileStream, geometryStream)))
	{
		// manually set up a skeleton
		// first is the hierarchy: the general non-spatial relationship between bones
		const a3ui32 jointCount = 32;

		// indices of joints, their parents and branching joints
		a3ui32 jointIndex = 0;
		a3i32 jointParentIndex = -1;
		a3i32 rootJointIndex, upperSpineJointIndex, clavicleJointIndex, pelvisJointIndex;

		// initialize hierarchy
		hierarchy = demoMode->hierarchy_skel;
		a3hierarchyCreate(hierarchy, jointCount, 0);

		// set up joint relationships
		jointParentIndex = rootJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:root");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_lower");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_mid");
		jointParentIndex = upperSpineJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:spine_upper");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:neck");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:head");
		jointParentIndex = upperSpineJointIndex;
		jointParentIndex = clavicleJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:clavicle");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulderblade_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulder_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:elbow_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:forearm_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:wrist_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hand_r");
		jointParentIndex = clavicleJointIndex;
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulderblade_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shoulder_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:elbow_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:forearm_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:wrist_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hand_l");
		jointParentIndex = rootJointIndex;
		jointParentIndex = pelvisJointIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:pelvis");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hip_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:knee_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shin_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:ankle_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:foot_r");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:toe_r");
		jointParentIndex = pelvisJointIndex;
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:hip_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:knee_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:shin_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:ankle_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:foot_l");
		jointParentIndex = a3hierarchySetNode(hierarchy, jointIndex++, jointParentIndex, "skel:toe_l");

		// save hierarchy assets
		a3hierarchySaveBinary(hierarchy, fileStream);
	
		// done
		a3fileStreamClose(fileStream);
	}


	// scene objects
	demoMode->obj_skeleton->position.y = +a3real_four;
	demoMode->obj_skeleton->euler.z = +a3real_oneeighty;
	demoMode->obj_skeleton->euler.x = -a3real_ninety;

	// next set up hierarchy poses
	hierarchy = demoMode->hierarchy_skel;
	hierarchyPoseGroup = demoMode->hierarchyPoseGroup_skel;
	//hierarchyPoseGroup->hierarchy = 0;
	//a3hierarchyPoseGroupCreate(hierarchyPoseGroup, hierarchy, 5); //0 = base, 1 = zero delta, all after = delta poses

	// define "bind pose" or "base pose" or the initial transformation 
	//	description for each joint (not a literal transform)
	//p = 0;
	//a3vec3 rot;
	//a3vec3 pos;
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, 0.0f, 0.0f, +3.6f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, +3.6f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz | a3poseChannel_scale_xyz | a3poseChannel_translate_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_lower");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, -90.0f, -5.0f);
	//a3real3Set(pos.v, 0.0f, -0.1f, +0.1f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -5.0f);
	////a3spatialPoseSetTranslation(spatialPose, 0.0f, -0.1f, +0.1f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_mid");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, +10.0f);
	//a3real3Set(pos.v, +2.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
	////a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_upper");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, -5.0f);
	//a3real3Set(pos.v, +2.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -5.0f);
	////a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:neck");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, +0.5f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:head");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, +0.5f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:clavicle");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, +90.0f, 0.0f);
	//a3real3Set(pos.v, -0.1f, +0.1f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, 0.0f);
	////a3spatialPoseSetTranslation(spatialPose, -0.1f, +0.1f, 0.0f);
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, +0.5f, -0.1f, -0.5f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, +0.5f, -0.1f, -0.5f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_translate_yz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, +30.0f, -10.0f);
	//a3real3Set(pos.v, +0.5f, 0.0f, +0.5f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, +30.0f, -10.0f);
	////a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, +0.5f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, +20.0f);
	//a3real3Set(pos.v, +2.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
	////a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_z;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, +1.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_x;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, -10.0f);
	//a3real3Set(pos.v, +1.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
	////a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, +0.5f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, -0.5f, -0.1f, -0.5f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, -0.5f, -0.1f, -0.5f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_translate_yz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v , 0.0f, -30.0f, +10.0f);
	//a3real3Set(pos.v, -0.5f, 0.0f, +0.5f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, -30.0f, +10.0f);
	////a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, +0.5f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, -20.0f);
	//a3real3Set(pos.v, -2.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
	////a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_z;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, -1.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_x;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, +10.0f);
	//a3real3Set(pos.v, -1.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
	////a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, -0.5f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:pelvis");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, 0.0f, 0.0f, -0.1f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, -0.1f);
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, +90.0f, +10.0f);
	//a3real3Set(pos.v, +1.0f, -0.1f, +0.5f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, +10.0f);
	////a3spatialPoseSetTranslation(spatialPose, +1.0f, -0.1f, +0.5f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, -20.0f);
	//a3real3Set(pos.v, +2.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_z;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, +1.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_x;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, +90.0f);
	//a3real3Set(pos.v, +1.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +90.0f);
	////a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, +10.0f);
	//a3real3Set(pos.v, +0.5f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
	////a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_z;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, +0.5f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, -90.0f, -10.0f);
	//a3real3Set(pos.v, -1.0f, -0.1f, +0.5f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -10.0f);
	////a3spatialPoseSetTranslation(spatialPose, -1.0f, -0.1f, +0.5f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, +20.0f);
	//a3real3Set(pos.v, -2.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
	////a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_z;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, -1.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_x;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, -90.0f);
	//a3real3Set(pos.v, -1.0f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -90.0f);
	////a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_xyz;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//a3real3Set(rot.v, 0.0f, 0.0f, -10.0f);
	//a3real3Set(pos.v, -0.5f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
	////a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);
	//hierarchyPoseGroup->channels[j] = a3poseChannel_orient_z;
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, -0.5f, 0.0f, 0.0f);
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);
	//
	//// each remaining pose represents a "delta" from the base
	//// initialize the changes where applicable
	//// (note: the channels describe which pose components can change)
	//p = 2;
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//pos = a3vec3_zero;
	//a3real3Set(rot.v, 0.0f, 0.0f, +90.0f);	// rotate whole figure by 90 degrees on Z
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +90.0f);	// rotate whole figure by 90 degrees on Z
	//
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//pos = a3vec3_zero;
	//a3real3Set(rot.v, 0.0f, 0.0f, -50.0f);	// bend left elbow
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -50.0f);	// bend left elbow
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//pos = a3vec3_zero;
	//a3real3Set(rot.v, 20.0f, 0.0f, 0.0f);	//raise right leg
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 20.0f, 0.0f, 0.0f);	//raise right leg
	//
	//p = 3;
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//pos = a3vec3_zero;
	//rot = a3vec3_zero;
	//a3vec3 scale = { 1.5f, 1.5f, 1.5f };
	//a3spatialPoseOpInit(spatialPose, scale, rot, pos);
	////a3spatialPoseSetScale(spatialPose, 1.5f, 1.5f, 1.5f);	// uniformly scale whole figure up by 50%
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_l");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//pos = a3vec3_zero;
	//a3real3Set(rot.v, 0.0f, 0.0f, -90.0f);	//move arm forward
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -90.0f);	//move arm forward
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_r");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//pos = a3vec3_zero;
	//a3real3Set(rot.v, 0.0f, 0.0f, 90.0f);	//move arm forward
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, 90.0f);	//move arm forward
	//
	//
	//p = 4;
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//rot = a3vec3_zero;
	//a3real3Set(pos.v, +0.3f, +0.4f, -0.5f);	// shift whole figure by some vector
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	////a3spatialPoseSetTranslation(spatialPose, +0.3f, +0.4f, -0.5f);	// shift whole figure by some vector
	//
	//j = a3hierarchyGetNodeIndex(hierarchy, "skel:pelvis");
	//spatialPose = hierarchyPoseGroup->hierarchyPosePool[p].spatialPose + j;
	//pos = a3vec3_zero;
	//a3real3Set(rot.v, 0.0f, 0.0f, 45.0f);	//rotate lower body by 45 degrees.
	//a3spatialPoseOpInit(spatialPose, a3vec3_one, rot, pos);
	//a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, 45.0f);	//rotate lower body by 45 degrees.

	// finally set up hierarchy states
	hierarchyState = demoMode->hierarchyState_skel_base;
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, demoMode->hierarchy_skel); //hierarchy_bvh

	hierarchyState = demoMode->hierarchyState_skel_toggle;
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, demoMode->hierarchy_skel); //hierarchy_bvh

	hierarchyState = demoMode->hierarchyState_skel_clip;
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, demoMode->hierarchy_skel); //hierarchy_bvh

	//the 4 control poses
	hierarchyState = &demoMode->hierarchyState_skel_control[0];
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, demoMode->hierarchy_skel); //hierarchy_bvh

	hierarchyState = &demoMode->hierarchyState_skel_control[1];
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, demoMode->hierarchy_skel); //hierarchy_bvh

	hierarchyState = &demoMode->hierarchyState_skel_control[2];
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, demoMode->hierarchy_skel); //hierarchy_bvh

	hierarchyState = &demoMode->hierarchyState_skel_control[3];
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, demoMode->hierarchy_skel); //hierarchy_bvh

	hierarchyState = demoMode->hierarchyState_skel_output;
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, demoMode->hierarchy_skel); //hierarchy_bvh

	a3hierarchyBlendTreeLoad(blendTree, demoMode->hierarchy_skel, blendfilePath);
	a3hierarchyBlendTreeBind(blendTree, demoMode, demoState, hierarchyPoseGroup);

}


//-----------------------------------------------------------------------------

void a3animation_input(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt);
void a3animation_update(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt);
void a3animation_render(a3_DemoState const* demoState, a3_DemoMode1_Animation const* demoMode, a3f64 const dt);
void a3animation_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state);
void a3animation_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state);

void a3animation_loadValidate(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode)
{
	// initialize callbacks
	a3_DemoModeCallbacks* const callbacks = demoState->demoModeCallbacks + demoState_modeAnimation;
	callbacks->demoMode = demoMode;
	callbacks->handleInput =	(a3_DemoMode_EventCallback)		a3animation_input;
	callbacks->handleUpdate =	(a3_DemoMode_EventCallback)		a3animation_update;
	callbacks->handleRender =	(a3_DemoMode_EventCallbackConst)a3animation_render;
	callbacks->handleKeyPress = (a3_DemoMode_InputCallback)		a3animation_input_keyCharPress;
	callbacks->handleKeyHold =	(a3_DemoMode_InputCallback)		a3animation_input_keyCharHold;

	// initialize cameras dependent on viewport
	demoMode->proj_camera_main->aspect = demoState->frameAspect;
	a3demo_updateProjectorProjectionMat(demoMode->proj_camera_main);
	a3demo_setProjectorSceneObject(demoMode->proj_camera_main, demoMode->obj_camera_main);

	// initialize cameras not dependent on viewport

	// animation
	demoMode->hierarchyState_skel_base->hierarchy = demoMode->hierarchy_skel; //hierarchy_bvh
	demoMode->hierarchyPoseGroup_skel->hierarchy = demoMode->hierarchy_skel; //hierarchy_bvh
}


void a3animation_load(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode)
{
	a3ui32 i;

	a3_DemoSceneObject* currentSceneObject;
	a3_DemoProjector* projector;


	// camera's starting orientation depends on "vertical" axis
	// we want the exact same view in either case
	const a3real sceneCameraAxisPos = 20.0f;
	const a3vec3 sceneCameraStartPos = {
		+sceneCameraAxisPos,
		-sceneCameraAxisPos,
		+sceneCameraAxisPos + 5.0f,
	};
	const a3vec3 sceneCameraStartEuler = {
		+55.0f,
		+0.0f,
		+45.0f,
	};
	const a3f32 sceneObjectDistance = 8.0f;
	const a3f32 sceneObjectHeight = 2.0f;


	// all objects
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
		a3demo_initSceneObject(demoMode->object_scene + i);
	for (i = 0; i < animationMaxCount_cameraObject; ++i)
		a3demo_initSceneObject(demoMode->object_camera + i);
	for (i = 0; i < animationMaxCount_projector; ++i)
		a3demo_initProjector(demoMode->projector + i);

	currentSceneObject = demoMode->obj_skybox;
	currentSceneObject->scaleMode = 1;
	currentSceneObject->scale.x = 256.0f;

	currentSceneObject = demoMode->obj_skeleton;
	currentSceneObject->scaleMode = 0;


	// set up cameras
	projector = demoMode->proj_camera_main;
	projector->perspective = a3true;
	projector->fovy = a3real_fortyfive;
	projector->znear = 1.0f;
	projector->zfar = 1024.0f;
	projector->ctrlMoveSpeed = 10.0f;
	projector->ctrlRotateSpeed = 5.0f;
	projector->ctrlZoomSpeed = 5.0f;
	projector->sceneObject->position = sceneCameraStartPos;
	projector->sceneObject->euler = sceneCameraStartEuler;


	// set flags
	demoMode->render = animation_renderPhong;
	demoMode->display = animation_displayTexture;
	demoMode->activeCamera = animation_cameraSceneViewer;

	demoMode->pipeline = animation_forward;
	demoMode->pass = animation_passComposite;

	demoMode->targetIndex[animation_passScene] = animation_scene_finalcolor;
	demoMode->targetIndex[animation_passComposite] = animation_scene_finalcolor;

	demoMode->targetCount[animation_passScene] = animation_target_scene_max;
	demoMode->targetCount[animation_passComposite] = 1;

	demoMode->numHierarchyStates = 8;
	demoMode->currentExamineHierarchyState = 0;
	demoMode->currentExamineNode = 0;

	// setup
	a3animation_init_animation(demoState, demoMode);
}


//-----------------------------------------------------------------------------
