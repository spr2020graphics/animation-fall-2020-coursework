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
	pose_out->transformMat = a3mat4_identity;
	pose_out->rotate = a3vec4_zero;
	pose_out->scale = a3vec4_one;
	pose_out->translate = a3vec4_zero;
	pose_out->rotate = a3vec4_zero;
	pose_out->rotate.w = 1.0f;
	// done
	return pose_out;
}

// pointer-based initialization with specified values
inline a3_SpatialPose* a3spatialPoseOpInit(a3_SpatialPose* pose_out, a3vec4 scale, a3vec4 rotate, a3vec4 translation)
{
	pose_out->transformMat = a3mat4_identity;
	pose_out->scale = scale;
	rotate.x = (a3real) fmod(rotate.x, 360.0f);
	rotate.y = (a3real) fmod(rotate.y, 360.0f);
	rotate.z = (a3real) fmod(rotate.z, 360.0f);
	pose_out->rotate = rotate;
	pose_out->translate = translation;
	a3quatSetEulerXYZ(&pose_out->rotate.q, rotate.x, rotate.y, rotate.z);

	//a3spatialPoseConvert(pose_out->transformMat.m, pose_out, a3poseChannel_translate_xyz | a3poseChannel_orient_xyz | a3poseChannel_scale_xyz, 0); //how to handle different euler orders?
	return pose_out;
}

// pointer-based nearest, takes 2 controls and u
inline a3_SpatialPose* a3spatialPoseOpNearest(a3_SpatialPose* spatialPose_out, a3_SpatialPose* const spatialPose_0, a3_SpatialPose* const spatialPose_1, const a3real u)
{
	if (u < 0.5f)
	{
		spatialPose_out->rotate = spatialPose_0->rotate;
		spatialPose_out->scale = spatialPose_0->scale;
		spatialPose_out->translate = spatialPose_0->translate;
		spatialPose_out->rotate = spatialPose_0->rotate;
	}
	else
	{
		spatialPose_out->rotate = spatialPose_1->rotate;
		spatialPose_out->scale = spatialPose_1->scale;
		spatialPose_out->translate = spatialPose_1->translate;
		spatialPose_out->rotate = spatialPose_1->rotate;
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
	pose_out->transformMat = a3mat4_identity;

	a3real3Lerp(pose_out->rotate.v, pose0->rotate.v, pose1->rotate.v, u);
	a3real3Lerp(pose_out->scale.v, pose0->scale.v, pose1->scale.v, u);
	a3real3Lerp(pose_out->translate.v, pose0->translate.v, pose1->translate.v, u);

	return pose_out;
}

// pointer-based cubic, 4 controls + u
inline a3_SpatialPose* a3spatialPoseOpCubic(a3_SpatialPose* spatialPose_out, a3_SpatialPose* const spatialPose_Prev, a3_SpatialPose* const spatialPose_0, a3_SpatialPose* const spatialPose_1, a3_SpatialPose* const spatialPose_Next, const a3real u)
{
	// still need to account for quaternion lerping
	a3real3CatmullRom(spatialPose_out->rotate.v, spatialPose_Prev->rotate.v, spatialPose_0->rotate.v, spatialPose_1->rotate.v, spatialPose_Next->rotate.v, u);
	a3real3CatmullRom(spatialPose_out->scale.v, spatialPose_Prev->scale.v, spatialPose_0->scale.v, spatialPose_1->scale.v, spatialPose_Next->scale.v, u);
	a3real3CatmullRom(spatialPose_out->translate.v, spatialPose_Prev->translate.v, spatialPose_0->translate.v, spatialPose_1->translate.v, spatialPose_Next->translate.v, u);

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
	a3real3GetNegative(pose_out->rotate.v, pose_in->rotate.v);
	a3real3GetNegative(pose_out->translate.v,	pose_in->translate.v);
	pose_out->scale.x = 1.0f / pose_in->scale.x;
	pose_out->scale.y = 1.0f / pose_in->scale.y;
	pose_out->scale.z = 1.0f / pose_in->scale.z;

	return pose_out;
}

//pointer-based concat
inline a3_SpatialPose* a3spatialPoseOpConcat(a3_SpatialPose* pose_out, a3_SpatialPose* const pose0, a3_SpatialPose* const pose1)
{
	a3real3Sum(pose_out->translate.v, pose0->translate.v, pose1->translate.v);
	a3real3Sum(pose_out->rotate.v, pose0->rotate.v, pose1->rotate.v);
	a3real3ProductComp(pose_out->scale.v, pose0->scale.v, pose1->scale.v);

	return pose_out;
}

//pointer-based deconcat (subtract/divide)
inline a3_SpatialPose* a3spatialPoseOpDeconcat(a3_SpatialPose* pose_out, a3_SpatialPose* const pose0, a3_SpatialPose* const pose1)
{
	a3real3Diff(pose_out->translate.v, pose0->translate.v, pose1->translate.v);
	a3real3Diff(pose_out->rotate.v, pose0->rotate.v, pose1->rotate.v);
	a3real3QuotientComp(pose_out->scale.v, pose0->scale.v, pose1->scale.v);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpEaseInOut(a3_SpatialPose* pose_out, a3_SpatialPose* const pose0, a3_SpatialPose* const pose1, const a3real u)
{
	a3real3Bezier1(pose_out->translate.v, pose0->translate.v, pose1->translate.v, u);
	a3real3Bezier1(pose_out->rotate.v, pose0->rotate.v, pose1->rotate.v, u);
	a3real3Bezier1(pose_out->scale.v, pose0->scale.v, pose1->scale.v, u);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpConvert(a3_SpatialPose* pose_inout)
{
	//create translate matrix. There's probably an a3 call for this, but it works.
	a3mat4 mat_out = a3mat4_identity;

	a3mat4 translate = a3mat4_identity;
	a3vec4 spatial_translate;
	spatial_translate = pose_inout->translate;
	spatial_translate.w = 1;
	translate.v3 = spatial_translate;

	//create rotate matrix
	a3mat4 rotate = a3mat4_identity;
	a3mat4 xRot, yRot, zRot, tmp;

	//initialize rotate matrices
	xRot = a3mat4_identity;
	a3real4x4SetRotateX(xRot.m, (float)fmod(pose_inout->rotate.x, 360.0f));
	yRot = a3mat4_identity;
	a3real4x4SetRotateY(yRot.m, (float)fmod(pose_inout->rotate.y, 360.0f));
	zRot = a3mat4_identity;
	a3real4x4SetRotateZ(zRot.m, (float)fmod(pose_inout->rotate.z, 360.0f));
	tmp = a3mat4_identity;

	//concatenate rotate matrices using XYZ order
	a3real4x4SetRotateZYX(rotate.m, (float)fmod(pose_inout->rotate.x, 360.0f), (float)fmod(pose_inout->rotate.y, 360.0f), (float)fmod(pose_inout->rotate.z, 360.0f));

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

	a3real4x4SetReal4x4(pose_inout->transformMat.m, mat_out.m);

	return pose_inout;
}

inline a3mat4* a3OpForwardKinematics(a3mat4* const objectTransform_inout, a3_Hierarchy* const hierarchy, a3mat4* const localTransform)
{
	// this assumes that the object transform passed in is the PARENT'S object-space transform

	a3real4x4Product(objectTransform_inout->m, objectTransform_inout->m, localTransform->m);

	return objectTransform_inout;
}

inline a3mat4* a3OpInverseKinematics(a3mat4* const localTransform_inout, a3_Hierarchy* const hierarchy, a3mat4* const objectTransform)
{
	

	return localTransform_inout;
}

inline a3_SpatialPose* a3spatialPoseOpBiDirectionalScale(a3_SpatialPose* pose_out, a3_SpatialPose* const pose_in, const a3real u)
{
	a3_SpatialPose identity[1];
	a3spatialPoseOpIdentity(identity);

	if (u > 0)
	{
		// scale(pose_in)
		a3spatialPoseOpScale(pose_out, pose_in, u);
	}
	else if (u < 0)
	{
		// scale(negate(pose_in))
		a3_SpatialPose inverse[1];
		a3spatialPoseOpNegate(inverse, pose_in);
		a3spatialPoseOpScale(pose_out, inverse, u);
	}
	else
	{
		pose_out = identity;
	}

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpRevert(a3_SpatialPose* pose_inout)
{
	a3mat4* TRS = &pose_inout->transformMat;
	a3vec4 translate, scale, rotate;

	// Extract translate
	translate.x = TRS->m30;
	translate.y = TRS->m31;
	translate.z = TRS->m32;
	
	// Reset transform matrix translation
	TRS->m30 = 0;
	TRS->m31 = 0;
	TRS->m32 = 0;
	
	// Extract scale
	a3vec3 col;
	col.x = TRS->m00;
	col.y = TRS->m01;
	col.z = TRS->m02;
	scale.x = a3real3Length(col.v);

	col.x = TRS->m10;
	col.y = TRS->m11;
	col.z = TRS->m12;
	scale.y = a3real3Length(col.v);

	col.x = TRS->m20;
	col.y = TRS->m21;
	col.z = TRS->m22;
	scale.z = a3real3Length(col.v);

	// Extract rotate (these 3 blocks essentially turn the TRS into just a rotate matrix)
	TRS->m00 /= scale.x;
	TRS->m01 /= scale.x;
	TRS->m02 /= scale.x;

	TRS->m10 /= scale.y;
	TRS->m11 /= scale.y;
	TRS->m12 /= scale.y;

	TRS->m20 /= scale.z;
	TRS->m21 /= scale.z;
	TRS->m22 /= scale.z;

	a3real4x4GetEulerXYZIgnoreScale(TRS->m, &rotate.x, &rotate.y, &rotate.z);

	pose_inout->rotate = rotate;
	pose_inout->translate = translate;
	pose_inout->scale = scale;

	pose_inout->transformMat = a3mat4_identity;

	return pose_inout;
}

//-----------------------------------------------------------------------------

// data-based reset/identity
inline a3_SpatialPose a3spatialPoseDOpIdentity()
{
	a3_SpatialPose result = { a3mat4_identity, a3dualquat_identity, a3vec4_zero, a3vec4_one, a3vec4_zero, a3vec4_zero };
	result.rotate.w = 1.0f;	//ensuring this is 1, always

	return result;
}

// data-based init
inline a3_SpatialPose a3spatialPoseDOpInit(const a3vec4 scale, const a3vec4 rotate, const a3vec4 translation)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpInit(result, scale, rotate, translation);

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
		a3spatialPoseOpIdentity(&pose_out->pose[i]);
	}

	// done
	return pose_out;
}

// pointer-based init operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpInit(a3_HierarchyPose* pose_out, a3vec4 const* rotates, a3vec4 const* scales, a3vec4 const* translations, const a3ui32 nodeCount)
{
	if (pose_out)
	{
		for (a3ui32 i = 0; i < nodeCount; i++)
		{
			a3spatialPoseOpInit(&pose_out->pose[i], scales[i], rotates[i], translations[i]);
		}

		return pose_out;
	}

}

// pointer-based nearest operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_0, a3_HierarchyPose* const pose_1, const a3real u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpNearest(&pose_out->pose[i], &pose_0->pose[i], &pose_1->pose[i], u);
	}

	return pose_out;
}

// pointer-based LERP operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpLERP(&pose_out->pose[i], &pose0->pose[i], &pose1->pose[i], u);
	}

	// done
	return pose_out;
}

// pointer-based negate operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpNegate(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_in, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpNegate(&pose_out->pose[i], &pose_in->pose[i]);
	}

	// done
	return pose_out;
}

// pointer-based concat operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpConcat(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose0, a3_HierarchyPose* const pose1, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpConcat(&pose_out->pose[i], &pose0->pose[i], &pose1->pose[i]);
	}

	// done
	return pose_out;
}

// pointer-based deconcat operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpDeconcat(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose0, a3_HierarchyPose* const pose1, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpDeconcat(&pose_out->pose[i], &pose0->pose[i], &pose1->pose[i]);
	}

	return pose_out;
}

// pointer-based cubic operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpCubic(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_Prev, a3_HierarchyPose* const pose_0, a3_HierarchyPose* const pose_1, a3_HierarchyPose* const pose_Next, const a3ui32 nodeCount, const a3real u)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpCubic(&pose_out->pose[i], &pose_Prev->pose[i], &pose_0->pose[i], &pose_1->pose[i], &pose_Next->pose[i], u);
	}

	// done
	return pose_out;
}

// pointer-based copy operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpCopy(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_in, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpCopy(&pose_out->pose[i], &pose_in->pose[i]);
	}

	return pose_out;
}

// pointer-based triangular lerp operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpTriangularLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose0, a3_HierarchyPose* const pose1, a3_HierarchyPose* const pose2, a3real const u1, a3real const u2, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpTriangularLERP(&pose_out->pose[i], &pose0->pose[i], &pose1->pose[i], &pose2->pose[i], u1, u2);
	}

	return pose_out;
}

// pointer-based binearest operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpBiNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose00, a3_HierarchyPose* const pose01, a3_HierarchyPose* const pose10, a3_HierarchyPose* const pose11, a3real const u0, a3real const u1, a3real const u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpBiNearest(&pose_out->pose[i], &pose00->pose[i], &pose01->pose[i], &pose10->pose[i], &pose11->pose[i], u0, u1, u);
	}

	return pose_out;
}

// pointer-based bilerp operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpBiLerp(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose00, a3_HierarchyPose* const pose01, a3_HierarchyPose* const pose10, a3_HierarchyPose* const pose11, a3real const u0, a3real const u1, a3real const u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpBiLerp(&pose_out->pose[i], &pose00->pose[i], &pose01->pose[i], &pose10->pose[i], &pose11->pose[i], u0, u1, u);
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
		a3spatialPoseOpBiCubic(&pose_out->pose[i], &pose_prev00->pose[i], &pose_prev01->pose[i], &pose_prev10->pose[i], &pose_prev11->pose[i],
			&pose00->pose[i], &pose01->pose[i], &pose10->pose[i], &pose11->pose[i],
			&pose00_2->pose[i], &pose20->pose[i], &pose02->pose[i], &pose22->pose[i],
			&pose_next00->pose[i], &pose_next01->pose[i], &pose_next10->pose[i], &pose_next11->pose[i],
			u_prev, u0, u1, u_next, u);
	}

	return pose_out;
}

// pointer-based scale operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpScale(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_in, a3real const u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpScale(&pose_out->pose[i], &pose_in->pose[i], u);
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
		a3spatialPoseOpEaseInOut(&pose_out->pose[i], &pose0->pose[i], &pose1->pose[i], u);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpConvert(a3_HierarchyPose* pose_inout, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpConvert(&pose_inout->pose[i]);
	}

	return pose_inout;
}

inline a3_HierarchyPose* a3hierarchyPoseOpBiDirectionalScale(a3_HierarchyPose* pose_out, a3_HierarchyPose* const pose_in, const a3real u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpBiDirectionalScale(&pose_out->pose[i], &pose_in->pose[i], u);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpRevert(a3_HierarchyPose* pose_inout, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpRevert(&pose_inout->pose[i]);
	}

	return pose_inout;
}

inline a3_HierarchyPose* a3clipOpAdd(a3_HierarchyPose* pose_out, a3_HierarchyPoseGroup* const poseGroup, a3_ClipController* const controller1, a3_ClipController* const controller2)
{
	a3_HierarchyPose controller1Result[1], controller2Result[1];

	a3hierarchyPoseOpIdentity(controller1Result, poseGroup->hierarchy->numNodes);
	a3hierarchyPoseOpIdentity(controller2Result, poseGroup->hierarchy->numNodes);

	a3clipOpSampleClip(controller1Result, poseGroup, controller1);
	a3clipOpSampleClip(controller2Result, poseGroup, controller2);

	a3hierarchyPoseOpConcat(pose_out, controller1Result, controller2Result, poseGroup->hierarchy->numNodes);

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpFK(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose_in, a3_Hierarchy* hierarchy, const a3ui32 nodeCount)
{
	if (pose_out && hierarchy && nodeCount)
	{
		const a3_Hierarchy* hierarchy = hierarchy;

		//loop from part of the way through the hierarchy (or from the beginning), till the end.
		for (a3ui32 i = 0; i < hierarchy->numNodes; i++)
		{
			a3_HierarchyNode* currentNode = hierarchy->nodes + i;

			if (currentNode->parentIndex != -1)
			{
				// if we aren't the root node, our object space transform = parent object space * our local space
				a3real4x4Product(pose_out->pose[currentNode->index].transformMat.m, pose_out->pose[currentNode->parentIndex].transformMat.m, pose_in->pose[currentNode->index].transformMat.m);
			}
			else
			{
				// If we ARE the root, our object space transform is the same as our local space
				a3real4x4SetReal4x4(pose_out->pose[currentNode->index].transformMat.m, pose_in->pose[currentNode->index].transformMat.m);
			}
		}
	}
}

inline a3_HierarchyPose* a3clipOpLerp(a3_HierarchyPose* pose_out, a3_HierarchyPoseGroup* const poseGroup, a3_ClipController* const controller1, a3_ClipController* const controller2, const a3real u)
{
	a3_HierarchyPose controller1Result[1], controller2Result[1];

	a3hierarchyPoseOpIdentity(controller1Result, poseGroup->hierarchy->numNodes);
	a3hierarchyPoseOpIdentity(controller2Result, poseGroup->hierarchy->numNodes);

	a3clipOpSampleClip(controller1Result, poseGroup, controller1);
	a3clipOpSampleClip(controller2Result, poseGroup, controller2);

	a3hierarchyPoseOpLERP(pose_out, controller1Result, controller2Result, u, poseGroup->hierarchy->numNodes);

	return pose_out;
}

inline a3_HierarchyPose* a3clipOpScale(a3_HierarchyPose* pose_out, a3_HierarchyPoseGroup* const poseGroup, a3_ClipController* const controller1, const a3real u)
{
	a3_HierarchyPose clipAResult[1];
	a3hierarchyPoseOpIdentity(clipAResult, poseGroup->hierarchy->numNodes);

	a3clipOpSampleClip(clipAResult, poseGroup, controller1);

	a3hierarchyPoseOpScale(pose_out, clipAResult, u, poseGroup->hierarchy->numNodes);

	return pose_out;
}

inline a3_HierarchyPose* a3clipOpNegate(a3_HierarchyPose* pose_out, a3_HierarchyPoseGroup* const poseGroup, a3_ClipController* const controller1)
{
	a3_HierarchyPose clipAResult[1];
	a3hierarchyPoseOpIdentity(clipAResult, poseGroup->hierarchy->numNodes);

	a3clipOpSampleClip(clipAResult, poseGroup, controller1);

	a3hierarchyPoseOpNegate(pose_out, clipAResult, poseGroup->hierarchy->numNodes);

	return pose_out;
}

inline a3_HierarchyPose* a3clipOpSampleClip(a3_HierarchyPose* pose_out, a3_HierarchyPoseGroup* const poseGroup, a3_ClipController* const controller1)
{
	a3_ClipPool* pool = controller1->clipPool;

	a3hierarchyPoseOpLERP(pose_out,
		poseGroup->hpose + pool->keyframe[controller1->keyframeIndex].sampleIndex0,
		poseGroup->hpose + pool->keyframe[controller1->keyframeIndex].sampleIndex1,
		(a3f32)controller1->keyframeParam, poseGroup->hierarchy->numNodes);

	return pose_out;
}

//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_INL
#endif	// __ANIMAL3D_HIERARCHYSTATEBLEND_H
