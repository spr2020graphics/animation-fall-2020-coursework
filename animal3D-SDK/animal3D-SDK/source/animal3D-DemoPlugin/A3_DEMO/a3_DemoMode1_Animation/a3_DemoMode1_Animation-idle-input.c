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

	a3_DemoMode1_Animation-idle-input.c
	Demo mode implementations: animation scene.

	********************************************
	*** INPUT FOR ANIMATION SCENE MODE       ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_Animation.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoMacros.h"


//-----------------------------------------------------------------------------
// CALLBACKS

// main demo mode callback
void a3animation_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state)
{
	switch (asciiKey)
	{
		// toggle render program
		//a3demoCtrlCasesLoop(demoMode->render, animation_render_max, 'k', 'j');

		// toggle display program
		//a3demoCtrlCasesLoop(demoMode->display, animation_display_max, 'K', 'J');

		// toggle active camera
		//a3demoCtrlCasesLoop(demoMode->activeCamera, animation_camera_max, 'v', 'c');

		// toggle pipeline mode
		a3demoCtrlCasesLoop(demoMode->pipeline, animation_pipeline_max, ']', '[');

		// toggle target
		a3demoCtrlCasesLoop(demoMode->targetIndex[demoMode->pass], demoMode->targetCount[demoMode->pass], '}', '{');

		// toggle pass to display
		a3demoCtrlCasesLoop(demoMode->pass, animation_pass_max, ')', '(');

		// toggle control target
		a3demoCtrlCasesLoop(demoMode->ctrl_target, animation_ctrlmode_max, '\'', ';');

		// toggle position input mode
		a3demoCtrlCasesLoop(demoMode->ctrl_position, animation_inputmode_max, '=', '-');

		// toggle rotation input mode
		a3demoCtrlCasesLoop(demoMode->ctrl_rotation, animation_inputmode_max, '+', '_');
	}
}

void a3animation_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode1_Animation* demoMode, a3i32 const asciiKey, a3i32 const state)
{
//	switch (asciiKey)
//	{
//
//	}
}


//-----------------------------------------------------------------------------

void a3demo_input_controlProjector(
	a3_DemoState* demoState, a3_DemoProjector* projector,
	a3f64 const dt, a3real ctrlMoveSpeed, a3real ctrlRotateSpeed, a3real ctrlZoomSpeed);

void a3demo_input_controlObject(
	a3_DemoState* demoState, a3_DemoSceneObject* object,
	a3f64 const dt, a3real ctrlMoveSpeed, a3real ctrlRotateSpeed);

/// <summary>
/// We input a velocity and it moves around
/// </summary>
/// <param name="vec_out"></param>
/// <param name="x"></param>
/// <param name="dx_dt"></param>
/// <param name="dt"></param>
/// <returns></returns>
a3real3r a3EulerIntegration(a3real3 x_out, a3real3 x, a3real3 v, const a3real dt)
{
	a3vec3 temp;

	a3real3ProductS(temp.v, v, dt);
	a3real3Sum(x_out, x, temp.v);

	return x_out;
}

/// <summary>
/// We pass in a position and we linearly interpolate to it, as though a velocity was being imparted
/// </summary>
/// <param name="vec_out"></param>
/// <param name="x"></param>
/// <param name="x_target"></param>
/// <param name="u"></param>
/// <returns></returns>
a3real3r a3EulerInterp(a3real3 x_out, a3real3 x, a3real3 x_target, const a3real u)
{
	a3real3Lerp(x_out, x, x_target, u);

	return x_out;
}

/// <summary>
/// We input an acceleration and output the new position and velocity.
/// </summary>
/// <param name="vec_out"></param>
/// <param name="x"></param>
/// <param name="v"></param>
/// <param name="a"></param>
/// <param name="dt"></param>
/// <returns></returns>
a3real3r a3KinematicIntegration(a3real3 x_out, a3real3 v_out, a3real3 x, a3real3 v, a3real3 a, const a3real dt)
{
	a3vec3 vdt, adt2;
	a3real3ProductS(vdt.v, v, dt); //vt
	a3real3ProductS(adt2.v, a, dt * dt * 0.5f); //1/2at^2
	a3vec3 vdtPlusAdt2;
	a3real3Sum(vdtPlusAdt2.v, vdt.v, adt2.v);
	a3real3Sum(x_out, x, vdtPlusAdt2.v); //add tempSum (vt + 1/2at^2) to x
	a3vec3 adt; //calculate new velocity
	a3real3ProductS(adt.v, a, dt); //secondary output for velocity
	a3real3Sum(v_out, v, adt.v);
	return x_out;
}

/// <summary>
/// We input a velocity
/// </summary>
/// <param name="v0"></param>
/// <param name="v1"></param>
/// <param name="u"></param>
/// <returns></returns>
a3real3r a3KinematicInterp(a3real3 x_out, a3real3 v_out, a3real3 x, a3real3 v0, a3real3 v1, const a3real u, const a3real dt)
{
	a3real3Lerp(v_out, v0, v1, u);
	a3vec3 temp;
	a3real3ProductS(temp.v, v_out, dt); //the vdt part of x + vdt
	a3real3Sum(x_out, x, temp.v); //output old X + vdt into new x
	return x_out;
}


void a3animation_input(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt)
{
	a3_DemoProjector* projector = demoMode->projector + demoMode->activeCamera;

	// right click to ray pick
	if (a3mouseGetState(demoState->mouse, a3mouse_right) == a3input_down)
	{
		// get window coordinates
		a3i32 const x = a3mouseGetX(demoState->mouse) + demoState->frameBorder;
		a3i32 const y = a3mouseGetY(demoState->mouse) + demoState->frameBorder;

		// transform to NDC
		a3vec4 coord = a3vec4_one;
		coord.x = +((a3real)x * demoState->frameWidthInv * a3real_two - a3real_one);
		coord.y = -((a3real)y * demoState->frameHeightInv * a3real_two - a3real_one);
		coord.z = -a3real_one;

		// transform to view space
		a3real4Real4x4Mul(projector->projectionMatInv.m, coord.v);
		a3real4DivS(coord.v, coord.w);

		// transform to world space
		a3real4Real4x4Mul(projector->sceneObject->modelMat.m, coord.v);
	}
	
	// choose control target
	switch (demoMode->ctrl_target)
	{
	case animation_ctrl_camera:
		// move camera
		a3demo_input_controlProjector(demoState, projector,
			dt, projector->ctrlMoveSpeed, projector->ctrlRotateSpeed, projector->ctrlZoomSpeed);
		break;
	case animation_ctrl_character:
		// capture axes
		if (a3XboxControlIsConnected(demoState->xcontrol))
		{
			//a3demo_input_controlObject(demoState, demoState->demoMode1_animation->obj_skeleton_ctrl,
			//	dt, projector->ctrlMoveSpeed, projector->ctrlRotateSpeed);
			// ****TO-DO:

			// get directly from joysticks, put into left/right axis variables
			a3XboxControlGetJoysticks(demoState->xcontrol, demoMode->axis_l, demoMode->axis_r);
			demoMode->branchTrigger = (a3f32)demoState->xcontrol->ctrl.lTrigger;
		}
		else
		{
			// ****TO-DO:
			// calculate normalized vectors given keyboard state
			a3i32 posUpDown = (demoState->keyboard->key.key[a3key_W] != 0) - (demoState->keyboard->key.key[a3key_S] != 0);
			a3i32 posLeftRight = (demoState->keyboard->key.key[a3key_D] != 0) - (demoState->keyboard->key.key[a3key_A] != 0);

			a3vec2 tempPos = { (a3real)posLeftRight, (a3real)posUpDown };
			if (tempPos.x != 0 || tempPos.y != 0)
			{
				a3real2Normalize(tempPos.v);
			}
			// set axis data
			demoMode->axis_l[0] = tempPos.x;
			demoMode->axis_l[1] = tempPos.y;

			// Use J and L to get rotation input
			a3i32 rotLeftRight = (demoState->keyboard->key.key[a3key_L] != 0) - (demoState->keyboard->key.key[a3key_J] != 0);
			demoMode->axis_r[0] = (a3real)rotLeftRight;

			// Used to trigger the branching transition for lab 4
			demoMode->branchTrigger = (a3f32)(demoState->keyboard->key.key[a3key_P] != 0 && demoState->keyboard->key.key[a3key_shift] == 0);
		}
		demoMode->mag_l = a3sqrt((a3f32)((demoMode->axis_l[0] * demoMode->axis_l[0]) + (demoMode->axis_l[1] * demoMode->axis_l[1])));
		demoMode->mag_r = a3sqrt((a3f32)((demoMode->axis_r[0] * demoMode->axis_r[0]) + (demoMode->axis_r[1] * demoMode->axis_r[1])));
		break;
	}

	// allow the controller, if connected, to change control targets
	if (a3XboxControlIsConnected(demoState->xcontrol))
	{
		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_DPAD_right))
			a3demoCtrlIncLoop(demoMode->ctrl_target, animation_ctrlmode_max);
		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_DPAD_left))
			a3demoCtrlDecLoop(demoMode->ctrl_target, animation_ctrlmode_max);

		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_B))
			a3demoCtrlIncLoop(demoMode->ctrl_position, animation_inputmode_max);
		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_X))
			a3demoCtrlDecLoop(demoMode->ctrl_position, animation_inputmode_max);

		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_Y))
			a3demoCtrlIncLoop(demoMode->ctrl_rotation, animation_inputmode_max);
		if (a3XboxControlIsPressed(demoState->xcontrol, a3xbox_A))
			a3demoCtrlDecLoop(demoMode->ctrl_rotation, animation_inputmode_max);
	}
}


//-----------------------------------------------------------------------------
