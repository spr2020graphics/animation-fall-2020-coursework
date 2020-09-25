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

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_Animation.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"
#include "../_a3_demo_utilities/a3_DemoMacros.h"


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

	//In order to use concat, we need _a_ spatial pose with no deltas. The object pose is always generated from the local pose, so it's safe to reset it at any point
	//to get a zero-delta pose. This is then concatenated with the base skeleton and loaded into the local pose, after which object pose is regenerated.
	//the object pose is presumably invalid before this _anyway_ because we will eventually be animating, so no important data is lost.
	a3spatialPoseReset(currentState->objectHPose->spatialPose);
	a3hierarchyPoseCopy(currentState->sampleHPose, &demoMode->hierarchyPoseGroup_skel->hierarchyPosePool[0], demoMode->hierarchy_skel->numNodes);
	a3hierarchyPoseConcat(currentState->localHPose, currentState->objectHPose, &demoMode->hierarchyPoseGroup_skel->hierarchyPosePool[0], demoMode->hierarchy_skel->numNodes);
	a3hierarchyPoseConvert(currentState->localHPose, demoMode->hierarchy_skel->numNodes, demoMode->hierarchyPoseGroup_skel->channels, demoMode->hierarchyPoseGroup_skel->eulerOrder);
	a3kinematicsSolveForward(currentState);

	currentState = demoMode->hierarchyState_skel_toggle;
	a3hierarchyPoseCopy(currentState->sampleHPose, &demoMode->hierarchyPoseGroup_skel->hierarchyPosePool[demoMode->currentToggleIndex], demoMode->hierarchy_skel->numNodes);
	if (demoMode->currentToggleIndex != 0)
	{
		a3hierarchyPoseConcat(currentState->localHPose, &demoMode->hierarchyPoseGroup_skel->hierarchyPosePool[0], currentState->sampleHPose, demoMode->hierarchy_skel->numNodes);
	}
	else
	{
		//nothing to copy, current index is 0.
		a3hierarchyPoseCopy(currentState->localHPose, currentState->sampleHPose, demoMode->hierarchy_skel->numNodes);
	}
	a3hierarchyPoseConvert(currentState->localHPose, demoMode->hierarchy_skel->numNodes, demoMode->hierarchyPoseGroup_skel->channels, demoMode->hierarchyPoseGroup_skel->eulerOrder);
	a3kinematicsSolveForward(currentState);

	currentState = demoMode->hierarchyState_skel_clip;

	demoMode->currentClipKeyVal = (a3ui32)(demoState->controller_skeleton->clipPool->clipArray[demoState->controller_skeleton->clipIndex].keyframes->keyframeArray[demoState->controller_skeleton->keyframeIndex].sample.value);
	a3real lerpParam = demoState->controller_skeleton->keyframeParameter; //For later, if we decide to try a3hierarchyPoseLerp() instead of a3hierarchyPoseCopy(), it should "just work" hahahhaahahahaa

	a3hierarchyPoseCopy(currentState->sampleHPose, &demoMode->hierarchyPoseGroup_skel->hierarchyPosePool[demoMode->currentClipKeyVal], demoMode->hierarchy_skel->numNodes);
	if (demoMode->currentClipKeyVal != 0)
	{
		a3hierarchyPoseConcat(currentState->localHPose, &demoMode->hierarchyPoseGroup_skel->hierarchyPosePool[0], currentState->sampleHPose, demoMode->hierarchy_skel->numNodes);
	}
	else
	{
		//nothing to copy, current index is 0.
		a3hierarchyPoseCopy(currentState->localHPose, currentState->sampleHPose, demoMode->hierarchy_skel->numNodes);
	}
	a3hierarchyPoseConvert(currentState->localHPose, demoMode->hierarchy_skel->numNodes, demoMode->hierarchyPoseGroup_skel->channels, demoMode->hierarchyPoseGroup_skel->eulerOrder);
	a3kinematicsSolveForward(currentState);
}


//-----------------------------------------------------------------------------
