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
#include "../_animation/a3_Tree.h"
#include "../_animation/a3_HierarchyBlendNode.h"

#include "../a3_DemoState.h"
#include "A3_DEMO/_animation/a3_Raycasting.h"


//-----------------------------------------------------------------------------

void a3animation_load_resetEffectors(a3_DemoMode1_Animation* demoMode,
	a3_HierarchyState* hierarchyState, a3_HierarchyPoseGroup const* poseGroup)
{
	a3_DemoSceneObject* sceneObject = demoMode->obj_skeleton;
	a3ui32 j = sceneObject->sceneGraphIndex;

	// need to properly transform joints to their parent frame
	a3mat4 const skeletonToControl = demoMode->sceneGraphState->localSpace->pose[j].transformMat;
	a3vec4 controlLocator;

	// look-at effector
	// position in front of neck
	j = a3hierarchyGetNodeIndex(demoMode->hierarchy_skel, "mixamorig:Neck");
	sceneObject = demoMode->obj_skeleton_neckLookat_ctrl;
	a3real4Real4x4Product(controlLocator.v, skeletonToControl.m,
		hierarchyState->objectSpace->pose[j].transformMat.v3.v);
	sceneObject->position.x = controlLocator.x;
	sceneObject->position.y = controlLocator.y + a3real_four;
	sceneObject->position.z = controlLocator.z;
	sceneObject->scale.x = a3real_third;
	sceneObject->scaleMode = 1;

	// right wrist effector
	// position on wrist
	j = a3hierarchyGetNodeIndex(demoMode->hierarchy_skel, "mixamorig:RightHand");
	sceneObject = demoMode->obj_skeleton_wristEffector_r_ctrl;
	a3real4Real4x4Product(controlLocator.v, skeletonToControl.m,
		hierarchyState->objectSpace->pose[j].transformMat.v3.v);
	sceneObject->position.x = controlLocator.x;
	sceneObject->position.y = controlLocator.y;
	sceneObject->position.z = controlLocator.z;
	sceneObject->scale.x = a3real_third;
	sceneObject->scaleMode = 1;

	// right wrist constraint
	// position behind elbow
	j = a3hierarchyGetNodeIndex(demoMode->hierarchy_skel, "mixamorig:RightForeArm");
	sceneObject = demoMode->obj_skeleton_wristConstraint_r_ctrl;
	a3real4Real4x4Product(controlLocator.v, skeletonToControl.m,
		hierarchyState->objectSpace->pose[j].transformMat.v3.v);
	sceneObject->position.x = controlLocator.x;
	sceneObject->position.y = controlLocator.y - a3real_half;
	sceneObject->position.z = controlLocator.z;
	sceneObject->scale.x = a3real_third;
	sceneObject->scaleMode = 1;

	// right wrist base
	//j = a3hierarchyGetNodeIndex(demoMode->hierarchy_skel, "mixamorig:RightArm");
}

// utility to load animation
void a3animation_init_animation(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode)
{
	// general counters
	a3ui32 j, p;

	// object pointers
	a3_Hierarchy* hierarchy = 0;
	a3_HierarchyState* hierarchyState = 0;
	a3_HierarchyPoseGroup* hierarchyPoseGroup = 0;
	a3_SpatialPose* spatialPose = 0;

	a3_FileStream fileStream[1] = { 0 };
	const a3byte* const geometryStream = "./data/gpro_base_anim_working.dat";


	// next set up hierarchy poses
	hierarchy = demoMode->hierarchy_skel;
	hierarchyPoseGroup = demoMode->hierarchyPoseGroup_skel;

	// stream animation assets
	if (!demoState->streaming && a3fileStreamOpenRead(fileStream, geometryStream))
	{
		// load hierarchy assets
		a3hierarchyLoadBinary(demoMode->sceneGraph, fileStream);
		a3hierarchyLoadBinary(hierarchy, fileStream);
		hierarchyPoseGroup->hierarchy = hierarchy;
		a3hierarchyPoseGroupLoadBinary(hierarchyPoseGroup, fileStream);

		// done
		a3fileStreamClose(fileStream);
	}
	// not streaming or stream doesn't exist
	else if (!demoState->streaming || a3fileStreamOpenWrite(fileStream, geometryStream))
	{
		// set up scenegraph
		a3hierarchyCreate(demoMode->sceneGraph, animationMaxCount_sceneObject, 0);
		a3hierarchySetNode(demoMode->sceneGraph, 0, -1, "scene_world_root");
		a3hierarchySetNode(demoMode->sceneGraph, 1, 0, "scene_camera_main");
		a3hierarchySetNode(demoMode->sceneGraph, 2, 0, "scene_light_main");
		a3hierarchySetNode(demoMode->sceneGraph, 3, 0, "scene_skybox");
		a3hierarchySetNode(demoMode->sceneGraph, 4, 0, "scene_skeleton_ctrl");
		a3hierarchySetNode(demoMode->sceneGraph, 5, 4, "scene_skeleton_neckLookat_ctrl");
		a3hierarchySetNode(demoMode->sceneGraph, 6, 4, "scene_skeleton_wristEff_r_ctrl");
		a3hierarchySetNode(demoMode->sceneGraph, 7, 4, "scene_skeleton_wristCon_r_ctrl");
		a3hierarchySetNode(demoMode->sceneGraph, 8, 4, "scene_skeleton");
		a3hierarchySetNode(demoMode->sceneGraph, 9, 0, "scene_plane");
		a3hierarchySetNode(demoMode->sceneGraph, 10, 9, "scene_ramp");
		a3hierarchySetNode(demoMode->sceneGraph, 11, 9, "scene_landing");

	/*
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


		// allocate poses
		a3hierarchyPoseGroupCreate(hierarchyPoseGroup, hierarchy, 5, a3poseEulerOrder_xyz);

		// define "bind pose" or "base pose" or the initial transformation
		//	description for each joint (not a literal transform)
		p = 0;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, +3.6f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz | a3poseChannel_scale_xyz | a3poseChannel_translate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_lower");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -5.0f);
		a3spatialPoseSetTranslation(spatialPose, 0.0f, -0.1f, +0.1f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_mid");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:spine_upper");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -5.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:neck");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:head");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:clavicle");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, 0.0f);
		a3spatialPoseSetTranslation(spatialPose, -0.1f, +0.1f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, -0.1f, -0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_translate_yz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, +30.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, +0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulderblade_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, -0.5f, -0.1f, -0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_translate_yz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shoulder_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, -30.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, +0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:elbow_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
		a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:forearm_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:wrist_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hand_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:pelvis");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, -0.1f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, +90.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, +1.0f, -0.1f, +0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -20.0f);
		a3spatialPoseSetTranslation(spatialPose, +2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +90.0f);
		a3spatialPoseSetTranslation(spatialPose, +1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +10.0f);
		a3spatialPoseSetTranslation(spatialPose, +0.5f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_r");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, +0.2f, 0.0f, 0.0f);

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:hip_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, -90.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, -1.0f, -0.1f, +0.5f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:knee_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, +20.0f);
		a3spatialPoseSetTranslation(spatialPose, -2.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:shin_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_x;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:ankle_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -90.0f);
		a3spatialPoseSetTranslation(spatialPose, -1.0f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_xyz;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:foot_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, -10.0f);
		a3spatialPoseSetTranslation(spatialPose, -0.5f, 0.0f, 0.0f);
		hierarchyPoseGroup->channel[j] = a3poseChannel_rotate_z;

		j = a3hierarchyGetNodeIndex(hierarchy, "skel:toe_l");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, -0.2f, 0.0f, 0.0f);


		// each remaining pose represents a "delta" from the base
		// initialize the changes where applicable
		// (note: the channels describe which pose components can change)
		p = 2;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetRotation(spatialPose, +45.0f, +60.0f, +90.0f);	// rotate whole figure about all axes


		p = 3;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetScale(spatialPose, 2.0f, 2.0f, 2.0f);	// uniformly scale whole figure up to 200%


		p = 4;
		j = a3hierarchyGetNodeIndex(hierarchy, "skel:root");
		spatialPose = hierarchyPoseGroup->hpose[p].pose + j;
		a3spatialPoseSetTranslation(spatialPose, +3.0f, +4.0f, +5.0f);	// shift whole figure by some vector
	*/

		// load skeletal data from file
		hierarchyPoseGroup->hierarchy = 0;
		a3hierarchyPoseGroupLoadHTR(hierarchyPoseGroup, hierarchy,
			//"../../../../resource/animdata/egnaro/egnaro_skel_anim.htr");
			"../../../../resource/animdata/wolf-htr/wolf_loco.htr");

		// edit assets as needed
		// mixamo assets have the wrong base pose; use first key as base and subtract from all
		//p = 1;
		//a3hierarchyPoseCopy(hierarchyPoseGroup->hpose, hierarchyPoseGroup->hpose + p, hierarchy->numNodes);
		//for (; p < hierarchyPoseGroup->hposeCount; ++p)
		//	a3hierarchyPoseDeconcat(hierarchyPoseGroup->hpose + p, hierarchyPoseGroup->hpose + p,
		//		hierarchyPoseGroup->hpose, hierarchy->numNodes);

		//// furthermore, end joints were removed, so they have no animation data; initialize it as identity
		//for (j = a3hierarchyGetNodeIndex(hierarchy, "HeadTop_End"), p = 1;
		//	p < hierarchyPoseGroup->hposeCount; ++p)
		//	a3spatialPoseReset(hierarchyPoseGroup->hpose[p].pose + j);
		//for (j = a3hierarchyGetNodeIndex(hierarchy, "LeftToe_End"), p = 1;
		//	p < hierarchyPoseGroup->hposeCount; ++p)
		//	a3spatialPoseReset(hierarchyPoseGroup->hpose[p].pose + j);
		//for (j = a3hierarchyGetNodeIndex(hierarchy, "RightToe_End"), p = 1;
		//	p < hierarchyPoseGroup->hposeCount; ++p)
		//	a3spatialPoseReset(hierarchyPoseGroup->hpose[p].pose + j);

		// finally, append prefix names to match what is expected for skinning
		//a3hierarchyPrefixNodeNames(demoMode->hierarchy_skel, "mixamorig:");

		// save hierarchy assets
		a3hierarchySaveBinary(demoMode->sceneGraph, fileStream);
		a3hierarchySaveBinary(hierarchy, fileStream);
		a3hierarchyPoseGroupSaveBinary(hierarchyPoseGroup, fileStream);

		// done
		a3fileStreamClose(fileStream);
	}

	a3_Ray ray[1];
	a3_Plane plane[1];

	a3vec3 origin = { 2, 1, 0 };
	a3vec3 dir = { -1, 1, 3 };
	
	a3vec3 center = { 3, 0, 1 };
	a3vec3 norm = { 3, -2, 1 };
	a3vec3 bounds = { 3, 3, 1 }; // basically the "localScale" of the plane, a 3x3 square in this instance

	a3createRay(ray, &origin, &dir);
	a3createPlane(plane, &center, &norm, &bounds);

	a3vec3 intersection = a3vec3_zero;
	a3raycastGetCollisionUnboundedPlane(ray, plane, &intersection);

	// map relevant objects to scene graph
	demoMode->obj_world_root->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_world_root");
	demoMode->obj_camera_main->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_camera_main");
	demoMode->obj_light_main->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_light_main");
	demoMode->obj_skybox->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_skybox");
	demoMode->obj_skeleton_ctrl->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_skeleton_ctrl");
	demoMode->obj_skeleton_neckLookat_ctrl->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_skeleton_neckLookat_ctrl");
	demoMode->obj_skeleton_wristEffector_r_ctrl->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_skeleton_wristEff_r_ctrl");
	demoMode->obj_skeleton_wristConstraint_r_ctrl->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_skeleton_wristCon_r_ctrl");
	demoMode->obj_skeleton->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_skeleton");
	demoMode->obj_plane->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_plane");
	demoMode->obj_ramp->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_ramp");
	demoMode->obj_landing->sceneGraphIndex = a3hierarchyGetNodeIndex(demoMode->sceneGraph, "scene_landing");

	demoMode->obj_plane->scaleMode = 1;
	//demoMode->obj_plane->position.y += 10.0f;
	demoMode->obj_plane->euler.x = 90.0f;
	demoMode->obj_plane->scale.x = 4.0f;

	demoMode->obj_ramp->scaleMode = 1;
	demoMode->obj_ramp->position.z -= 1.866f;
	demoMode->obj_ramp->position.y += 0.5f;
	demoMode->obj_ramp->euler.x = 30.0f;
	demoMode->obj_ramp->scale.x = 1.0f;

	demoMode->obj_landing->scaleMode = 1;
	demoMode->obj_landing->position.z -= 1.866f * 2.0f;
	demoMode->obj_landing->position.y += 1.0f;
	demoMode->obj_landing->scale.x = 1.0f;

	// scene graph state
	demoMode->sceneGraphState->hierarchy = 0;
	a3hierarchyStateCreate(demoMode->sceneGraphState, demoMode->sceneGraph);

	/*
	* WOLF CLIPS
	* 
	* walk  - 49 frames  (0-48)
	* run   - 21 frames  (49-69)
	* idle  - 231 frames (70-300)
	* creep - 49 frames  (301-349)
	* 
	*/

	// clips and controllers
	{
		a3byte const* clipName[] = {
			"wolf",
			"wolf_walk",
			"wolf_run",
			"wolf_idle",
			"wolf_creep",
		};
		// forward branch transition list
		a3ivec2 clipIndexForward[] = {
			{0, 0},
			{1, 1},
			{2, 2},
			{3, 3},
			{4, 4},
		};

		// reverse branch transition list
		a3ivec2 clipIndexBack[] = {
			{0, 0},
			{1, 1},
			{2, 2},
			{3, 3},
			{4, 4},
		};

		a3ui32 const clipCount = sizeof(clipName) / sizeof(*clipName);
		a3ui32 const sampleIndexFirst[] = {
			1,1,50,71,302
		};
		a3ui32 const sampleIndexFinal[] = {
			350,49,70,301,350
		};
		a3ui32 const rate = 24;
		a3f64 const fps = (a3f64)rate;

		p = hierarchyPoseGroup->hposeCount;
		a3clipPoolCreate(demoMode->clipPool, clipCount, p - 1, p);
		for (j = 0; j < p; ++j)
			a3sampleInit(demoMode->clipPool->sample + j, j, fps);
		for (j = 0; j < p - 1; ++j)
			a3keyframeInit(demoMode->clipPool->keyframe + j,
				demoMode->clipPool->sample + j, demoMode->clipPool->sample + j + 1, fps);
		for (j = 0; j < clipCount; ++j)
		{
			a3clipInit(demoMode->clipPool->clip + j, clipName[j],
				demoMode->clipPool->keyframe + sampleIndexFirst[j],
				demoMode->clipPool->keyframe + sampleIndexFinal[j] - 1,
				(clipIndexForward + j)->v,
				(clipIndexBack + j)->v, demoMode->character->jumpTrigger);
			a3clipCalculateDuration(demoMode->clipPool, j, fps);
		}

		j = a3clipGetIndexInPool(demoMode->clipPool, "wolf");
		a3clipControllerInit(demoMode->clipCtrl, "wolf_ctrl", demoMode->clipPool, j, rate, fps);
		j = a3clipGetIndexInPool(demoMode->clipPool, "wolf_idle");
		a3clipControllerInit(demoMode->clipCtrlA, "wolf_ctrlA", demoMode->clipPool, j, rate, fps);
		j = a3clipGetIndexInPool(demoMode->clipPool, "wolf_walk");
		a3clipControllerInit(demoMode->clipCtrlB, "wolf_ctrlB", demoMode->clipPool, j, rate, fps);

		a3ui32 temp[4];

		// set up the character controller's target idle, jump, walk, and run indices
		j = a3clipGetIndexInPool(demoMode->clipPool, "wolf_idle");
		demoMode->character->idleClipIndex = j;
		temp[0] = j;
		j = a3clipGetIndexInPool(demoMode->clipPool, "wolf_walk");
		demoMode->character->walkClipIndex = j;
		temp[1] = j;
		j = a3clipGetIndexInPool(demoMode->clipPool, "wolf_run");
		demoMode->character->runClipIndex = j;
		temp[2] = j;
		j = a3clipGetIndexInPool(demoMode->clipPool, "wolf_creep");
		demoMode->character->jumpClipIndex = j;
		temp[3] = j;

		demoMode->walkThreshold = 4.0f;

		for (a3ui32 i = 0; i < 4; i++)
		{
			a3clipControllerInit(&demoMode->characterAnimControllers[i], "character\0", demoMode->clipPool, temp[i], rate, fps);
		}

		//init character controller once the clip controllers are initted.
		a3characterControllerInit(demoMode->character, demoMode->characterAnimControllers, demoMode->obj_skeleton_ctrl, hierarchyPoseGroup, &demoMode->jumpTrigger, demoMode->walkThreshold);

		// set up jump transition action
		j = a3clipGetIndexInPool(demoMode->clipPool, "wolf_creep");
		//a3clipTransitionBindAction(demoMode->clipCtrlA->clipPool->clip[j].transitionForward, &a3characterToggleIsJumping);


		a3hierarchyBlendTreeCreate(demoMode->character->blendTree, 4, true);

		a3hierarchyBlendNodeCreate(demoMode->character->blendTree->blendNodes[0], copyClip); //idle
		a3hierarchyBlendNodeBindClipController(demoMode->character->blendTree->blendNodes[0], &demoMode->characterAnimControllers[0], 0);
		demoMode->character->blendTree->blendNodes[0]->poseGroup = hierarchyPoseGroup;

		a3hierarchyBlendNodeCreate(demoMode->character->blendTree->blendNodes[1], lerpClip); //walk and run
		a3hierarchyBlendNodeBindClipController(demoMode->character->blendTree->blendNodes[1], &demoMode->characterAnimControllers[1], 0);
		a3hierarchyBlendNodeBindClipController(demoMode->character->blendTree->blendNodes[1], &demoMode->characterAnimControllers[2], 1);
		demoMode->character->blendTree->blendNodes[1]->poseGroup = hierarchyPoseGroup;
		//a3hierarchyBlendNodeAddControl(demoMode->character->blendTree->blendNodes[1], 0, &demoMode->character->currentVelocity); //incorrect but a good start

		a3hierarchyBlendNodeCreate(demoMode->character->blendTree->blendNodes[2], copyClip); //crawl
		a3hierarchyBlendNodeBindClipController(demoMode->character->blendTree->blendNodes[2], &demoMode->characterAnimControllers[3], 0);
		demoMode->character->blendTree->blendNodes[2]->poseGroup = hierarchyPoseGroup;

		a3hierarchyBlendNodeCreate(demoMode->character->blendTree->blendNodes[3], triangular); //idle + walk/run + crawl

		a3hierarchyBlendTreeAddNodeToTree(demoMode->character->blendTree, 3, -1);
		a3hierarchyBlendTreeAddNodeToTree(demoMode->character->blendTree, 0, 3);
		a3hierarchyBlendTreeAddNodeToTree(demoMode->character->blendTree, 1, 3);
		a3hierarchyBlendTreeAddNodeToTree(demoMode->character->blendTree, 2, 3);

		a3hierarchyBlendTreeBindStates(demoMode->character->blendTree, hierarchy, NULL);
	}

	// finally set up hierarchy states
	// base state for skeleton
	hierarchyState = demoMode->hierarchyState_skel_base;
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, hierarchy);
	a3hierarchyPoseCopy(hierarchyState->localSpace, hierarchyPoseGroup->hpose, hierarchy->numNodes);
	a3hierarchyPoseConvert(hierarchyState->localSpace, hierarchy->numNodes, hierarchyPoseGroup->channel, hierarchyPoseGroup->order);
	a3kinematicsSolveForward(hierarchyState);
	a3hierarchyStateUpdateLocalInverse(hierarchyState);
	a3hierarchyStateUpdateObjectInverse(hierarchyState);

	// FK state
	hierarchyState = demoMode->hierarchyState_skel_fk;
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, hierarchy);

	// IK state
	hierarchyState = demoMode->hierarchyState_skel_ik;
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, hierarchy);

	// final blend state
	hierarchyState = demoMode->hierarchyState_skel_final;
	hierarchyState->hierarchy = 0;
	a3hierarchyStateCreate(hierarchyState, hierarchy);

	// egnaro
	//demoMode->obj_skeleton->euler.x = -a3real_ninety;

	// xbot
	demoMode->obj_skeleton->euler.x = a3real_ninety;
	demoMode->obj_skeleton->euler.y = a3real_oneeighty;

	// control node
	//demoMode->obj_skeleton_ctrl->position.y = +a3real_four;
	demoMode->obj_skeleton_ctrl->euler.z = a3real_oneeighty;

	// effectors
	// do one update to get first pose for target IK frame
	{
		void a3animation_update_animation(a3_DemoMode1_Animation* demoMode, a3f64 const dt, a3boolean const updateIK);
		void a3animation_update_sceneGraph(a3_DemoMode1_Animation* demoMode, a3f64 const dt);
		for (p = 0; p < 3; ++p)
		{
			a3animation_update_animation(demoMode, 0.0, 0);
			a3animation_update_sceneGraph(demoMode, 0.0);
			//a3animation_load_resetEffectors(demoMode, demoMode->hierarchyState_skel_fk, hierarchyPoseGroup);
		}
	}
}


//-----------------------------------------------------------------------------

void a3animation_input(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt);
void a3animation_update(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt);
void a3animation_render(a3_DemoState const* demoState, a3_DemoMode1_Animation const* demoMode, a3f64 const dt);
void a3animation_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state);
void a3animation_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state);

void a3animation_loadValidate(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode)
{
	a3ui32 i, j;

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
	demoMode->sceneGraphState->hierarchy = demoMode->sceneGraph;
	demoMode->hierarchyPoseGroup_skel->hierarchy = demoMode->hierarchy_skel;
	demoMode->clipCtrl->clipPool = demoMode->clipPool;
	demoMode->clipCtrl->clip = demoMode->clipPool->clip + demoMode->clipCtrl->clipIndex;
	demoMode->clipCtrl->keyframe = demoMode->clipPool->keyframe + demoMode->clipCtrl->keyframeIndex;
	for (i = 0, j = sizeof(demoMode->hierarchyState_skel) / sizeof(a3_HierarchyState); i < j; ++i)
		demoMode->hierarchyState_skel[i].hierarchy = demoMode->hierarchy_skel;
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


	// setup
	a3animation_init_animation(demoState, demoMode);
}


//-----------------------------------------------------------------------------
