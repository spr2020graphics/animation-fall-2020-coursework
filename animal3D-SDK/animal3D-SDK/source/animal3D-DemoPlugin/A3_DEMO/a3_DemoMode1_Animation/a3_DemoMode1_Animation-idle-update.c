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

	a3real4SetReal4(q, a3vec4_w.v);

	// done
	return q;
}

inline a3real4x2r a3demo_mat2dquat_safe(a3real4x2 Q, a3real4x4 const m)
{
	// ****TO-DO: 
	//	-> convert matrix to dual quaternion
	//	-> NOTE: this is for testing dual quaternion skinning only; 
	//		quaternion data would normally be computed with poses

	a3demo_mat2quat_safe(Q[0], m);
	a3real4SetReal4(Q[1], a3vec4_zero.v);

	// done
	return Q;
}


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update_objects(a3f64 const dt, a3_DemoSceneObject* sceneObjectBase,
	a3ui32 count, a3boolean useZYX, a3boolean applyScale);
void a3demo_update_defaultAnimation(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3ui32 axis);
void a3demo_update_bindSkybox(a3_DemoSceneObject* obj_camera, a3_DemoSceneObject* obj_skybox);
void a3demo_update_pointLight(a3_DemoSceneObject* obj_camera, a3_DemoPointLight* pointLightBase, a3ui32 count);

void a3demo_applyScale_internal(a3_DemoSceneObject* sceneObject, a3real4x4p s);

a3real3r a3KinematicIntegration(a3real3 x_out, a3real3 v_out, a3real3 x, a3real3 v, a3real3 a, const a3real dt);
a3real3r a3EulerIntegration(a3real3 x_out, a3real3 x, a3real3 v, const a3real dt);

void a3animation_update_graphics(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode,
	a3_DemoModelMatrixStack const* matrixStack, a3_HierarchyState const* activeHS)
{
	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// temp scale mat
	a3mat4 scaleMat = a3mat4_identity;
	a3addressdiff const skeletonIndex = demoMode->obj_skeleton - demoMode->object_scene;
	a3ui32 const mvp_size = demoMode->hierarchy_skel->numNodes * sizeof(a3mat4);
	a3ui32 const t_skin_size = sizeof(demoMode->t_skin);
	a3ui32 const dq_skin_size = sizeof(demoMode->dq_skin);
	a3mat4 const mvp_obj = matrixStack[skeletonIndex].modelViewProjectionMat;
	a3mat4* mvp_joint, * mvp_bone, * t_skin;
	a3dualquat* dq_skin;
	a3index i;
	a3i32 p;

	// update joint and bone transforms
	for (i = 0; i < demoMode->hierarchy_skel->numNodes; ++i)
	{
		mvp_joint = demoMode->mvp_joint + i;
		mvp_bone = demoMode->mvp_bone + i;
		t_skin = demoMode->t_skin + i;
		dq_skin = demoMode->dq_skin + i;

		// joint transform
		a3real4x4SetScale(scaleMat.m, a3real_sixth);
		a3real4x4Concat(activeHS->objectSpace->pose[i].transformMat.m, scaleMat.m);
		a3real4x4Product(mvp_joint->m, mvp_obj.m, scaleMat.m);

		// bone transform
		p = demoMode->hierarchy_skel->nodes[i].parentIndex;
		if (p >= 0)
		{
			// position is parent joint's position
			scaleMat.v3 = activeHS->objectSpace->pose[p].transformMat.v3;

			// direction basis is from parent to current
			a3real3Diff(scaleMat.v2.v,
				activeHS->objectSpace->pose[i].transformMat.v3.v, scaleMat.v3.v);

			// right basis is cross of some upward vector and direction
			// select 'z' for up if either of the other dimensions is set
			a3real3MulS(a3real3CrossUnit(scaleMat.v0.v,
				a3real2LengthSquared(scaleMat.v2.v) > a3real_zero
				? a3vec3_z.v : a3vec3_y.v, scaleMat.v2.v), a3real_sixth);

			// up basis is cross of direction and right
			a3real3MulS(a3real3CrossUnit(scaleMat.v1.v,
				scaleMat.v2.v, scaleMat.v0.v), a3real_sixth);
		}
		else
		{
			// if we are a root joint, make bone invisible
			a3real4x4SetScale(scaleMat.m, a3real_zero);
		}
		a3real4x4Product(mvp_bone->m, mvp_obj.m, scaleMat.m);

		// get base to current object-space
		*t_skin = activeHS->objectSpaceBindToCurrent->pose[i].transformMat;

		// calculate DQ
		a3demo_mat2dquat_safe(dq_skin->Q, t_skin->m);
	}

	// upload
	a3bufferRefill(demoState->ubo_transformMVP, 0, mvp_size, demoMode->mvp_joint);
	a3bufferRefill(demoState->ubo_transformMVPB, 0, mvp_size, demoMode->mvp_bone);
	a3bufferRefill(demoState->ubo_transformBlend, 0, t_skin_size, demoMode->t_skin);
	a3bufferRefillOffset(demoState->ubo_transformBlend, 0, t_skin_size, dq_skin_size, demoMode->dq_skin);
}

void a3animation_update_fk(a3_HierarchyState* activeHS,
	a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup)
{
	if (activeHS->hierarchy == baseHS->hierarchy &&
		activeHS->hierarchy == poseGroup->hierarchy)
	{
		// FK pipeline
		a3hierarchyPoseConcat(activeHS->localSpace,	// local: goal to calculate
			activeHS->animPose, // holds current sample pose
			baseHS->localSpace, // holds base pose (animPose is all identity poses)
			activeHS->hierarchy->numNodes);
		a3hierarchyPoseConvert(activeHS->localSpace,
			activeHS->hierarchy->numNodes,
			poseGroup->channel,
			poseGroup->order);
		a3kinematicsSolveForward(activeHS);
	}
}

//Reverse the FK pipeline shown above
void a3animation_update_ik(a3_HierarchyState* activeHS,
	a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup)
{
	if (activeHS->hierarchy == baseHS->hierarchy &&
		activeHS->hierarchy == poseGroup->hierarchy)
	{
		a3kinematicsSolveInverse(activeHS);
		a3hierarchyPoseOpRevert(activeHS->localSpace, activeHS->hierarchy->numNodes);
		a3hierarchyPoseDeconcat(activeHS->animPose, activeHS->localSpace, baseHS->localSpace, activeHS->hierarchy->numNodes);

	}
}

void a3_animation_updateIK_single(a3_HierarchyState* activeHS, const a3_HierarchyState* baseHS, a3ui32 jointIndex, a3mat4* jointTransform)
{
	if (activeHS->hierarchy == baseHS->hierarchy)
	{
		a3real4x4TransformInverseIgnoreScale(activeHS->objectSpaceInv->pose[jointIndex].transformMat.m, jointTransform->m);

		a3kinematicsSolveInverseSingle(activeHS, jointIndex, activeHS->hierarchy->nodes[jointIndex].parentIndex);
		a3spatialPoseOpRevert(activeHS->localSpace->pose + jointIndex);
		a3spatialPoseDeconcat(activeHS->animPose->pose + jointIndex, activeHS->localSpace->pose + jointIndex, baseHS->localSpace->pose + jointIndex);
	}
}

void a3animation_update_skin(a3_HierarchyState* activeHS,
	a3_HierarchyState const* baseHS)
{
	if (activeHS->hierarchy == baseHS->hierarchy)
	{
		// FK pipeline extended for skinning and other applications
		a3hierarchyStateUpdateLocalInverse(activeHS);
		a3hierarchyStateUpdateObjectInverse(activeHS);
		a3hierarchyStateUpdateObjectBindToCurrent(activeHS, baseHS);
	}
}

void updateFrontIK(a3_DemoMode1_Animation* demoMode, a3_HierarchyState* activeHS, a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup,
	a3real4 ctrlLoc_eff, a3real4 ctrlLoc_con, a3real4 ctrlLoc_base, a3mat4* pTrans_paw, a3mat4* pTrans_elbow, a3mat4* pTrans_base,
	a3ui32 j_paw, a3ui32 j_elbow, a3ui32 j_base)
{
	a3mat4 jTrans_base = *pTrans_base;
	a3mat4 jTrans_elbow = *pTrans_elbow;
	a3mat4 jTrans_paw = *pTrans_paw;
	a3vec3 shoulderToEffector = a3vec3_zero; //also known as DVec
	a3real3Diff(shoulderToEffector.v, ctrlLoc_eff, jTrans_base.v3.v);
	a3vec3 shoulderToElbow = a3vec3_zero;
	a3real3Diff(shoulderToElbow.v, baseHS->objectSpace->pose[j_elbow].transformMat.v3.v, baseHS->objectSpace->pose[j_base].transformMat.v3.v);
	a3vec3 elbowToWrist = a3vec3_zero;
	a3real3Diff(elbowToWrist.v, baseHS->objectSpace->pose[j_paw].transformMat.v3.v, baseHS->objectSpace->pose[j_elbow].transformMat.v3.v);

	//cache lengths between various joints
	a3real shoulderElbowLen = a3real3Length(shoulderToElbow.v); //not squared because I think there's a chance of some distances failing this check when they shouldn't
	a3real elbowWristLen = a3real3Length(elbowToWrist.v);
	a3real shoulderToWristLen = a3sqrt(demoMode->character->frontLegMaxLengthSq);
	a3real shoulderEffectorLen = a3real3Length(shoulderToEffector.v);

	//printf("S-El: %f %f %f (%f)\n", shoulderToElbow.x, shoulderToElbow.y, shoulderToElbow.z, shoulderElbowLen);
	//printf("El-W: %f %f %f (%f)\n", elbowToWrist.x, elbowToWrist.y, elbowToWrist.z, elbowWristLen);
	//printf("S-Ef: %f %f %f (%f)\n", shoulderToEffector.x, shoulderToEffector.y, shoulderToEffector.z, shoulderEffectorLen);

	if (shoulderEffectorLen < shoulderToWristLen) //no solution exists, straighten arm.
	{
		//find the length from shoulder to constraint
		a3vec3 shoulderToConstraint = a3vec3_zero;
		a3real3Diff(shoulderToConstraint.v, ctrlLoc_con, jTrans_base.v3.v);

		a3vec3 nVecNormal = a3vec3_zero; //the non-normalized Normal Vector to the plane
		a3real3CrossUnit(nVecNormal.v, shoulderToEffector.v, shoulderToConstraint.v);

		//unit vector from shoulder to effector
		a3vec3 dNormal = a3vec3_zero;
		a3real3QuotientS(dNormal.v, shoulderToEffector.v, shoulderEffectorLen);

		//find vector perpendicular to normal and d vec
		a3vec3 hVecNormal = a3vec3_zero;
		a3real3CrossUnit(hVecNormal.v, nVecNormal.v, dNormal.v);

		//heron's formula
		a3real L1 = shoulderElbowLen;
		a3real L2 = elbowWristLen;
		a3real heronS = 0.5f * (shoulderEffectorLen + L1 + L2);
		a3real heronASq = heronS * (heronS - shoulderEffectorLen) * (heronS - L1) * (heronS - L2);
		a3real heronA = a3sqrt(heronASq);
		a3real heronH = 2 * heronA / shoulderEffectorLen;
		a3vec3 hVec = a3vec3_zero;
		a3real3ProductS(hVec.v, hVecNormal.v, heronH);

		//pythagorean theorem
		a3real L1Sq = L1 * L1;
		a3real D = a3sqrt(L1Sq - (heronH * heronH));

		a3vec3 elbowDVec = shoulderToEffector;
		a3real3Normalize(elbowDVec.v);
		a3real3MulS(elbowDVec.v, D);


		//calculate new elbow position
		a3vec3 newElbowPos = jTrans_base.v3.xyz;
		a3real3Add(newElbowPos.v, elbowDVec.v);
		a3real3Add(newElbowPos.v, hVec.v);

		//assign wrist and elbow positions. Wrist assignment doesn't work for unknown reason. We think they're in the same coord space? Manual variable inspection indicates it.
		jTrans_elbow.v3.xyz = newElbowPos;
		jTrans_paw.v3.x = ctrlLoc_eff[0];
		jTrans_paw.v3.y = ctrlLoc_eff[1];
		jTrans_paw.v3.z = ctrlLoc_eff[2];

		//Set rotations (shoulder first, then elbow)
		a3real3Diff(shoulderToElbow.v, jTrans_elbow.v3.xyz.v, jTrans_base.v3.xyz.v);
		jTrans_base.v0.xyz = shoulderToElbow;

		a3real3Normalize(jTrans_base.v0.xyz.v);
		a3real3Negate(jTrans_base.v0.xyz.v);

		jTrans_base.v1.xyz = nVecNormal;
		a3real3Cross(jTrans_base.v2.xyz.v, jTrans_base.v0.xyz.v, jTrans_base.v1.xyz.v);

		a3real3Diff(elbowToWrist.v, jTrans_paw.v3.xyz.v, jTrans_elbow.v3.v);
		jTrans_elbow.v0.xyz = elbowToWrist;

		a3real3Normalize(jTrans_elbow.v0.xyz.v);
		a3real3Negate(jTrans_elbow.v0.xyz.v);

		jTrans_elbow.v1.xyz = nVecNormal;
		a3real3Cross(jTrans_elbow.v2.xyz.v, jTrans_elbow.v0.xyz.v, jTrans_elbow.v1.xyz.v);
	}
	//
	// ****TO-DO:
	// reassign resolved transforms to OBJECT-SPACE matrices
	// work from root to leaf too get correct transformations

	activeHS->objectSpace->pose[j_base].transformMat = jTrans_base;
	activeHS->objectSpace->pose[j_elbow].transformMat = jTrans_elbow;
	activeHS->objectSpace->pose[j_paw].transformMat = jTrans_paw;

	a3_animation_updateIK_single(activeHS, baseHS, j_base, &jTrans_base);
	a3_animation_updateIK_single(activeHS, baseHS, j_elbow, &jTrans_elbow);
	a3_animation_updateIK_single(activeHS, baseHS, j_paw, &jTrans_paw);
}

void updateBackIK(a3_DemoMode1_Animation* demoMode, a3_HierarchyState* activeHS, a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup,
	a3real4 ctrlLoc_eff, a3real4 ctrlLoc_con, a3real4 ctrlLoc_base, a3mat4* pTrans_toe, a3mat4* pTrans_paw, a3mat4* pTrans_elbow, a3mat4* pTrans_base,
	a3ui32 j_toe, a3ui32 j_paw, a3ui32 j_elbow, a3ui32 j_base)
{
	a3mat4 jTrans_base = *pTrans_base;
	a3mat4 jTrans_elbow = *pTrans_elbow;
	a3mat4 jTrans_paw = *pTrans_paw;
	a3mat4 jTrans_toe = *pTrans_toe;

	a3vec3 pawToToe;
	a3real3Diff(pawToToe.v, jTrans_paw.v3.xyz.v, jTrans_toe.v3.xyz.v);

	// solve ankle and toe independently, assigning toe location directly
	jTrans_toe.v3.xyz.x = ctrlLoc_eff[0];
	jTrans_toe.v3.xyz.y = ctrlLoc_eff[1];
	jTrans_toe.v3.xyz.z = ctrlLoc_eff[2];

	a3real3Sum(jTrans_paw.v3.xyz.v, jTrans_toe.v3.xyz.v, pawToToe.v);

	// use updateFrontIK using base, constraint joint, ankle as effector
	updateFrontIK(demoMode, activeHS, baseHS, poseGroup,
		jTrans_paw.v3.v, ctrlLoc_con, ctrlLoc_base, &jTrans_paw, &jTrans_elbow, &jTrans_base,
		j_paw, j_elbow, j_base);

	activeHS->objectSpace->pose[j_base].transformMat = jTrans_base;
	activeHS->objectSpace->pose[j_elbow].transformMat = jTrans_elbow;
	activeHS->objectSpace->pose[j_paw].transformMat = jTrans_paw;
	activeHS->objectSpace->pose[j_toe].transformMat = jTrans_toe;

	//a3_animation_updateIK_single(activeHS, baseHS, j_base, &jTrans_base);
	//a3_animation_updateIK_single(activeHS, baseHS, j_elbow, &jTrans_elbow);
	//a3_animation_updateIK_single(activeHS, baseHS, j_paw, &jTrans_paw);
	a3_animation_updateIK_single(activeHS, baseHS, j_toe, &jTrans_toe);
}

void a3animation_update_applyEffectors(a3_DemoMode1_Animation* demoMode,
	a3_HierarchyState* activeHS, a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup)
{
	if (activeHS->hierarchy == baseHS->hierarchy &&
		activeHS->hierarchy == poseGroup->hierarchy)
	{
		a3_DemoSceneObject* sceneObject = demoMode->obj_skeleton;
		a3ui32 j = sceneObject->sceneGraphIndex;

		// need to properly transform joints to their parent frame and vice-versa
		a3mat4 const controlToSkeleton = demoMode->sceneGraphState->localSpaceInv->pose[j].transformMat;

		a3vec4 controlLocator_eff_FL, controlLocator_eff_FR, controlLocator_eff_BL, controlLocator_eff_BR;
		a3vec4 controlLocator_con_FL, controlLocator_con_FR, controlLocator_con_BL, controlLocator_con_BR;
		a3vec4 controlLocator_base_FL, controlLocator_base_FR, controlLocator_base_BL, controlLocator_base_BR;

		a3mat4 jTrans_paw_FL = a3mat4_identity, jTrans_paw_FR = a3mat4_identity, jTrans_toe_BL = a3mat4_identity, jTrans_toe_BR = a3mat4_identity, jTrans_paw_BL = a3mat4_identity, jTrans_paw_BR = a3mat4_identity,
			jTrans_elbow_FL = a3mat4_identity, jTrans_elbow_FR = a3mat4_identity, jTrans_elbow_BL = a3mat4_identity, jTrans_elbow_BR = a3mat4_identity,
			jTrans_base_FL = a3mat4_identity, jTrans_base_FR = a3mat4_identity, jTrans_base_BL = a3mat4_identity, jTrans_base_BR = a3mat4_identity;

		a3ui32 j_paw_FL, j_paw_FR, j_toe_BL, j_toe_BR, j_paw_BL, j_paw_BR;
		a3ui32 j_elbow_FL, j_elbow_FR, j_elbow_BL, j_elbow_BR;
		a3ui32 j_base_FL, j_base_FR, j_base_BL, j_base_BR;

		/*
		WOLF SOLVER NOTES

		2 separate solvers, one for the "upper leg" chain, one for JUST the ankle node

		"end effector" of upper leg is driven by base effector of lower leg

		calculate foot + ankle first, toe to ankle, using actual effector to position toe
		calculate leg chain second, essentially using the ankle as the effector

		ALTERNATE SYSTEM (CRAFTY TIME)

		"reverse foot" solver

		*/

		// FL effector
		sceneObject = demoMode->obj_wolf_effector_FL;
		a3real4Real4x4Product(controlLocator_eff_FL.v, controlToSkeleton.m,
			demoMode->sceneGraphState->localSpace->pose[sceneObject->sceneGraphIndex].transformMat.v3.v);
		j = j_paw_FL = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Vorderpfote_L");
		jTrans_paw_FL = activeHS->objectSpace->pose[j].transformMat;

		// FR effector
		sceneObject = demoMode->obj_wolf_effector_FR;
		a3real4Real4x4Product(controlLocator_eff_FR.v, controlToSkeleton.m,
			demoMode->sceneGraphState->localSpace->pose[sceneObject->sceneGraphIndex].transformMat.v3.v);
		j = j_paw_FR = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Vorderpfote_R");
		jTrans_paw_FR = activeHS->objectSpace->pose[j].transformMat;

		// BL effector
		sceneObject = demoMode->obj_wolf_effector_BL;
		a3real4Real4x4Product(controlLocator_eff_BL.v, controlToSkeleton.m,
			demoMode->sceneGraphState->localSpace->pose[sceneObject->sceneGraphIndex].transformMat.v3.v);
		j = j_toe_BL = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Pfote2_L");
		jTrans_toe_BL = activeHS->objectSpace->pose[j].transformMat;

		// BR effector
		sceneObject = demoMode->obj_wolf_effector_BR;
		a3real4Real4x4Product(controlLocator_eff_BR.v, controlToSkeleton.m,
			demoMode->sceneGraphState->localSpace->pose[sceneObject->sceneGraphIndex].transformMat.v3.v);
		j = j_toe_BR = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Pfote2_R");
		jTrans_toe_BR = activeHS->objectSpace->pose[j].transformMat;

		j = j_paw_BL = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Pfote1_L");
		jTrans_paw_BL = activeHS->objectSpace->pose[j].transformMat;

		j = j_paw_BR = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Pfote1_R");
		jTrans_paw_BR = activeHS->objectSpace->pose[j].transformMat;



		// FL constraint
		sceneObject = demoMode->obj_wolf_constraint_FL;
		a3real4Real4x4Product(controlLocator_con_FL.v, controlToSkeleton.m,
			demoMode->sceneGraphState->localSpace->pose[sceneObject->sceneGraphIndex].transformMat.v3.v);
		j = j_elbow_FL = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Unterarm_L");
		jTrans_elbow_FL = activeHS->objectSpace->pose[j].transformMat;

		// FR constraint
		sceneObject = demoMode->obj_wolf_constraint_FR;
		a3real4Real4x4Product(controlLocator_con_FR.v, controlToSkeleton.m,
			demoMode->sceneGraphState->localSpace->pose[sceneObject->sceneGraphIndex].transformMat.v3.v);
		j = j_elbow_FR = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Unterarm_R");
		jTrans_elbow_FR = activeHS->objectSpace->pose[j].transformMat;

		// BL constraint
		sceneObject = demoMode->obj_wolf_constraint_BL;
		a3real4Real4x4Product(controlLocator_con_BL.v, controlToSkeleton.m,
			demoMode->sceneGraphState->localSpace->pose[sceneObject->sceneGraphIndex].transformMat.v3.v);
		j = j_elbow_BL = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Unterschenkel_L");
		jTrans_elbow_BL = activeHS->objectSpace->pose[j].transformMat;

		// BR constraint
		sceneObject = demoMode->obj_wolf_constraint_BR;
		a3real4Real4x4Product(controlLocator_con_BR.v, controlToSkeleton.m,
			demoMode->sceneGraphState->localSpace->pose[sceneObject->sceneGraphIndex].transformMat.v3.v);
		j = j_elbow_BR = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Unterschenkel_R");
		jTrans_elbow_BR = activeHS->objectSpace->pose[j].transformMat;



		// FL base
		j = j_base_FL = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Oberarm_L");
		jTrans_base_FL = activeHS->objectSpace->pose[j].transformMat;
		controlLocator_base_FL = jTrans_base_FL.v3;

		// FR base
		j = j_base_FR = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Oberarm_R");
		jTrans_base_FR = activeHS->objectSpace->pose[j].transformMat;
		controlLocator_base_FR = jTrans_base_FR.v3;

		// BL base
		j = j_base_BL = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Oberschenkel_L");
		jTrans_base_BL = activeHS->objectSpace->pose[j].transformMat;
		controlLocator_base_BL = jTrans_base_BL.v3;

		// BR base
		j = j_base_BR = a3hierarchyGetNodeIndex(activeHS->hierarchy, "Oberschenkel_R");
		jTrans_base_BR = activeHS->objectSpace->pose[j].transformMat;
		controlLocator_base_BR = jTrans_base_BR.v3;

		for (int i = 0; i < 4; i++)
		{
			a3i32 raycastStatus = 0;
			for (int j = 0; j < 3; j++)
			{
				raycastStatus = max(raycastStatus, demoMode->raycastHits[i][j]);
			}
			if (raycastStatus == 2)
			{
				if (i == 0)
				{
					updateFrontIK(demoMode, activeHS, baseHS, poseGroup, controlLocator_eff_FL.v,
						controlLocator_con_FL.v, controlLocator_base_FL.v, &jTrans_paw_FL, &jTrans_elbow_FL,
						&jTrans_base_FL, j_paw_FL, j_elbow_FL, j_base_FL);
				}
				if (i == 1)
				{
					updateFrontIK(demoMode, activeHS, baseHS, poseGroup, controlLocator_eff_FR.v,
						controlLocator_con_FR.v, controlLocator_base_FR.v, &jTrans_paw_FR, &jTrans_elbow_FR,
						&jTrans_base_FR, j_paw_FR, j_elbow_FR, j_base_FR);
				}
				if (i == 2)
				{
					//updateBackIK(demoMode, activeHS, baseHS, poseGroup, controlLocator_eff_BL.v,
					//	controlLocator_con_BL.v, controlLocator_base_BL.v, &jTrans_toe_BL, &jTrans_paw_BL, &jTrans_elbow_BL,
					//	&jTrans_base_BL, j_toe_BL, j_paw_BL, j_elbow_BL, j_base_BL);
				}
				if (i == 3)
				{
					//updateBackIK(demoMode, activeHS, baseHS, poseGroup, controlLocator_eff_BR.v,
					//	controlLocator_con_BR.v, controlLocator_base_BR.v, &jTrans_toe_BR, &jTrans_paw_BR, &jTrans_elbow_BR,
					//	&jTrans_base_BR, j_toe_BR, j_paw_BR, j_elbow_BR, j_base_BR);
				}
			}
		}
		jTrans_paw_FL = activeHS->objectSpace->pose[j_paw_FL].transformMat;
		jTrans_base_FL = activeHS->objectSpace->pose[j_base_FL].transformMat;

		jTrans_paw_FR = activeHS->objectSpace->pose[j_paw_FR].transformMat;
		jTrans_base_FR = activeHS->objectSpace->pose[j_base_FR].transformMat;

		a3f32 maxFrontShoulder = 0;
		a3f32 maxBackShoulder = 0;
		a3vec3 dist = a3vec3_zero;

		a3real3Diff(dist.v, controlLocator_eff_FL.xyz.v, jTrans_base_FL.v3.xyz.v);
		maxFrontShoulder = max(maxFrontShoulder, a3real3Length(dist.v));
		a3real3Diff(dist.v, controlLocator_eff_FR.xyz.v, jTrans_base_FR.v3.xyz.v);
		maxFrontShoulder = max(maxFrontShoulder, a3real3Length(dist.v));
		a3f32 propDist = a3sqrt(demoMode->character->frontLegMaxLengthSq);
		//if (maxFrontShoulder > propDist * 1.2f)
		//{
		//	a3f32 diff = maxFrontShoulder - (propDist * 1.2f);
		//	demoMode->obj_skeleton_ctrl->position.z -= diff;
		//}
		//else if (maxFrontShoulder < propDist / 1.2f)
		//{
		//	a3f32 diff = (propDist / 1.2f) - maxFrontShoulder;
		//	demoMode->obj_skeleton_ctrl->position.z += diff;
		//}
		a3real3Diff(dist.v, controlLocator_eff_BL.xyz.v, jTrans_base_BL.v3.xyz.v);
		maxBackShoulder = max(maxBackShoulder, a3real3Length(dist.v));
		a3real3Diff(dist.v, controlLocator_eff_BR.xyz.v, jTrans_base_BR.v3.xyz.v);
		maxBackShoulder = max(maxBackShoulder, a3real3Length(dist.v));
		propDist = a3sqrt(demoMode->character->backLegMaxLengthSq);
		if (maxBackShoulder > propDist * 1.2f)
		{
			a3f32 diff = maxBackShoulder - (propDist * 1.2f);
			demoMode->obj_skeleton_ctrl->position.z -= diff;
		}
		else if (maxBackShoulder < propDist / 1.2f)
		{
			a3f32 diff = (propDist / 1.2f) - maxBackShoulder;
			demoMode->obj_skeleton_ctrl->position.z += diff;
		}
		
	}
}
	a3boolean firstFrame = true;

	void updateRaycasts(a3_DemoMode1_Animation * demoMode, a3_HierarchyState * state)
	{
		a3_Ray* ray = calloc(1, sizeof(a3_Ray));
		a3vec3 newOrigin = a3vec3_zero;
		a3vec3 raycastOutput;
		for (int i = 0; i < 4; i++)
		{
			a3real3Sum(newOrigin.v, demoMode->ray[i].origin->v, demoMode->obj_skeleton_ctrl->position.v);
			a3createRay(ray, &newOrigin, demoMode->ray[i].direction);
			for (int j = 0; j < 3; j++)
			{
				//a3real3GetNegative(negatedOrigin.v, demoMode->ray[i].origin->v);
				demoMode->raycastHits[i][j] = 0;
				//if we've collided with the plane
				if (a3raycastGetCollisionBoundedPlane(ray, demoMode->plane + j, false, &raycastOutput))
				{
					demoMode->raycastHits[i][j] = 1;
				}
				else
				{
					if (a3raycastGetCollisionBoundedPlane(ray, demoMode->plane + j, true, &raycastOutput))
					{
						demoMode->raycastHits[i][j] = 2;
					}
					//if no raycast at all, snap effectors to feet.
					//in IK: if all j in an i are false, use IK to manip limb. For rear leg, offset ankle by something
					//then do normal IK with triangle stuff
				}
				if (demoMode->raycastHits[i][j] > 0)
				{
					demoMode->raycastPositions[i][j] = raycastOutput;
					demoMode->lastHitPositions[i] = raycastOutput;
					a3vec3 orig = *ray->origin;
					a3vec3 pos = orig;
					a3real3Sum(pos.v, orig.v, demoMode->ray[i].direction->v);
					demoMode->intersectionPoint[i] = pos;
					a3f32 tmp = pos.y; //mascot
				}
			}
			a3i32 anyTrue = 0;
			for (int j = 0; j < 3; j++)
			{
				anyTrue = max(demoMode->raycastHits[i][j], anyTrue);
			}
			if (anyTrue == 0)
			{
				demoMode->lastHitPositions[i] = *(ray->origin);
			}
		}
		for (int i = 0; i < 4; i++)
		{
			a3vec3 newPt = a3vec3_zero;
			a3real3Diff(newPt.v, demoMode->lastHitPositions[i].v, demoMode->obj_skeleton_ctrl->position.v);
			(demoMode->obj_wolf_effector_FL + i)->position = newPt;
		}
	}

	void a3animation_update_animation(a3_DemoMode1_Animation * demoMode, a3f64 const dt,
		a3boolean const updateIK)
	{
		a3_HierarchyState* activeHS_fk = demoMode->hierarchyState_skel_fk;
		a3_HierarchyState* activeHS_ik = demoMode->hierarchyState_skel_ik;
		a3_HierarchyState* activeHS = demoMode->hierarchyState_skel_final;
		a3_HierarchyState const* baseHS = demoMode->hierarchyState_skel_base;
		a3_HierarchyPoseGroup const* poseGroup = demoMode->hierarchyPoseGroup_skel;

		// switch controller to see different states
		// A is idle, arms down; B is skin test, arms out
		a3_ClipController* clipCtrl_fk = demoMode->clipCtrlA;
		//a3ui32 sampleIndex0, sampleIndex1;

		a3_HierarchyPose movingPose[1], idlePose[1];
		a3hierarchyPoseOpCreate(movingPose, activeHS_fk->hierarchy->numNodes);
		a3hierarchyPoseOpIdentity(movingPose, activeHS_fk->hierarchy->numNodes);
		a3hierarchyPoseOpCreate(idlePose, activeHS_fk->hierarchy->numNodes);
		a3hierarchyPoseOpIdentity(idlePose, activeHS_fk->hierarchy->numNodes);

		a3characterControllerUpdate(demoMode->character, movingPose, (a3real)dt);

		// resolve FK state
		// update clip controller, keyframe lerp
		a3clipControllerUpdate(clipCtrl_fk, dt);

		//characte->animControllers[0] is the idle controller for a Character Controller
		//sampleIndex0 = demoMode->clipPool->keyframe[demoMode->character->animControllers[0].keyframeIndex].sampleIndex0;
		//sampleIndex1 = demoMode->clipPool->keyframe[demoMode->character->animControllers[0].keyframeIndex].sampleIndex1;

		movingPose->pose[0].translate.x = demoMode->sceneGraphState->objectSpace->pose[demoMode->character->object->sceneGraphIndex].translate.x;
		//movingPose->pose[0].translate.y = demoMode->sceneGraphState->objectSpace->pose[demoMode->character->object->sceneGraphIndex].translate.y;
		movingPose->pose[0].translate.z = demoMode->sceneGraphState->objectSpace->pose[demoMode->character->object->sceneGraphIndex].translate.z;

		a3hierarchyPoseCopy(activeHS_fk->animPose, movingPose, activeHS_fk->hierarchy->numNodes);

		// run FK pipeline
		a3animation_update_fk(activeHS_fk, baseHS, poseGroup);

		if (firstFrame)
		{
			a3i32 jUpLeg = a3hierarchyGetNodeIndex(activeHS_fk->hierarchy, "Oberarm_L");
			a3i32 jDownLeg = a3hierarchyGetNodeIndex(activeHS_fk->hierarchy, "Unterarm_L");
			a3i32 jFoot = a3hierarchyGetNodeIndex(activeHS_fk->hierarchy, "Vorderpfote_L");
			a3_SpatialPose* upLegObj = &activeHS_fk->objectSpace->pose[jUpLeg];
			a3_SpatialPose* downLegObj = &activeHS_fk->objectSpace->pose[jDownLeg];
			a3_SpatialPose* footObj = &activeHS_fk->objectSpace->pose[jFoot];
			a3vec3 vector, v1, v2, v3, v4;
			a3real3Diff(v1.v, footObj->transformMat.v3.xyz.v, downLegObj->transformMat.v3.xyz.v);
			a3real3Diff(v2.v, downLegObj->transformMat.v3.xyz.v, upLegObj->transformMat.v3.xyz.v);
			a3real3Sum(vector.v, v1.v, v2.v);

			demoMode->character->frontLegMaxLengthSq = a3real3LengthSquared(vector.v);

			jUpLeg = a3hierarchyGetNodeIndex(activeHS_fk->hierarchy, "Oberschenkel_L");
			jDownLeg = a3hierarchyGetNodeIndex(activeHS_fk->hierarchy, "Unterschenkel_L");
			jFoot = a3hierarchyGetNodeIndex(activeHS_fk->hierarchy, "Vorderpfote_L");
			a3i32 jToe = a3hierarchyGetNodeIndex(activeHS_fk->hierarchy, "Pfote2_L");

			upLegObj = &activeHS_fk->objectSpace->pose[jUpLeg];
			downLegObj = &activeHS_fk->objectSpace->pose[jDownLeg];
			footObj = &activeHS_fk->objectSpace->pose[jFoot];
			a3_SpatialPose* toeObj = &activeHS_fk->objectSpace->pose[jToe];
			a3real3Diff(v1.v, footObj->transformMat.v3.xyz.v, downLegObj->transformMat.v3.xyz.v);
			a3real3Diff(v2.v, downLegObj->transformMat.v3.xyz.v, upLegObj->transformMat.v3.xyz.v);
			a3real3Diff(v3.v, toeObj->transformMat.v3.xyz.v, footObj->transformMat.v3.xyz.v);
			a3real3Sum(v4.v, v1.v, v2.v);
			a3real3Sum(vector.v, v3.v, v4.v);

			demoMode->character->backLegMaxLengthSq = a3real3LengthSquared(vector.v);
			firstFrame = false;
		}
		// resolve IK state
		// copy FK to IK
		a3hierarchyPoseCopy(
			activeHS_ik->animPose,	// dst: IK anim
			activeHS_fk->animPose,	// src: FK anim
			//baseHS->animPose,	// src: base anim
			activeHS_ik->hierarchy->numNodes);
		// run FK
		a3animation_update_fk(activeHS_ik, baseHS, poseGroup);
		if (updateIK)
			//if (false)
		{
			// invert object-space
			a3hierarchyStateUpdateObjectInverse(activeHS_ik);

			updateRaycasts(demoMode, activeHS_ik);
			// run solvers
			a3animation_update_applyEffectors(demoMode, activeHS_ik, baseHS, poseGroup);
			// run full IK pipeline (if not resolving with effectors)
			//a3animation_update_ik(activeHS_ik, baseHS, poseGroup);
		}

		// blend FK/IK to final
		// testing: copy source to final
		a3hierarchyPoseCopy(activeHS->animPose,	// dst: final anim
			//activeHS_fk->animPose,	// src: FK anim
			activeHS_ik->animPose,	// src: IK anim
			//baseHS->animPose,	// src: base anim (identity)
			activeHS->hierarchy->numNodes);
		// run FK pipeline (skinning optional)
		a3animation_update_fk(activeHS, baseHS, poseGroup);
		a3animation_update_skin(activeHS, baseHS);
	}

	void a3animation_update_sceneGraph(a3_DemoMode1_Animation * demoMode, a3f64 const dt)
	{
		a3ui32 i;
		a3mat4 scaleMat = a3mat4_identity;

		a3demo_update_objects(dt, demoMode->object_scene, animationMaxCount_sceneObject, 0, 0);
		a3demo_update_objects(dt, demoMode->obj_camera_main, 1, 1, 0);

		a3demo_updateProjectorViewProjectionMat(demoMode->proj_camera_main);

		// apply scales to objects
		for (i = 0; i < animationMaxCount_sceneObject; ++i)
		{
			a3demo_applyScale_internal(demoMode->object_scene + i, scaleMat.m);
		}

		// update skybox
		a3demo_update_bindSkybox(demoMode->obj_camera_main, demoMode->obj_skybox);

		for (i = 0; i < animationMaxCount_sceneObject; ++i)
			demoMode->sceneGraphState->localSpace->pose[i].transformMat = demoMode->object_scene[i].modelMat;
		a3kinematicsSolveForward(demoMode->sceneGraphState);
		a3hierarchyStateUpdateLocalInverse(demoMode->sceneGraphState);
		a3hierarchyStateUpdateObjectInverse(demoMode->sceneGraphState);
	}

	void a3animation_update(a3_DemoState * demoState, a3_DemoMode1_Animation * demoMode, a3f64 const dt)
	{
		a3ui32 i;
		a3_DemoModelMatrixStack matrixStack[animationMaxCount_sceneObject];

		// active camera
		a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
		a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

		// skeletal
		if (demoState->updateAnimation)
			a3animation_update_animation(demoMode, dt, 1);

		// update scene graph local transforms
		a3animation_update_sceneGraph(demoMode, dt);
		printf("%f", demoMode->sceneGraphState->objectSpace->pose[4].transformMat.v3.x);
		printf(", %f", demoMode->sceneGraphState->objectSpace->pose[4].transformMat.v3.y);
		printf(", %f\n", demoMode->sceneGraphState->objectSpace->pose[4].transformMat.v3.z);

		// update matrix stack data using scene graph
		for (i = 0; i < animationMaxCount_sceneObject; ++i)
		{
			a3demo_updateModelMatrixStack(matrixStack + i,
				activeCamera->projectionMat.m,
				demoMode->sceneGraphState->objectSpace->pose[demoMode->obj_camera_main->sceneGraphIndex].transformMat.m,
				demoMode->sceneGraphState->objectSpaceInv->pose[demoMode->obj_camera_main->sceneGraphIndex].transformMat.m,
				demoMode->sceneGraphState->objectSpace->pose[demoMode->object_scene[i].sceneGraphIndex].transformMat.m,
				a3mat4_identity.m);
		}

		// prepare and upload graphics data
		a3animation_update_graphics(demoState, demoMode, matrixStack, demoMode->hierarchyState_skel_final);

		// testing: reset IK effectors to lock them to FK result
		{
			//void a3animation_load_resetEffectors(a3_DemoMode1_Animation * demoMode,
			//	a3_HierarchyState * hierarchyState, a3_HierarchyPoseGroup const* poseGroup);
			//a3animation_load_resetEffectors(demoMode,
			//	demoMode->hierarchyState_skel_final, demoMode->hierarchyPoseGroup_skel);
		}

		//a3vec3 point = a3vec3_zero;
		//a3boolean collisionResult = a3raycastGetCollisionUnboundedPlane(demoMode->ray, demoMode->plane, true, &demoMode->intersectionPoint);
		//a3vec3 coll2Point;
		//a3boolean coll2 = a3raycastGetCollisionBoundedPlane(demoMode->ray, demoMode->plane, true, &(demoMode->intersectionPoint[0]));
		//printf("%i\n", collisionResult);
		//printf("%f, %f, %f\n", demoMode->ray->direction->x, demoMode->ray->direction->y, demoMode->ray->direction->z);
		//printf("%f, %f, %f\n", demoMode->plane->normal->x, demoMode->plane->normal->y, demoMode->plane->normal->z);
		//printf("%f, %f, %f\n", demoMode->intersectionPoint.x, demoMode->intersectionPoint.y, demoMode->intersectionPoint.z);
		//printf("%i\n", coll2);
		//printf("%f, %f, %f\n", coll2Point.x, coll2Point.y, coll2Point.z);
		//printf("%f\n", point.x);

		// ****TO-DO:
		// process input
		demoMode->axis_r[0] *= 180.0f;
		demoMode->axis_l[0] *= 4.0f;
		demoMode->axis_l[1] *= 4.0f;
		switch (demoMode->ctrl_position)
		{
		case animation_input_direct:
			demoMode->pos.x = (a3real)demoMode->axis_l[0];
			demoMode->pos.y = (a3real)demoMode->axis_l[1];

			break;
		case animation_input_euler:
		{
			//Put raw values into container variables (a3real3 because that's what our integration functions run on)

			demoMode->vel.x = (a3real)demoMode->axis_l[0];
			demoMode->vel.y = (a3real)demoMode->axis_l[1];
			demoMode->acc = a3vec2_zero;

			/* let it be known, cameron, when left on autopilot, will manually calculate completely uncesessary values
			* yes, this calculates acceleration from temp variables.
			a3real3 dxdt, dxrdt;
			dxdt[0] = (tempVel[0] - demoMode->vel.x) / (a3real)dt;
			dxdt[1] = (tempVel[1] - demoMode->vel.y) / (a3real)dt;
			dxdt[2] = 0.0f;

			dxrdt[0] = (tempVelR[0] - demoMode->velr) / (a3real)dt;
			dxrdt[1] = 0.0f;
			dxrdt[2] = 0.0f;
			*/

			a3vec3 velIn = { demoMode->vel.x, demoMode->vel.y, 0 };
			a3vec3 posIn = { demoMode->pos.x, demoMode->pos.y, 0 };
			a3vec3 posOut = a3vec3_zero;

			a3EulerIntegration(posOut.v, posIn.v, velIn.v, (a3real)dt);

			demoMode->pos.x = posOut.x;
			demoMode->pos.y = posOut.y;
		}
		break;
		case animation_input_kinematic:
			demoMode->acc.x = (a3real)demoMode->axis_l[0] * 4.0f;
			demoMode->acc.y = (a3real)demoMode->axis_l[1] * 4.0f;

			a3vec3 accIn = { demoMode->acc.x, demoMode->acc.y, 0 };

			a3vec3 velIn = { demoMode->vel.x, demoMode->vel.y, 0 };
			a3vec3 posIn = { demoMode->pos.x, demoMode->pos.y, 0 };

			a3vec3 velOut = a3vec3_zero;
			a3vec3 posOut = a3vec3_zero;

			a3KinematicIntegration(posOut.v, velOut.v, posIn.v, velIn.v, accIn.v, (a3real)dt);

			demoMode->pos.x = posOut.x;
			demoMode->pos.y = posOut.y;
			demoMode->vel.x = velOut.x;
			demoMode->vel.y = velOut.y;

			break;
		case animation_input_interpolate1:
		{
			a3real2 targetPos;
			targetPos[0] = (a3real)demoMode->axis_l[0];
			targetPos[1] = (a3real)demoMode->axis_l[1];


			a3real2Lerp(demoMode->pos.v, demoMode->pos.v, targetPos, 0.4f);
		}
		break;
		case animation_input_interpolate2:
		{
			a3vec3 velIn = { demoMode->vel.x, demoMode->vel.y, 0 };

			a3vec3 posIn = { demoMode->pos.x, demoMode->pos.y, 0 };
			a3vec3 posOut = a3vec3_zero;
			a3EulerIntegration(posOut.v, posIn.v, velIn.v, (a3real)dt);

			a3real2 targetVel = { (a3real)demoMode->axis_l[0], (a3real)demoMode->axis_l[1] };

			a3vec3 velOut = a3vec3_zero;
			a3real2Lerp(velOut.v, velIn.v, targetVel, 0.4f);

			demoMode->pos.x = posOut.x;
			demoMode->pos.y = posOut.y;

			demoMode->vel.x = velOut.x;
			demoMode->vel.y = velOut.y;

		}
		break;
		case animation_input_interpolate3:
		{
			a3vec3 accIn = { demoMode->acc.x, demoMode->acc.y, 0 };

			a3vec3 velIn = { demoMode->vel.x, demoMode->vel.y, 0 };
			a3vec3 posIn = { demoMode->pos.x, demoMode->pos.y, 0 };
			a3vec3 posOut = a3vec3_zero;
			a3vec3 velOut = a3vec3_zero;
			a3KinematicIntegration(posOut.v, velOut.v, posIn.v, velIn.v, accIn.v, (a3real)dt);

			demoMode->pos.x = posOut.x;
			demoMode->pos.y = posOut.y;

			demoMode->vel.x = velOut.x;
			demoMode->vel.y = velOut.y;

			a3real2 targetAcc = { (a3real)demoMode->axis_l[0], (a3real)demoMode->axis_l[1] };
			a3vec3 accOut = a3vec3_zero;
			a3real2Lerp(accOut.v, accIn.v, targetAcc, 0.6f);

			demoMode->acc.x = accOut.x;
			demoMode->acc.y = accOut.y;
		}
		break;
		}

		switch (demoMode->ctrl_rotation)
		{
		case animation_input_direct:
			demoMode->rot = (a3real)a3clamp(-180.0f, 180.0f, demoMode->axis_r[0]);	//Clamp between -180 and +180 degrees

			break;
		case animation_input_euler:
		{
			demoMode->velr = (a3real)demoMode->axis_r[0];
			demoMode->accr = 0;

			a3vec3 rVelIn = { demoMode->velr, 0, 0 };
			a3vec3 rotIn = { demoMode->rot, 0, 0 };
			a3vec3 rotOut = a3vec3_zero;
			a3EulerIntegration(rotOut.v, rotIn.v, rVelIn.v, (a3real)dt);

			demoMode->rot = (float)fmod(rotOut.x, 360.0f);
		}
		break;
		case animation_input_kinematic:
			demoMode->accr = (a3real)demoMode->axis_r[0];

			a3vec3 rAccIn = { demoMode->accr, 0, 0 };

			a3vec3 rVelIn = { demoMode->velr, 0, 0 };
			a3vec3 rotIn = { demoMode->rot, 0, 0 };

			a3vec3 rVelOut = a3vec3_zero;
			a3vec3 rotOut = a3vec3_zero;

			a3KinematicIntegration(rotOut.v, rVelOut.v, rotIn.v, rVelIn.v, rAccIn.v, (a3real)dt);

			demoMode->rot = (float)fmod(rotOut.x, 360.0f);
			demoMode->velr = rVelOut.x;
			break;
		case animation_input_interpolate1:
		{
			demoMode->accr = 0;
			demoMode->velr = 0;
			a3real targetRot;

			targetRot = ((a3real)demoMode->axis_r[0]);

			a3real rotOut = a3lerp(demoMode->rot, targetRot, 0.4f);
			demoMode->rot = rotOut;
		}
		break;
		case animation_input_interpolate2:
		{
			demoMode->accr = 0;
			a3vec3 rVelIn = { demoMode->velr, 0, 0 };

			a3vec3 rotIn = { demoMode->rot, 0, 0 };
			a3vec3 rotOut = a3vec3_zero;
			a3EulerIntegration(rotOut.v, rotIn.v, rVelIn.v, (a3real)dt);

			a3real targetRVel = (a3real)demoMode->axis_r[0];

			a3real rVelOut = a3lerp(demoMode->velr, targetRVel, 0.4f);
			demoMode->velr = rVelOut;

			demoMode->rot = (float)fmod(rotOut.x, 360.0f);
		}
		break;
		case animation_input_interpolate3:
		{
			a3vec3 rAccIn = { demoMode->accr, 0, 0 };

			a3vec3 rVelIn = { demoMode->velr, 0, 0 };
			a3vec3 rotIn = { demoMode->rot, 0, 0 };
			a3vec3 rotOut = a3vec3_zero;
			a3vec3 rVelOut = a3vec3_zero;
			a3KinematicIntegration(rotOut.v, rVelOut.v, rotIn.v, rVelIn.v, rAccIn.v, (a3real)dt);

			demoMode->rot = rotOut.x;

			demoMode->velr = rVelOut.x;

			a3real targetRAcc = (a3real)demoMode->axis_r[0];

			a3real rAccOut = a3lerp(demoMode->accr, targetRAcc, 0.6f);

			demoMode->accr = rAccOut;
		}
		break;
		}
		// apply input
		a3characterControllerApplyInput(demoMode->character, &demoMode->pos, demoMode->rot, (a3real)dt);

		//demoMode->obj_skeleton_ctrl->position.x = +(demoMode->pos.x);
		//demoMode->obj_skeleton_ctrl->position.y = +(demoMode->pos.y);
		//demoMode->obj_skeleton_ctrl->euler.z = -a3trigValid_sind(demoMode->rot);
	}


	//-----------------------------------------------------------------------------
