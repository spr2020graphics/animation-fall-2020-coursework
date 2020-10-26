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
	
	a3_HierarchyStateBlend.inl
	Implementation of inline hierarchical blend operations.
*/

/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
*/


#ifdef __ANIMAL3D_HIERARCHYSTATEBLEND_H
#ifndef __ANIMAL3D_HIERARCHYSTATEBLEND_INL
#define __ANIMAL3D_HIERARCHYSTATEBLEND_INL

#include <string.h>
#include <math.h>
#include "..\a3_HierarchyStateBlend.h"
#include <stdlib.h>


//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out)
{
	// ...
	pose_out->transform = a3mat4_identity;
	pose_out->orientation = a3vec3_zero;
	pose_out->scale = a3vec3_one;
	pose_out->position = a3vec3_zero;
	pose_out->rotation = a3quat_identity;
	pose_out->rotation.w = 1.0f;
	// done
	return pose_out;
}

// pointer-based initialization with specified values
inline a3_SpatialPose* a3spatialPoseOpInit(a3_SpatialPose* pose_out, a3vec3 scale, a3vec3 orientation, a3vec3 translation)
{
	pose_out->transform = a3mat4_identity;
	pose_out->scale = scale;
	orientation.x = (a3real) fmod(orientation.x, 360.0f);
	orientation.y = (a3real) fmod(orientation.y, 360.0f);
	orientation.z = (a3real) fmod(orientation.z, 360.0f);
	pose_out->orientation = orientation;
	pose_out->position = translation;
	a3quatSetEulerXYZ(pose_out->rotation.q, orientation.x, orientation.y, orientation.z);

	//a3spatialPoseConvert(pose_out->transform.m, pose_out, a3poseChannel_translate_xyz | a3poseChannel_orient_xyz | a3poseChannel_scale_xyz, 0); //how to handle different euler orders?
	return pose_out;
}

// pointer-based nearest, takes 2 controls and u
inline a3_SpatialPose* a3spatialPoseOpNearest(a3_SpatialPose* spatialPose_out, a3_SpatialPose* const spatialPose_0, a3_SpatialPose* const spatialPose_1, const a3real u)
{
	if (u < 0.5f)
	{
		spatialPose_out->orientation = spatialPose_0->orientation;
		spatialPose_out->scale = spatialPose_0->scale;
		spatialPose_out->position = spatialPose_0->position;
		spatialPose_out->rotation = spatialPose_0->rotation;
	}
	else
	{
		spatialPose_out->orientation = spatialPose_1->orientation;
		spatialPose_out->scale = spatialPose_1->scale;
		spatialPose_out->position = spatialPose_1->position;
		spatialPose_out->rotation = spatialPose_1->rotation;
	}

	return spatialPose_out;
}

// pointer-based copy operation
inline a3_SpatialPose* a3spatialPoseOpCopy(a3_SpatialPose* pose_out, a3_SpatialPose* const pose_in)
{
	*pose_out = *pose_in;
	return pose_out;
}

// simple passthrough.
inline a3_SpatialPose* a3spatialPoseOpConst(a3_SpatialPose* pose_inout)
{
	return pose_inout;
}

// pointer-based LERP operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPose* const pose0, a3_SpatialPose* const pose1, a3real const u)
{
	pose_out->transform = a3mat4_identity;
	// ...
	a3real3Lerp(pose_out->orientation.v, pose0->orientation.v, pose1->orientation.v, u);
	a3real3Lerp(pose_out->scale.v, pose0->scale.v, pose1->scale.v, u);
	a3real3Lerp(pose_out->position.v, pose0->position.v, pose1->position.v, u);
	a3quatSlerp(pose_out->rotation.q, pose0->rotation.q, pose1->rotation.q, u);

	return pose_out;
}

// pointer-based cubic, 4 controls + u
inline a3_SpatialPose* a3spatialPoseOpCubic(a3_SpatialPose* spatialPose_out, a3_SpatialPose* const spatialPose_Prev, a3_SpatialPose* const spatialPose_0, a3_SpatialPose* const spatialPose_1, a3_SpatialPose* const spatialPose_Next, const a3real u)
{
	// still need to account for quaternion lerping
	a3real3CatmullRom(spatialPose_out->orientation.v, spatialPose_Prev->orientation.v, spatialPose_0->orientation.v, spatialPose_1->orientation.v, spatialPose_Next->orientation.v, u);
	a3real3CatmullRom(spatialPose_out->scale.v, spatialPose_Prev->scale.v, spatialPose_0->scale.v, spatialPose_1->scale.v, spatialPose_Next->scale.v, u);
	a3real3CatmullRom(spatialPose_out->position.v, spatialPose_Prev->position.v, spatialPose_0->position.v, spatialPose_1->position.v, spatialPose_Next->position.v, u);
	a3real4CatmullRom(spatialPose_out->rotation.qv, spatialPose_Prev->rotation.qv, spatialPose_0->rotation.qv, spatialPose_1->rotation.qv, spatialPose_Next->rotation.qv, u);	//Sketchy, but there's no quaternion catmull rom (qv is the vector portion of a quat)

	return spatialPose_out;
}

// pointer-based triangular lerp, 3 controls + u1 + u2
inline a3_SpatialPose* a3spatialPoseOpTriangularLERP(a3_SpatialPose* pose_out, a3_SpatialPose* const pose0, a3_SpatialPose* const pose1, a3_SpatialPose* const pose2, a3real const u1, a3real const u2)
{
	a3real u0 = 1 - u1 - u2;
	
	a3_SpatialPose tmpPoses[4];
	a3spatialPoseOpScale(&tmpPoses[0], pose0, u0);
	a3spatialPoseOpScale(&tmpPoses[1], pose1, u1);
	a3spatialPoseOpScale(&tmpPoses[2], pose2, u2);

	a3spatialPoseOpConcat(&tmpPoses[3], &tmpPoses[0], &tmpPoses[1]);
	a3spatialPoseOpConcat(pose_out, &tmpPoses[3], &tmpPoses[2]);

	return pose_out;
}

// pointer-based binearest, 4 controls + 3 u values
inline a3_SpatialPose* a3spatialPoseOpBiNearest(a3_SpatialPose* pose_out, a3_SpatialPose* const pose00, a3_SpatialPose* const pose01, a3_SpatialPose* const pose10, a3_SpatialPose* const pose11, a3real const u0, a3real const u1, a3real const u)
{
	a3_SpatialPose tmpPoses[2];
	a3spatialPoseOpNearest(&tmpPoses[0],pose00, pose01, u0);
	a3spatialPoseOpNearest(&tmpPoses[1],pose10, pose11, u1);
	a3spatialPoseOpNearest(pose_out, &tmpPoses[0], &tmpPoses[1], u);
	return pose_out;
}

// pointer-based bilerp, 4 controls + 3 u values
inline a3_SpatialPose* a3spatialPoseOpBiLerp(a3_SpatialPose* pose_out, a3_SpatialPose* const pose00, a3_SpatialPose* const pose01, a3_SpatialPose* const pose10, a3_SpatialPose* const pose11, a3real const u0, a3real const u1, a3real const u)
{
	a3_SpatialPose tmpPoses[2];
	a3spatialPoseOpLERP(&tmpPoses[0], pose00, pose01, u0);
	a3spatialPoseOpLERP(&tmpPoses[1], pose10, pose11, u1);

	a3spatialPoseOpLERP(pose_out, &tmpPoses[0], &tmpPoses[1], u);
	return pose_out;
}

// pointer-based bicubic, 16 controls + 5 u values
inline a3_SpatialPose* a3spatialPoseOpBiCubic(a3_SpatialPose* pose_out, 
	a3_SpatialPose* const pose_prev00, a3_SpatialPose* const pose_prev01, 
	a3_SpatialPose* const pose_prev10, a3_SpatialPose* const pose_prev11, 
	a3_SpatialPose* const pose00, a3_SpatialPose* const pose01, 
	a3_SpatialPose* const pose10, a3_SpatialPose* const pose11, 
	a3_SpatialPose* const pose00_2, a3_SpatialPose* const pose20, 
	a3_SpatialPose* const pose02, a3_SpatialPose* const pose22, 
	a3_SpatialPose* const pose_next00, a3_SpatialPose* const pose_next01, 
	a3_SpatialPose* const pose_next10, a3_SpatialPose* const pose_next11, 
	a3real const u_prev, a3real const u0, a3real const u1, a3real const u_next, a3real u)
{
	a3_SpatialPose result_prev[1], result_0[1], result_1[1], result_next[1];

	a3spatialPoseOpCubic(result_prev, pose_prev00, pose00, pose00_2, pose_next00, u_prev);
	a3spatialPoseOpCubic(result_0, pose_prev01, pose01, pose20, pose_next01, u0);
	a3spatialPoseOpCubic(result_1, pose_prev10, pose10, pose02, pose_next10, u1);
	a3spatialPoseOpCubic(result_next, pose_prev11, pose11, pose22, pose_next11, u_next);

	a3spatialPoseOpCubic(pose_out, result_prev, result_0, result_1, result_next, u);

	return pose_out;
}

// pointer-based scale. 1 control + u
inline a3_SpatialPose* a3spatialPoseOpScale(a3_SpatialPose* pose_out, a3_SpatialPose* const pose_in, a3real u)
{
	a3_SpatialPose identity[1];
	a3spatialPoseOpIdentity(identity);
	a3spatialPoseOpLERP(pose_out, identity, pose_in, u);
	return pose_out;
}

// pointer-based negate
inline a3_SpatialPose* a3spatialPoseOpNegate(a3_SpatialPose* pose_out, a3_SpatialPose* const pose_in)
{
	a3real3GetNegative(pose_out->orientation.v, pose_in->orientation.v);
	a3real3GetNegative(pose_out->position.v,	pose_in->position.v);
	pose_out->scale.x = 1.0f / pose_in->scale.x;
	pose_out->scale.y = 1.0f / pose_in->scale.y;
	pose_out->scale.z = 1.0f / pose_in->scale.z;
	a3quatGetConjugated(pose_out->rotation.q, pose_in->rotation.q);	// the conjugate is the inverse

	return pose_out;
}

//pointer-based concat
inline a3_SpatialPose* a3spatialPoseOpConcat(a3_SpatialPose* pose_out, a3_SpatialPose* const pose0, a3_SpatialPose* const pose1)
{
	a3real3Sum(pose_out->position.v, pose0->position.v, pose1->position.v);
	a3real3Sum(pose_out->orientation.v, pose0->orientation.v, pose1->orientation.v);
	a3real3ProductComp(pose_out->scale.v, pose0->scale.v, pose1->scale.v);
	a3quatProduct(pose_out->rotation.q, pose0->rotation.q, pose1->rotation.q);

	return pose_out;
}

//pointer-based deconcat (subtract/divide)
inline a3_SpatialPose* a3spatialPoseOpDeconcat(a3_SpatialPose* pose_out, a3_SpatialPose* const pose0, a3_SpatialPose* const pose1)
{
	a3real3Diff(pose_out->position.v, pose0->position.v, pose1->position.v);
	a3real3Diff(pose_out->orientation.v, pose0->orientation.v, pose1->orientation.v);
	a3real3QuotientComp(pose_out->scale.v, pose0->scale.v, pose1->scale.v);
	a3real4QuotientComp(pose_out->rotation.qv, pose0->rotation.qv, pose1->rotation.qv); // This is sketchy, but I don't see any sort of a3quat division or reversal

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpEaseInOut(a3_SpatialPose* pose_out, a3_SpatialPose* const pose0, a3_SpatialPose* const pose1, const a3real u)
{
	a3real3Bezier1(pose_out->position.v, pose0->position.v, pose1->position.v, u);
	a3real3Bezier1(pose_out->orientation.v, pose0->orientation.v, pose1->orientation.v, u);
	a3real3Bezier1(pose_out->scale.v, pose0->scale.v, pose1->scale.v, u);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpConvert(a3_SpatialPose* pose_inout, const a3_SpatialPoseEulerOrder order)
{
	//create translate matrix. There's probably an a3 call for this, but it works.
	a3mat4 mat_out = a3mat4_identity;

	a3mat4 translate = a3mat4_identity;
	a3vec4 spatial_translate;
	spatial_translate.xyz = pose_inout->position;
	spatial_translate.w = 1;
	translate.v3 = spatial_translate;

	//create rotation matrix
	a3mat4 rotate = a3mat4_identity;
	a3mat4 xRot, yRot, zRot, tmp;

	//initialize rotation matrices
	xRot = a3mat4_identity;
	a3real4x4SetRotateX(xRot.m, pose_inout->orientation.x);
	yRot = a3mat4_identity;
	a3real4x4SetRotateY(yRot.m, pose_inout->orientation.y);
	zRot = a3mat4_identity;
	a3real4x4SetRotateZ(zRot.m, pose_inout->orientation.z);
	tmp = a3mat4_identity;

	//concatenate rotation matrices. xyz and zyx have shortcuts
	switch (order)
	{
	case a3poseEulerOrder_xyz:
		a3real4x4SetRotateXYZ(rotate.m, pose_inout->orientation.x, pose_inout->orientation.y, pose_inout->orientation.z);
		break;
	case a3poseEulerOrder_zyx:
		a3real4x4SetRotateZYX(rotate.m, pose_inout->orientation.x, pose_inout->orientation.y, pose_inout->orientation.z);
		break;
	case a3poseEulerOrder_zxy:
		a3real4x4Product(tmp.m, xRot.m, yRot.m);
		a3real4x4Product(rotate.m, zRot.m, tmp.m);
		break;
	case a3poseEulerOrder_yxz:
		a3real4x4Product(tmp.m, xRot.m, zRot.m);
		a3real4x4Product(rotate.m, yRot.m, tmp.m);
		break;
	case a3poseEulerOrder_xzy:
		a3real4x4Product(tmp.m, zRot.m, yRot.m);
		a3real4x4Product(rotate.m, xRot.m, tmp.m);
		break;
	case a3poseEulerOrder_yzx:
		a3real4x4Product(tmp.m, zRot.m, xRot.m);
		a3real4x4Product(rotate.m, yRot.m, tmp.m);
	}

	//create scale matrix
	a3mat4 scale = a3mat4_identity;
	scale.m00 = pose_inout->scale.x;
	scale.m11 = pose_inout->scale.y;
	scale.m22 = pose_inout->scale.z;
	// Algorithm in slides

	//form TRS matrix
	a3mat4 rs;
	a3real4x4Product(rs.m, rotate.m, scale.m);
	a3real4x4Product(mat_out.m, translate.m, rs.m);

	a3real4x4SetReal4x4(pose_inout->transform.m, mat_out.m);

	return pose_inout;
}

inline a3mat4* a3OpForwardKinematics(a3mat4* const objectTransform_out, a3_Hierarchy* const hierarchy, a3mat4* const localTransform)
{
	

	return objectTransform_out;
}

//-----------------------------------------------------------------------------

// data-based reset/identity
inline a3_SpatialPose a3spatialPoseDOpIdentity()
{
	a3_SpatialPose result = { a3mat4_identity, a3vec3_zero, a3vec3_one, a3vec3_zero, a3quat_identity };
	result.rotation.w = 1.0f;	//ensuring this is 1, always

	return result;
}

// data-based init
inline a3_SpatialPose a3spatialPoseDOpInit(const a3vec3 scale, const a3vec3 orientation, const a3vec3 translation)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpInit(result, scale, orientation, translation);

	return *result;
}

// data-based nearest
inline a3_SpatialPose a3spatialPoseDOpNearest(a3_SpatialPose spatialPose_0, a3_SpatialPose spatialPose_1, const a3real u)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpNearest(result, &spatialPose_0, &spatialPose_1, u);

	return *result;
}

// data-based LERP
inline a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose pose0, a3_SpatialPose pose1, a3real const u)
{
	a3_SpatialPose result[1];
	
	a3spatialPoseOpLERP(result, &pose0, &pose1, u);

	// done
	return *result;
}

// data-based cubic
inline a3_SpatialPose a3spatialPoseDOpCubic(a3_SpatialPose spatialPose_Prev, a3_SpatialPose spatialPose_0, a3_SpatialPose spatialPose_1, a3_SpatialPose spatialPose_Next, const a3real u)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpCubic(result, &spatialPose_Prev, &spatialPose_0, &spatialPose_1, &spatialPose_Next, u);

	return *result;
}

// data-based triangular lerp
inline a3_SpatialPose a3spatialPoseDOpTriangularLERP(a3_SpatialPose pose0, a3_SpatialPose pose1, a3_SpatialPose pose2, a3real const u1, a3real const u2)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpTriangularLERP(result, &pose0, &pose1, &pose2, u1, u2);

	return *result;
}

// data-based binearest
inline a3_SpatialPose a3spatialPoseDOpBiNearest(a3_SpatialPose pose00, a3_SpatialPose pose01, a3_SpatialPose pose10, a3_SpatialPose pose11, a3real const u0, a3real const u1, a3real const u)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpBiNearest(result, &pose00, &pose01, &pose10, &pose10, u0, u1, u);
	return *result;
}

// data-based bilerp
inline a3_SpatialPose a3spatialPoseDOpBiLerp(a3_SpatialPose pose00, a3_SpatialPose pose01, a3_SpatialPose pose10, a3_SpatialPose pose11, a3real const u0, a3real const u1, a3real const u)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpBiLerp(result, &pose00, &pose01, &pose10, &pose11, u0, u1, u);

	return *result;
}

// data-based bicubic
inline a3_SpatialPose a3spatialPoseDOpBiCubic(a3_SpatialPose pose_prev00, a3_SpatialPose pose_prev01, a3_SpatialPose pose_prev10, a3_SpatialPose pose_prev11, a3_SpatialPose pose00, a3_SpatialPose pose01, a3_SpatialPose pose10, a3_SpatialPose pose11, a3_SpatialPose pose00_2, a3_SpatialPose pose20, a3_SpatialPose pose02, a3_SpatialPose pose22, a3_SpatialPose pose_next00, a3_SpatialPose pose_next01, a3_SpatialPose pose_next10, a3_SpatialPose pose_next11, a3real const u_prev, a3real const u0, a3real const u1, a3real const u_next, a3real u)
{
	a3_SpatialPose result[1];
	a3spatialPoseOpBiCubic(result,
		&pose_prev00, &pose_prev01, &pose_prev10, &pose_prev11,
		&pose00, &pose01, &pose10, &pose11, &pose00_2,
		&pose20, &pose02, &pose22,
		&pose_next00, &pose_next01, &pose_next10, &pose_next11,
		u_prev, u0, u1, u_next, u);
	return *result;
}

// data-based scale
inline a3_SpatialPose a3spatialPoseDOpScale(a3_SpatialPose pose_in, a3real u)
{
	a3_SpatialPose result = a3spatialPoseDOpLERP(a3spatialPoseDOpIdentity(), pose_in, u);

	return result;
}

// data-based negate
inline a3_SpatialPose a3spatialPoseDOpNegate(a3_SpatialPose pose_in)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpNegate(result, &pose_in);

	// done
	return *result;
}

// data-based const. Same as copy
inline a3_SpatialPose a3spatialPoseDOpConst(a3_SpatialPose pose_in)
{
	return pose_in;
}

// data-based copy. Same as const
inline a3_SpatialPose a3spatialPoseDOpCopy(a3_SpatialPose pose_in)
{
	return pose_in;
}

// data-based concat
inline a3_SpatialPose a3spatialPoseDOpConcat(a3_SpatialPose pose0, a3_SpatialPose pose1)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpConcat(result, &pose0, &pose1);

	// done
	return *result;
}


//data-based deconcat
inline a3_SpatialPose a3spatialPoseDOpDeconcat(a3_SpatialPose pose0, a3_SpatialPose pose1)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpDeconcat(result, &pose0, &pose1);

	return *result;
}

//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpIdentity(a3_HierarchyPose* pose_out, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpIdentity(&pose_out->spatialPose[i]);
	}

	// done
	return pose_out;
}

// pointer-based init operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpInit(a3_HierarchyPose* pose_out, a3vec3 const* orientations, a3vec3 const* scales, a3vec3 const* translations, const a3ui32 nodeCount)
{
	if (pose_out)
	{
		for (a3ui32 i = 0; i < nodeCount; i++)
		{
			a3spatialPoseOpInit(&pose_out->spatialPose[i], scales[i], orientations[i], translations[i]);
		}

		return pose_out;
	}

}

// pointer-based nearest operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_0, a3_HierarchyPose* const pose_1, const a3real u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpNearest(&pose_out->spatialPose[i], &pose_0->spatialPose[i], &pose_1->spatialPose[i], u);
	}

	return pose_out;
}

// pointer-based LERP operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpLERP(&pose_out->spatialPose[i], &pose0->spatialPose[i], &pose1->spatialPose[i], u);
	}

	// done
	return pose_out;
}

// pointer-based negate operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpNegate(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_in, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpNegate(&pose_out->spatialPose[i], &pose_in->spatialPose[i]);
	}

	// done
	return pose_out;
}

// pointer-based concat operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpConcat(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose0, a3_HierarchyPose* const pose1, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpConcat(&pose_out->spatialPose[i], &pose0->spatialPose[i], &pose1->spatialPose[i]);
	}

	// done
	return pose_out;
}

// pointer-based deconcat operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpDeconcat(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose0, a3_HierarchyPose* const pose1, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpDeconcat(&pose_out->spatialPose[i], &pose0->spatialPose[i], &pose1->spatialPose[i]);
	}

	return pose_out;
}

// pointer-based cubic operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpCubic(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_Prev, a3_HierarchyPose* const pose_0, a3_HierarchyPose* const pose_1, a3_HierarchyPose* const pose_Next, const a3ui32 nodeCount, const a3real u)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpCubic(&pose_out->spatialPose[i], &pose_Prev->spatialPose[i], &pose_0->spatialPose[i], &pose_1->spatialPose[i], &pose_Next->spatialPose[i], u);
	}

	// done
	return pose_out;
}

// pointer-based copy operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpCopy(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_in, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpCopy(&pose_out->spatialPose[i], &pose_in->spatialPose[i]);
	}

	return pose_out;
}

// pointer-based triangular lerp operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpTriangularLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose0, a3_HierarchyPose* const pose1, a3_HierarchyPose* const pose2, a3real const u1, a3real const u2, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpTriangularLERP(&pose_out->spatialPose[i], &pose0->spatialPose[i], &pose1->spatialPose[i], &pose2->spatialPose[i], u1, u2);
	}

	return pose_out;
}

// pointer-based binearest operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpBiNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose00, a3_HierarchyPose* const pose01, a3_HierarchyPose* const pose10, a3_HierarchyPose* const pose11, a3real const u0, a3real const u1, a3real const u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpBiNearest(&pose_out->spatialPose[i], &pose00->spatialPose[i], &pose01->spatialPose[i], &pose10->spatialPose[i], &pose11->spatialPose[i], u0, u1, u);
	}

	return pose_out;
}

// pointer-based bilerp operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpBiLerp(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose00, a3_HierarchyPose* const pose01, a3_HierarchyPose* const pose10, a3_HierarchyPose* const pose11, a3real const u0, a3real const u1, a3real const u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpBiLerp(&pose_out->spatialPose[i], &pose00->spatialPose[i], &pose01->spatialPose[i], &pose10->spatialPose[i], &pose11->spatialPose[i], u0, u1, u);
	}

	return pose_out;
}

// pointer-based bicubic operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpBiCubic(a3_HierarchyPose* pose_out,
	a3_HierarchyPose* const pose_prev00, a3_HierarchyPose* const pose_prev01,
	a3_HierarchyPose* const pose_prev10, a3_HierarchyPose* const pose_prev11,
	a3_HierarchyPose* const pose00, a3_HierarchyPose* const pose01,
	a3_HierarchyPose* const pose10, a3_HierarchyPose* const pose11,
	a3_HierarchyPose* const pose00_2, a3_HierarchyPose* const pose20,
	a3_HierarchyPose* const pose02, a3_HierarchyPose* const pose22,
	a3_HierarchyPose* const pose_next00, a3_HierarchyPose* const pose_next01,
	a3_HierarchyPose* const pose_next10, a3_HierarchyPose* const pose_next11,
	a3real const u_prev, a3real const u0, a3real const u1, a3real const u_next, a3real const u, a3ui32 const nodeCount)
{

	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpBiCubic(&pose_out->spatialPose[i], &pose_prev00->spatialPose[i], &pose_prev01->spatialPose[i], &pose_prev10->spatialPose[i], &pose_prev11->spatialPose[i],
			&pose00->spatialPose[i], &pose01->spatialPose[i], &pose10->spatialPose[i], &pose11->spatialPose[i],
			&pose00_2->spatialPose[i], &pose20->spatialPose[i], &pose02->spatialPose[i], &pose22->spatialPose[i],
			&pose_next00->spatialPose[i], &pose_next01->spatialPose[i], &pose_next10->spatialPose[i], &pose_next11->spatialPose[i],
			u_prev, u0, u1, u_next, u);
	}

	return pose_out;
}

// pointer-based scale operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpScale(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_in, a3real const u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpScale(&pose_out->spatialPose[i], &pose_in->spatialPose[i], u);
	}

	return pose_out;
}

// pointer-based const operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpConst(a3_HierarchyPose* pose_inout)
{
	return pose_inout;
}

inline a3_HierarchyPose* a3hierarchyPoseOpEaseInOut(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose0, a3_HierarchyPose* const pose1, const a3real u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpEaseInOut(&pose_out->spatialPose[i], &pose0->spatialPose[i], &pose1->spatialPose[i], u);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpConvert(a3_HierarchyPose* pose_inout, const a3_SpatialPoseEulerOrder order, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpConvert(&pose_inout->spatialPose[i], order);
	}

	return pose_inout;
}
//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_INL
#endif	// __ANIMAL3D_HIERARCHYSTATEBLEND_H
