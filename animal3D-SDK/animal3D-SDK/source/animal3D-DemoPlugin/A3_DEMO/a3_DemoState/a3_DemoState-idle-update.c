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

	a3_DemoState_idle-update.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     UPDATING THE STATE in this file.         ***
	****************************************************
*/

/*
	Animation Framework Addons by Cameron Schneider
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"


//-----------------------------------------------------------------------------
// GENERAL UTILITIES

void a3demo_applyScale_internal(a3_DemoSceneObject* sceneObject, a3real4x4p s)
{
	if (sceneObject->scaleMode)
	{
		if (sceneObject->scaleMode == 1)
		{
			s[0][0] = s[1][1] = s[2][2] = sceneObject->scale.x;
			a3real4x4ConcatL(sceneObject->modelMat.m, s);
			a3real4x4TransformInverseUniformScale(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
		}
		else
		{
			s[0][0] = sceneObject->scale.x;
			s[1][1] = sceneObject->scale.y;
			s[2][2] = sceneObject->scale.z;
			a3real4x4ConcatL(sceneObject->modelMat.m, s);
			a3real4x4TransformInverse(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
		}
	}
	else
		a3real4x4TransformInverseIgnoreScale(sceneObject->modelMatInv.m, sceneObject->modelMat.m);
}


//-----------------------------------------------------------------------------
// UPDATE SUB-ROUTINES

void a3demo_update_objects(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3boolean useZYX, a3boolean applyScale)
{
	a3ui32 i;
	a3mat4 scaleMat = a3mat4_identity;

	if (applyScale)
		for (i = 0; i < count; ++i, ++sceneObjectBase)
		{
			// update transforms
			a3demo_updateSceneObject(sceneObjectBase, useZYX);

			// apply scale
			a3demo_applyScale_internal(sceneObjectBase, scaleMat.m);
		}
	else
		for (i = 0; i < count; ++i, ++sceneObjectBase)
		{
			a3demo_updateSceneObject(sceneObjectBase, useZYX);
		}
}

void a3demo_update_defaultAnimation(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3ui32 axis)
{
	const a3f32 dr = demoState->updateAnimation ? (a3f32)dt * 15.0f : 0.0f;
	a3ui32 i;

	// do simple animation
	for (i = 0, axis %= 3; i < count; ++i, ++sceneObjectBase)
		sceneObjectBase->euler.v[axis] = a3trigValid_sind(sceneObjectBase->euler.v[axis] + dr);
}

void a3demo_update_bindSkybox(a3_DemoSceneObject* obj_camera, a3_DemoSceneObject* obj_skybox)
{
	// model transformations (if needed)
	const a3mat4 convertY2Z = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, +1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};

	// bind skybox to camera
	obj_skybox->position = obj_camera->position;
	obj_skybox->modelMat.v3 = obj_camera->modelMat.v3;

	// correct rotations as needed
	// need to rotate skybox if Z-up
	a3real4x4ConcatL(obj_skybox->modelMat.m, convertY2Z.m);
}

void a3demo_update_pointLight(a3_DemoSceneObject* obj_camera, a3_DemoPointLight* pointLightBase, a3ui32 count)
{
	a3ui32 i;

	// update lights view positions for current camera
	for (i = 0; i < count; ++i, ++pointLightBase)
	{
		// convert to view space and retrieve view position
		a3real4Real4x4Product(pointLightBase->viewPos.v, obj_camera->modelMatInv.m, pointLightBase->worldPos.v);
	}
}


//-----------------------------------------------------------------------------
// UPDATE

/// <summary>
/// Extracts some shared functionality in a3demo_update when handling transitions between clips. Retrieves the sample value of the next clip
/// </summary>
/// <param name="currentClip">The clip we're examining</param>
/// <param name="forward">The play direction</param>
/// <returns></returns>
a3real a3_peek_keyframe_transition_value(a3_Clip* currentClip, a3boolean forward)
{
	a3_ClipTransition* clipTransition = forward ? &currentClip->forwardTransition : &currentClip->reverseTransition;
	a3_Clip* tempClip = &clipTransition->targetClipPool->clipArray[a3clipGetIndexInPool(clipTransition->targetClipPool, clipTransition->targetClipName)];
	a3real retVal = 0;
	a3ui32 pauseIndex = 0;
	switch (clipTransition->transition)
	{
	case a3clipTransitionPause:
		pauseIndex = forward ? currentClip->lastKeyframeIndex : currentClip->firstKeyframeIndex;
		retVal = currentClip->keyframes->keyframeArray[pauseIndex].sample.value;
		break;
	case a3clipTransitionForward:
	case a3clipTransitionForwardPause:
	case a3clipTransitionForwardFrame:
		retVal = (tempClip->keyframes->keyframeArray[tempClip->firstKeyframeIndex]).sample.value;
		break;
	case a3clipTransitionForwardSkip:
		retVal = (tempClip->keyframes->keyframeArray[tempClip->firstKeyframeIndex + 1]).sample.value;
		break;
	case a3clipTransitionReverse:
	case a3clipTransitionReversePause:
	case a3clipTransitionReverseFrame:
		retVal = (tempClip->keyframes->keyframeArray[tempClip->lastKeyframeIndex]).sample.value;
		break;
	case a3clipTransitionReverseSkip:
		retVal = (tempClip->keyframes->keyframeArray[tempClip->lastKeyframeIndex - 1]).sample.value;
	}
	return retVal;
}
/// <param name="dt">delta time, in SECONDS</param>
void a3demo_update(a3_DemoState* demoState, a3f64 const dt)
{
	demoState->demoModeCallbacksPtr->handleUpdate(demoState,
		demoState->demoModeCallbacksPtr->demoMode, dt);

	// Update all clip controllers
	for (int i = 0; i < demoState->controllerCount; i++)
	{
		a3_ClipController* ctrl = &demoState->controllers[i];
		a3clipControllerUpdate(ctrl, (a3real)dt * demoState->globalPlaybackDir * demoState->globalSpeedMod);

		//assign position based on the waypoint
		a3_Clip* clip = ctrl->clipPool->clipArray + ctrl->clipIndex;
		a3real val1 = (clip->keyframes->keyframeArray + ctrl->keyframeIndex)->sample.value;
		a3real val2 = val1; //default case for pausing. Lerping ceases to work when paused because it doesn't know the direction it _was_ going.

		if (ctrl->playbackDir == 1) //forward
		{
			//still within clip
			if (ctrl->keyframeIndex + 1 <= clip->lastKeyframeIndex)
			{
				val2 = (clip->keyframes->keyframeArray + ctrl->keyframeIndex + 1)->sample.value;
			}
			else
			{
				//transitioning, checking next clip forwards
				val2 = a3_peek_keyframe_transition_value(clip, true);
			}
			demoState->demoMode0_starter->object_scene[i + 2].position = a3vecLerp(demoState->waypoints[(int)val1], demoState->waypoints[(int)val2], ctrl->keyframeParameter);
		}
		else if (ctrl->playbackDir == -1) //reverse
		{
			//still within clip
			if (ctrl->keyframeIndex - 1 >= clip->firstKeyframeIndex)
			{
				val2 = (clip->keyframes->keyframeArray + ctrl->keyframeIndex - 1)->sample.value;
			}
			else
			{
				//transitioning, checking next clip in reverse
				val2 = a3_peek_keyframe_transition_value(clip, false);
			}
			demoState->demoMode0_starter->object_scene[i + 2].position = a3vecLerp(demoState->waypoints[(int)val1], demoState->waypoints[(int)val2], 1.0f - ctrl->keyframeParameter);
		}
		else
		{
			demoState->demoMode0_starter->object_scene[i + 2].position = demoState->waypoints[(int)val1];
		}
	}
}


//-----------------------------------------------------------------------------
