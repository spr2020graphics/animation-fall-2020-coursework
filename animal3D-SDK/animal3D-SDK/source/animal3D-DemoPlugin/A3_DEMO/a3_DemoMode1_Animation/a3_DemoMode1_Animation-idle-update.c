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
	
	a3_DemoMode1_Animation-idle-update.c
	Demo mode implementations: animation scene.

	********************************************
	*** UPDATE FOR ANIMATION SCENE MODE      ***
	********************************************
*/

/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_Animation.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"
#include "../_a3_demo_utilities/a3_DemoMacros.h"


//-----------------------------------------------------------------------------
// UTILS

inline a3real4r a3demo_mat2quat_safe(a3real4 q, a3real4x4 const m)
{
	// ****TO-DO: 
	//	-> convert rotation part of matrix to quaternion
	//	-> NOTE: this is for testing dual quaternion skinning only; 
	//		quaternion data would normally be computed with poses

	return q;
}


//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update_objects(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3boolean useZYX, a3boolean applyScale);
void a3demo_update_defaultAnimation(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3ui32 axis);
void a3demo_update_bindSkybox(a3_DemoSceneObject* obj_camera, a3_DemoSceneObject* obj_skybox);
void a3demo_update_pointLight(a3_DemoSceneObject* obj_camera, a3_DemoPointLight* pointLightBase, a3ui32 count);

void a3demo_applyScale_internal(a3_DemoSceneObject* sceneObject, a3real4x4p s);


void a3animation_update(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt)
{
	a3ui32 i;
	a3_DemoModelMatrixStack matrixStack[animationMaxCount_sceneObject];

	a3_HierarchyState* activeHS = demoMode->hierarchyState_skel + 1, * baseHS = demoMode->hierarchyState_skel;

	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// temp scale mat
	a3mat4 scaleMat = a3mat4_identity;

	a3demo_update_objects(demoState, dt,
		demoMode->object_scene, animationMaxCount_sceneObject, 0, 0);
	a3demo_update_objects(demoState, dt,
		demoMode->object_camera, animationMaxCount_cameraObject, 1, 0);

	a3demo_updateProjectorViewProjectionMat(demoMode->proj_camera_main);

	// apply scales to objects
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
		a3demo_applyScale_internal(demoMode->object_scene + i, scaleMat.m);

	// update skybox
	a3demo_update_bindSkybox(demoMode->obj_camera_main, demoMode->obj_skybox);

	// update matrix stack data
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
	{
		a3demo_updateModelMatrixStack(matrixStack + i,
			activeCamera->projectionMat.m, activeCameraObject->modelMat.m, activeCameraObject->modelMatInv.m,
			demoMode->object_scene[i].modelMat.m, a3mat4_identity.m);
	}

	// ANIMATION
	// interpolate -> between deltas/key poses
	// concatenate -> current delta with base
	// convert     -> base to local space
	// FK          -> local to object space

	// &demoMode->hierarchyPoseGroup_skel->hierarchyPosePool[0] is the base pose
	//a3hierarchyPoseLerp(currentState->sampleHPose, &demoMode->hierarchyPoseGroup_skel->hierarchyPosePool[0], &demoMode->hierarchyPoseGroup_skel->hierarchyPosePool[1], demoMode->hierarchy_skel->numNodes, 0.5f);

	a3_HierarchyState* currentState = demoMode->hierarchyState_skel_base;

	//In order to use concat, we need _a_ spatial pose with no deltas. The identity's three spatial poses are identical, so I picked local at random.
	//for a step interpolation, copying the specified pose is equivalent to interpolating.

	a3_HierarchyPoseGroup* poseGroup = demoMode->hierarchyPoseGroup_skel;
	a3_Hierarchy* hierarchy = demoMode->hierarchy_skel;
	a3ui32 deltaPoses = poseGroup->poseCount - 1;
	a3hierarchyPoseCopy(currentState->sampleHPose, &poseGroup->hierarchyPosePool[0], hierarchy->numNodes);
	a3hierarchyPoseConcat(currentState->localHPose, &poseGroup->hierarchyPosePool[1], &poseGroup->hierarchyPosePool[0], hierarchy->numNodes);
	a3hierarchyPoseConvert(currentState->localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
	a3kinematicsSolveForward(currentState);

	// Update the toggle state. Copy delta pose, concat base with either sample or identity, convert, FK.
	currentState = demoMode->hierarchyState_skel_toggle;
	//sampleHPose stores the current delta pose.
	a3hierarchyPoseCopy(currentState->sampleHPose, &poseGroup->hierarchyPosePool[demoMode->currentToggleIndex + 1], hierarchy->numNodes);
	//concat deltaPose with base pose.
	a3hierarchyPoseConcat(currentState->localHPose, &poseGroup->hierarchyPosePool[0], currentState->sampleHPose, hierarchy->numNodes);
	a3hierarchyPoseConvert(currentState->localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
	a3kinematicsSolveForward(currentState);


	// Update the clip-controlled state. Copy, concat base with either sample or identity, convert, FK.
	currentState = demoMode->hierarchyState_skel_clip;

	demoMode->currentClipKeyVal = (a3ui32)(demoState->controller_skeleton->clipPool->clipArray[demoState->controller_skeleton->clipIndex].keyframes->keyframeArray[demoState->controller_skeleton->keyframeIndex].sample.value);

	a3real lerpParam = demoState->controller_skeleton->keyframeParameter; //For later, if we decide to try a3hierarchyPoseLerp() instead of a3hierarchyPoseCopy(), it should "just work" hahahhaahahahaa
	//clipKeyVal points to a deltapose, but there's an offset in the pool, so +1
	//a3hierarchyPoseCopy(currentState->sampleHPose, &demoMode->hierarchyPoseGroup_skel->hierarchyPosePool[demoMode->currentClipKeyVal + 1], demoMode->hierarchy_skel->numNodes);

	// Currently lerp from zero delta to the new delta by lerpParam
	//a3hierarchyPoseLerp(currentState->sampleHPose, &demoMode->hierarchyPoseGroup_bvh->hierarchyPosePool[demoMode->currentClipKeyVal + 1], &demoMode->hierarchyPoseGroup_bvh->hierarchyPosePool[((demoMode->currentClipKeyVal + 1) % deltaPoses) + 1], demoMode->hierarchy_bvh->numNodes, lerpParam);

	// Determine the previous and next poses for camull-rom
	a3_HierarchyPose* prev = &poseGroup->hierarchyPosePool[((demoMode->currentClipKeyVal + deltaPoses - 1) % deltaPoses) + 1]; //clipKeyVal = 0,1,2,3 => add 1 to get index ||| clip = 2; index = 3; prev = "0" => 2
	a3_HierarchyPose* next = &poseGroup->hierarchyPosePool[((demoMode->currentClipKeyVal + deltaPoses + 2) % deltaPoses) + 1]; //										    ||| clip = 2; index = 3; next = "0" => 1


	a3_HierarchyPose* pose0 = &poseGroup->hierarchyPosePool[((demoMode->currentClipKeyVal + deltaPoses) % deltaPoses) + 1];
	a3_HierarchyPose* pose1 = &poseGroup->hierarchyPosePool[((demoMode->currentClipKeyVal + deltaPoses + 1) % deltaPoses) + 1];
	switch (demoMode->interpFunction)
	{
	case 0: //copy
		a3hierarchyPoseCopy(currentState->sampleHPose, pose0, hierarchy->numNodes);
		break;
	case 2: //nearest
		a3hierarchyPoseNearest(currentState->sampleHPose, pose0, pose1, hierarchy->numNodes, lerpParam);
		break;
	case 3: //catmull
		a3hierarchyPoseCatRom(currentState->sampleHPose, prev, pose0, pose1, next, hierarchy->numNodes, lerpParam);
		break;
	default: //also case 1, lerping.
		a3hierarchyPoseLerp(currentState->sampleHPose, pose0, pose1, hierarchy->numNodes, lerpParam);
		break;
	}

	//concat deltaPose (in samplePose) with base pose
	a3hierarchyPoseConcat(currentState->localHPose, &poseGroup->hierarchyPosePool[0], currentState->sampleHPose, hierarchy->numNodes);
	a3hierarchyPoseConvert(currentState->localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
	a3kinematicsSolveForward(currentState);
	
	//lab 3 materials

	a3_HierarchyState* output = demoMode->hierarchyState_skel_output;
	a3_HierarchyState* control = demoMode->hierarchyState_skel_control;
	switch (demoMode->blendOp)
	{
		case 0: //identity, output = base pose
			a3hierarchyPoseOpIdentity(output->sampleHPose, hierarchy->numNodes);
			break;
		case 1: //constant/copy, output = delta pose 2, index 3; still display delta pose 2 in control[0]
			a3hierarchyPoseOpCopy(output->sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			//process control[0]
			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);
			break;
		case 2: //negate/invert, output = -delta pose 1, index 2; still display delta pose 1 in control[0]
			a3hierarchyPoseOpNegate(output->sampleHPose, &poseGroup->hierarchyPosePool[2], hierarchy->numNodes);
			//process control[0]
			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[2], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);
			break;
		case 3: //concatenate, output = delta pose 1 + delta pose 2; display both in control[0] and control[1]
			a3hierarchyPoseOpConcat(output->sampleHPose, &poseGroup->hierarchyPosePool[2], &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			//process control[0]
			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[2], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);
			//process control[1]
			a3hierarchyPoseOpCopy(control[1].sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[1].localHPose, &poseGroup->hierarchyPosePool[0], control[1].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[1].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[1]);
			break;
		case 4: //nearest, output = nearest(delta pose 2, delta pose 3); display both in control[0] and control[1], display u in text (controllable +/- 0.1, limited at 1.0)
			a3hierarchyPoseOpNearest(output->sampleHPose, &poseGroup->hierarchyPosePool[3], &poseGroup->hierarchyPosePool[4], demoMode->u, hierarchy->numNodes);
			//process control[0]
			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);
			//process control[1]
			a3hierarchyPoseOpCopy(control[1].sampleHPose, &poseGroup->hierarchyPosePool[4], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[1].localHPose, &poseGroup->hierarchyPosePool[0], control[1].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[1].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[1]);
			break;
		case 5: //lerp, output = lerp(delta pose 2, delta pose 3);  display both in control[0] and control[1], display u in text (controllable +/- 0.1, limited at 1.0)
			a3hierarchyPoseOpLERP(output->sampleHPose, &poseGroup->hierarchyPosePool[3], &poseGroup->hierarchyPosePool[4], demoMode->u, hierarchy->numNodes);
			//process control[0]
			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);
			//process control[1]
			a3hierarchyPoseOpCopy(control[1].sampleHPose, &poseGroup->hierarchyPosePool[4], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[1].localHPose, &poseGroup->hierarchyPosePool[0], control[1].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[1].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[1]);
			break;
		case 6: //catmull, output = catmull(1, 2, 3, "0"); display all in control[], u in text
			a3hierarchyPoseOpCubic(output->sampleHPose,
				&poseGroup->hierarchyPosePool[2],
				&poseGroup->hierarchyPosePool[3],
				&poseGroup->hierarchyPosePool[4],
				&poseGroup->hierarchyPosePool[1], hierarchy->numNodes, demoMode->u);

			//process control[0]
			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[2], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);
			//process control[1]
			a3hierarchyPoseOpCopy(control[1].sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[1].localHPose, &poseGroup->hierarchyPosePool[0], control[1].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[1].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[1]);

			//process control[2]
			a3hierarchyPoseOpCopy(control[2].sampleHPose, &poseGroup->hierarchyPosePool[4], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[2].localHPose, &poseGroup->hierarchyPosePool[0], control[2].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[2].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[2]);
			//process control[3]
			a3hierarchyPoseOpCopy(control[3].sampleHPose, &poseGroup->hierarchyPosePool[1], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[3].localHPose, &poseGroup->hierarchyPosePool[0], control[3].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[3].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[3]);
			break;
		case 7: //deconcat, output = 3 - 1; display 3 and 1 in control[0,1]
			a3hierarchyPoseOpDeconcat(output->sampleHPose, &poseGroup->hierarchyPosePool[4], &poseGroup->hierarchyPosePool[2], hierarchy->numNodes);

			//process control[0]
			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[4], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);
			//process control[1]
			a3hierarchyPoseOpCopy(control[1].sampleHPose, &poseGroup->hierarchyPosePool[2], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[1].localHPose, &poseGroup->hierarchyPosePool[0], control[1].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[1].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[1]);
			break;
		case 8: //scale, output = delta pose 2; display 2 in control[0], u in text
			a3hierarchyPoseOpScale(output->sampleHPose, &poseGroup->hierarchyPosePool[3], demoMode->u, hierarchy->numNodes);
			//process control[0]
			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);
			break;
		case 9: //triangular, output = triangle(1,2,3); display in control[0,1,2], u1 and u2 in text, limited to u1+u2=1
			a3hierarchyPoseOpTriangularLERP(output->sampleHPose,
				&poseGroup->hierarchyPosePool[2],
				&poseGroup->hierarchyPosePool[3],
				&poseGroup->hierarchyPosePool[4],
				demoMode->u_1, demoMode->u_2, hierarchy->numNodes);

			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[2], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);

			a3hierarchyPoseOpCopy(control[1].sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[1].localHPose, &poseGroup->hierarchyPosePool[0], control[1].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[1].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[1]);

			a3hierarchyPoseOpCopy(control[2].sampleHPose, &poseGroup->hierarchyPosePool[4], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[2].localHPose, &poseGroup->hierarchyPosePool[0], control[2].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[2].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[2]);
			break;
		case 10: //bi-nearest, output = ("0', 1, 2, 3); display all in controls; u0, u1, u in text
			a3hierarchyPoseOpBiNearest(output->sampleHPose,
				&poseGroup->hierarchyPosePool[1],
				&poseGroup->hierarchyPosePool[2],
				&poseGroup->hierarchyPosePool[3],
				&poseGroup->hierarchyPosePool[4],
				demoMode->u_0, demoMode->u_1, demoMode->u, hierarchy->numNodes);

			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[1], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);

			a3hierarchyPoseOpCopy(control[1].sampleHPose, &poseGroup->hierarchyPosePool[2], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[1].localHPose, &poseGroup->hierarchyPosePool[0], control[1].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[1].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[1]);

			a3hierarchyPoseOpCopy(control[2].sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[2].localHPose, &poseGroup->hierarchyPosePool[0], control[2].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[2].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[2]);

			a3hierarchyPoseOpCopy(control[3].sampleHPose, &poseGroup->hierarchyPosePool[4], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[3].localHPose, &poseGroup->hierarchyPosePool[0], control[3].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[3].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[3]);
			break;
		case 11: //bilinear, output = ("0', 1, 2, 3); display all in controls; u0, u1, u in text
			a3hierarchyPoseOpBiLerp(output->sampleHPose,
				&poseGroup->hierarchyPosePool[1],
				&poseGroup->hierarchyPosePool[2],
				&poseGroup->hierarchyPosePool[3],
				&poseGroup->hierarchyPosePool[4],
				demoMode->u_0, demoMode->u_1, demoMode->u, hierarchy->numNodes);

			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[1], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);

			a3hierarchyPoseOpCopy(control[1].sampleHPose, &poseGroup->hierarchyPosePool[2], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[1].localHPose, &poseGroup->hierarchyPosePool[0], control[1].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[1].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[1]);

			a3hierarchyPoseOpCopy(control[2].sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[2].localHPose, &poseGroup->hierarchyPosePool[0], control[2].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[2].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[2]);

			a3hierarchyPoseOpCopy(control[3].sampleHPose, &poseGroup->hierarchyPosePool[4], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[3].localHPose, &poseGroup->hierarchyPosePool[0], control[3].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[3].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[3]);
			break;
		case 12: //bicubic: output = ((0123),(1230),(2301),(3012)); display all in controls; all 5 u's in text
			a3hierarchyPoseOpBiCubic(output->sampleHPose,
				&poseGroup->hierarchyPosePool[1], &poseGroup->hierarchyPosePool[2], &poseGroup->hierarchyPosePool[3], &poseGroup->hierarchyPosePool[4],
				&poseGroup->hierarchyPosePool[2], &poseGroup->hierarchyPosePool[3], &poseGroup->hierarchyPosePool[4], &poseGroup->hierarchyPosePool[1],
				&poseGroup->hierarchyPosePool[3], &poseGroup->hierarchyPosePool[4], &poseGroup->hierarchyPosePool[1], &poseGroup->hierarchyPosePool[2],
				&poseGroup->hierarchyPosePool[4], &poseGroup->hierarchyPosePool[1], &poseGroup->hierarchyPosePool[2], &poseGroup->hierarchyPosePool[3],
				demoMode->u_n1, demoMode->u_0, demoMode->u_1, demoMode->u_2, demoMode->u, hierarchy->numNodes);

			a3hierarchyPoseOpCopy(control[0].sampleHPose, &poseGroup->hierarchyPosePool[1], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[0].localHPose, &poseGroup->hierarchyPosePool[0], control[0].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[0].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[0]);

			a3hierarchyPoseOpCopy(control[1].sampleHPose, &poseGroup->hierarchyPosePool[2], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[1].localHPose, &poseGroup->hierarchyPosePool[0], control[1].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[1].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[1]);

			a3hierarchyPoseOpCopy(control[2].sampleHPose, &poseGroup->hierarchyPosePool[3], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[2].localHPose, &poseGroup->hierarchyPosePool[0], control[2].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[2].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[2]);

			a3hierarchyPoseOpCopy(control[3].sampleHPose, &poseGroup->hierarchyPosePool[4], hierarchy->numNodes);
			a3hierarchyPoseOpConcat(control[3].localHPose, &poseGroup->hierarchyPosePool[0], control[3].sampleHPose, hierarchy->numNodes);
			a3hierarchyPoseConvert(control[3].localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
			a3kinematicsSolveForward(&control[3]);
			break;
	}
	a3hierarchyPoseOpConcat(output->localHPose, &poseGroup->hierarchyPosePool[0], output->sampleHPose, hierarchy->numNodes);
	a3hierarchyPoseConvert(output->localHPose, hierarchy->numNodes, poseGroup->channels, poseGroup->eulerOrders);
	a3kinematicsSolveForward(output);

/*
	// skeletal
	if (demoState->updateAnimation)
	{
		i = (a3ui32)(demoState->timer_display->totalTime);
		demoMode->hierarchyKeyPose_display[0] = (i + 0) % (demoMode->hierarchyPoseGroup_skel->hposeCount - 1);
		demoMode->hierarchyKeyPose_display[1] = (i + 1) % (demoMode->hierarchyPoseGroup_skel->hposeCount - 1);
		demoMode->hierarchyKeyPose_param = (a3real)(demoState->timer_display->totalTime - (a3f64)i);
	}

	//a3hierarchyPoseCopy(activeHS->objectSpace,
	//	demoMode->hierarchyPoseGroup_skel->hpose + demoMode->hierarchyKeyPose_display[0] + 1,
	//	demoMode->hierarchy_skel->numNodes);
	a3hierarchyPoseLerp(activeHS->objectSpace,	// use as temp storage
		demoMode->hierarchyPoseGroup_skel->hpose + demoMode->hierarchyKeyPose_display[0] + 1,
		demoMode->hierarchyPoseGroup_skel->hpose + demoMode->hierarchyKeyPose_display[1] + 1,
		demoMode->hierarchyKeyPose_param,
		demoMode->hierarchy_skel->numNodes);
	a3hierarchyPoseConcat(activeHS->localSpace,	// goal to calculate
		baseHS->localSpace, // holds base pose
		activeHS->objectSpace, // temp storage
		demoMode->hierarchy_skel->numNodes);
	a3hierarchyPoseConvert(activeHS->localSpace,
		demoMode->hierarchy_skel->numNodes,
		demoMode->hierarchyPoseGroup_skel->channel,
		demoMode->hierarchyPoseGroup_skel->order);
	a3kinematicsSolveForward(activeHS);
	a3hierarchyStateUpdateObjectInverse(activeHS);
	a3hierarchyStateUpdateObjectBindToCurrent(activeHS, baseHS);
*/

	// prepare and upload graphics data
	{
		a3addressdiff const skeletonIndex = demoMode->obj_skeleton - demoMode->object_scene;
		a3ui32 const mvp_size = demoMode->hierarchy_skel->numNodes * sizeof(a3mat4);
		a3ui32 const t_skin_size = sizeof(demoMode->t_skin);
		a3ui32 const dq_skin_size = sizeof(demoMode->dq_skin);
		//a3mat4 const mvp_obj = matrixStack[skeletonIndex].modelViewProjectionMat;
		a3mat4* mvp_joint, * mvp_bone, * t_skin;
		a3dualquat* dq_skin;
		a3index i;
		//a3i32 p;
		
		// update joint and bone transforms
		for (i = 0; i < demoMode->hierarchy_skel->numNodes; ++i)
		{
			mvp_joint = demoMode->mvp_joint + i;
			mvp_bone = demoMode->mvp_bone + i;
			t_skin = demoMode->t_skin + i;
			dq_skin = demoMode->dq_skin + i;
		/*
			// joint transform
			a3real4x4SetScale(scaleMat.m, a3real_quarter);
			a3real4x4Concat(activeHS->objectSpace->pose[i].transform.m, scaleMat.m);
			a3real4x4Product(mvp_joint->m, mvp_obj.m, scaleMat.m);
			
			// bone transform
			p = demoMode->hierarchy_skel->nodes[i].parentIndex;
			if (p >= 0)
			{
				// position is parent joint's position
				scaleMat.v3 = activeHS->objectSpace->pose[p].transform.v3;

				// direction basis is from parent to current
				a3real3Diff(scaleMat.v2.v,
					activeHS->objectSpace->pose[i].transform.v3.v, scaleMat.v3.v);

				// right basis is cross of some upward vector and direction
				// select 'z' for up if either of the other dimensions is set
				a3real3MulS(a3real3CrossUnit(scaleMat.v0.v,
					a3real2LengthSquared(scaleMat.v2.v) > a3real_zero
					? a3vec3_z.v : a3vec3_y.v, scaleMat.v2.v), a3real_quarter);
			
				// up basis is cross of direction and right
				a3real3MulS(a3real3CrossUnit(scaleMat.v1.v,
					scaleMat.v2.v, scaleMat.v0.v), a3real_quarter);
			}
			else
			{
				// if we are a root joint, make bone invisible
				a3real4x4SetScale(scaleMat.m, a3real_zero);
			}
			a3real4x4Product(mvp_bone->m, mvp_obj.m, scaleMat.m);

			// get base to current object-space
			*t_skin = activeHS->objectSpaceBindToCurrent->pose[i].transform;
		*/
			// calculate DQ
			{
				a3real4 d = { a3real_zero };
				a3demo_mat2quat_safe(dq_skin->r.q, t_skin->m);
				a3real3ProductS(d, t_skin->v3.v, a3real_half);
				a3quatProduct(dq_skin->d.q, d, dq_skin->r.q);
			}
		}
		
		// upload
		a3bufferRefill(demoState->ubo_transformMVP, 0, mvp_size, demoMode->mvp_joint);
		a3bufferRefill(demoState->ubo_transformMVPB, 0, mvp_size, demoMode->mvp_bone);
		a3bufferRefill(demoState->ubo_transformBlend, 0, t_skin_size, demoMode->t_skin);
		a3bufferRefillOffset(demoState->ubo_transformBlend, 0, t_skin_size, dq_skin_size, demoMode->dq_skin);
	}
}


//-----------------------------------------------------------------------------
