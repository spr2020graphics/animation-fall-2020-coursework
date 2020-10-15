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


//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out)
{
	// ...
	pose_out->transform = a3mat4_identity;
	pose_out->orientation = a3vec3_zero;
	pose_out->scale = a3vec3_one;
	pose_out->position = a3vec3_zero;
	pose_out->rotation = a3vec4_w;
	// done
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpInit(a3_SpatialPose* pose_out, a3vec3 scale, a3vec3 orientation, a3vec3 translation)
{
	pose_out->transform = a3mat4_identity;
	pose_out->scale = scale;
	orientation.x = (a3real) fmod(orientation.x, 360.0f);
	orientation.y = (a3real) fmod(orientation.y, 360.0f);
	orientation.z = (a3real) fmod(orientation.z, 360.0f);
	pose_out->orientation = orientation;
	pose_out->position = translation;
	a3quatSetEulerXYZ(pose_out->rotation.v, orientation.x, orientation.y, orientation.z);

	//a3spatialPoseConvert(pose_out->transform.m, pose_out, a3poseChannel_translate_xyz | a3poseChannel_orient_xyz | a3poseChannel_scale_xyz, 0); //how to handle different euler orders?
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpNearest(a3_SpatialPose* spatialPose_out, a3_SpatialPose* spatialPose_0, a3_SpatialPose* spatialPose_1, const a3real u)
{
	if (u < 0.5f)
	{
		spatialPose_out->orientation = spatialPose_0->orientation;
		spatialPose_out->scale = spatialPose_0->scale;
		spatialPose_out->position = spatialPose_0->position;
	}
	else
	{
		spatialPose_out->orientation = spatialPose_1->orientation;
		spatialPose_out->scale = spatialPose_1->scale;
		spatialPose_out->position = spatialPose_1->position;
	}

	return spatialPose_out;
}

inline a3_SpatialPose* a3spatialPoseOpCopy(a3_SpatialPose* pose_out, a3_SpatialPose* pose_in)
{
	*pose_out = *pose_in;
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpConst(a3_SpatialPose* pose_inout)
{
	return pose_inout;
}

// pointer-based LERP operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u)
{
	pose_out->transform = a3mat4_identity;
	// ...
	a3real3Lerp(pose_out->orientation.v, pose0->orientation.v, pose1->orientation.v, u);
	a3real3Lerp(pose_out->scale.v, pose0->scale.v, pose1->scale.v, u);
	a3real3Lerp(pose_out->position.v, pose0->position.v, pose1->position.v, u);
	a3real4Slerp(pose_out->rotation.v, pose0->rotation.v, pose1->rotation.v, u);

	//a3spatialPoseConvert(pose_out->transform.m, pose_out, a3poseChannel_translate_xyz | a3poseChannel_orient_xyz | a3poseChannel_scale_xyz, 0); //how to handle different euler orders?
	// done
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpCubic(a3_SpatialPose* spatialPose_out, a3_SpatialPose const* spatialPose_Prev, a3_SpatialPose const* spatialPose_0, a3_SpatialPose const* spatialPose_1, a3_SpatialPose const* spatialPose_Next, const a3real u)
{
	// still need to account for quaternion lerping


	a3real3CatmullRom(spatialPose_out->orientation.v, spatialPose_Prev->orientation.v, spatialPose_0->orientation.v, spatialPose_1->orientation.v, spatialPose_Next->orientation.v, u);
	a3real3CatmullRom(spatialPose_out->scale.v, spatialPose_Prev->scale.v, spatialPose_0->scale.v, spatialPose_1->scale.v, spatialPose_Next->scale.v, u);
	a3real3CatmullRom(spatialPose_out->position.v, spatialPose_Prev->position.v, spatialPose_0->position.v, spatialPose_1->position.v, spatialPose_Next->position.v, u);
	a3real4CatmullRom(spatialPose_out->rotation.v, spatialPose_Prev->rotation.v, spatialPose_0->rotation.v, spatialPose_1->rotation.v, spatialPose_Next->rotation.v, u);

	return spatialPose_out;
}

inline a3_SpatialPose* a3spatialPoseOpTriangularLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose2, a3real const u1, a3real const u2)
{
	a3real u0 = 1 - u1 - u2;
	
	a3_SpatialPose result = a3spatialPoseDOpConcat(a3spatialPoseDOpConcat(a3spatialPoseDOpScale(pose0, u0), a3spatialPoseDOpScale(pose1, u1)), a3spatialPoseDOpScale(pose2, u2));

	a3spatialPoseOpCopy(pose_out, &result);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpBiNearest(a3_SpatialPose* pose_out, a3_SpatialPose const* pose00, a3_SpatialPose const* pose01, a3_SpatialPose const* pose10, a3_SpatialPose const* pose11, a3real const u0, a3real const u1, a3real const u)
{
	a3_SpatialPose pose0_inout = a3spatialPoseDOpNearest(pose00, pose01, u), pose1_inout = a3spatialPoseDOpNearest(pose10, pose11, u);
	a3spatialPoseNearest(pose_out, &pose0_inout, &pose1_inout, u);
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpBiLerp(a3_SpatialPose* pose_out, a3_SpatialPose const* pose00, a3_SpatialPose const* pose01, a3_SpatialPose const* pose10, a3_SpatialPose const* pose11, a3real const u0, a3real const u1, a3real const u)
{
	a3_SpatialPose result1 = a3spatialPoseDOpLERP(*pose00, *pose01, u0);
	a3_SpatialPose result2 = a3spatialPoseDOpLERP(*pose10, *pose11, u1);

	a3spatialPoseOpLERP(pose_out, &result1, &result2, u);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpBiCubic(a3_SpatialPose* pose_out, 
	a3_SpatialPose const* pose_prev00, a3_SpatialPose const* pose_prev01, 
	a3_SpatialPose const* pose_prev10, a3_SpatialPose const* pose_prev11, 
	a3_SpatialPose const* pose00, a3_SpatialPose const* pose01, 
	a3_SpatialPose const* pose10, a3_SpatialPose const* pose11, 
	a3_SpatialPose const* pose12, a3_SpatialPose const* pose20, 
	a3_SpatialPose const* pose02, a3_SpatialPose const* pose22, 
	a3_SpatialPose const* pose_next00, a3_SpatialPose const* pose_next01, 
	a3_SpatialPose const* pose_next10, a3_SpatialPose const* pose_next11, 
	a3real const u_prev, a3real const u0, a3real const u1, a3real const u_next, a3real u)
{
	a3_SpatialPose result_prev[1], result_0[1], result_1[1], result_next[1];

	a3spatialPoseOpCubic(result_prev, pose_prev00, pose00, pose12, pose_next00, u_prev);
	a3spatialPoseOpCubic(result_0, pose_prev01, pose01, pose20, pose_next01, u0);
	a3spatialPoseOpCubic(result_1, pose_prev10, pose10, pose02, pose_next10, u1);
	a3spatialPoseOpCubic(result_next, pose_prev11, pose11, pose22, pose_next11, u_next);

	a3spatialPoseOpCubic(pose_out, result_prev, result_0, result_1, result_next, u);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpScale(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_in, a3real u)
{
	a3_SpatialPose result = a3spatialPoseDOpLERP(a3spatialPoseDOpIdentity(), *pose_in, u);

	a3spatialPoseOpCopy(pose_out, &result);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpNegate(a3_SpatialPose* pose_out, a3_SpatialPose* pose_in)
{
	a3real3GetNegative(pose_out->orientation.v, pose_in->orientation.v);
	a3real3GetNegative(pose_out->position.v,	pose_in->position.v);
	a3real3GetNegative(pose_out->scale.v,		pose_in->scale.v);
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpConcat(a3_SpatialPose* pose_out, a3_SpatialPose* pose0, a3_SpatialPose* pose1)
{
	a3real3Sum(pose_out->position.v, pose0->position.v, pose1->position.v);
	a3real3Sum(pose_out->orientation.v, pose0->orientation.v, pose1->orientation.v);
	a3real3ProductComp(pose_out->scale.v, pose0->scale.v, pose1->scale.v);


	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpDeconcat(a3_SpatialPose* pose_out, a3_SpatialPose* pose0, a3_SpatialPose* pose1)
{
	a3real3Diff(pose_out->position.v, pose0->position.v, pose1->position.v);
	a3real3Diff(pose_out->orientation.v, pose0->orientation.v, pose1->orientation.v);
	a3real3QuotientComp(pose_out->scale.v, pose0->scale.v, pose1->scale.v);

	return pose_out;
}

//-----------------------------------------------------------------------------

// data-based reset/identity
inline a3_SpatialPose a3spatialPoseDOpIdentity()
{
	a3_SpatialPose const result = { a3mat4_identity, a3vec3_zero, a3vec3_one, a3vec3_zero, a3vec4_w };
	return result;
}

inline a3_SpatialPose a3spatialPoseDOpInit(const a3vec3 scale, const a3vec3 orientation, const a3vec3 translation)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpInit(result, scale, orientation, translation);

	return *result;
}

inline a3_SpatialPose a3spatialPoseDOpNearest(a3_SpatialPose* spatialPose_0, a3_SpatialPose* spatialPose_1, const a3real u)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpNearest(result, spatialPose_0, spatialPose_1, u);

	return *result;
}

// data-based LERP
inline a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u)
{
	a3_SpatialPose result[1];
	
	a3spatialPoseOpLERP(result, &pose0, &pose1, u);

	// done
	return *result;
}

inline a3_SpatialPose a3spatialPoseDOpCubic(a3_SpatialPose spatialPose_Prev, a3_SpatialPose spatialPose_0, a3_SpatialPose spatialPose_1, a3_SpatialPose spatialPose_Next, const a3real u)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpCubic(result, &spatialPose_Prev, &spatialPose_0, &spatialPose_1, &spatialPose_Next, u);

	return *result;
}

inline a3_SpatialPose a3spatialPoseDOpTriangularLERP(a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose2, a3real const u1, a3real const u2)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpTriangularLERP(result, pose0, pose1, pose2, u1, u2);

	return *result;
}

inline a3_SpatialPose a3spatialPoseDOpBiNearest(a3_SpatialPose const* pose00, a3_SpatialPose const* pose01, a3_SpatialPose const* pose10, a3_SpatialPose const* pose11, a3real const u0, a3real const u1, a3real const u)
{
	a3_SpatialPose result[1];

	return *result;
}

inline a3_SpatialPose a3spatialPoseDOpBiLerp(a3_SpatialPose const* pose00, a3_SpatialPose const* pose01, a3_SpatialPose const* pose10, a3_SpatialPose const* pose11, a3real const u0, a3real const u1, a3real const u)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpBiLerp(result, pose00, pose01, pose10, pose11, u0, u1, u);

	return *result;
}

inline a3_SpatialPose* a3spatialPoseDOpBiCubic(a3_SpatialPose const* pose_prev00, a3_SpatialPose const* pose_prev01, a3_SpatialPose const* pose_prev10, a3_SpatialPose const* pose_prev11, a3_SpatialPose const* pose00, a3_SpatialPose const* pose01, a3_SpatialPose const* pose10, a3_SpatialPose const* pose11, a3_SpatialPose const* pose12, a3_SpatialPose const* pose20, a3_SpatialPose const* pose02, a3_SpatialPose const* pose22, a3_SpatialPose const* pose_next00, a3_SpatialPose const* pose_next01, a3_SpatialPose const* pose_next10, a3_SpatialPose const* pose_next11, a3real const u_prev, a3real const u0, a3real const u1, a3real const u_next, a3real u)
{
	return NULL;
}

inline a3_SpatialPose a3spatialPoseDOpScale(a3_SpatialPose const* pose_in, a3real u)
{
	a3_SpatialPose result = a3spatialPoseDOpLERP(a3spatialPoseDOpIdentity(), *pose_in, u);

	return result;
}

inline a3_SpatialPose a3spatialPoseDOpNegate(a3_SpatialPose pose_in)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpNegate(result, &pose_in);

	// done
	return *result;
}

inline a3_SpatialPose a3spatialPoseDOpConst(a3_SpatialPose pose_in)
{
	return pose_in;
}

inline a3_SpatialPose a3spatialPoseDOpCopy(a3_SpatialPose pose_in)
{
	return pose_in;
}

inline a3_SpatialPose a3spatialPoseDOpConcat(a3_SpatialPose pose0, a3_SpatialPose pose1)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpConcat(result, &pose0, &pose1);

	// done
	return *result;
}

inline a3_SpatialPose a3spatialPoseDOpDeconcat(a3_SpatialPose* pose0, a3_SpatialPose* pose1)
{
	a3_SpatialPose result[1];

	a3spatialPoseOpDeconcat(result, pose0, pose1);

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

inline a3_HierarchyPose* a3hierarchyPoseOpNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose_0, a3_HierarchyPose* pose_1, const a3real u, const a3ui32 nodeCount)
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

inline a3_HierarchyPose* a3hierarchyPoseOpNegate(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose_in, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpNegate(&pose_out->spatialPose[i], &pose_in->spatialPose[i]);
	}

	// done
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpConcat(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose0, a3_HierarchyPose* pose1, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpConcat(&pose_out->spatialPose[i], &pose0->spatialPose[i], &pose1->spatialPose[i]);
	}

	// done
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpDeconcat(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose0, a3_HierarchyPose* pose1, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpDeconcat(&pose_out->spatialPose[i], &pose0->spatialPose[i], &pose1->spatialPose[i]);
	}

	return pose_out;
}


inline a3_HierarchyPose* a3hierarchyPoseOpCubic(a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_Prev, const a3_HierarchyPose* pose_0, const a3_HierarchyPose* pose_1, const a3_HierarchyPose* pose_Next, const a3ui32 nodeCount, const a3real u)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpCubic(&pose_out->spatialPose[i], &pose_Prev->spatialPose[i], &pose_0->spatialPose[i], &pose_1->spatialPose[i], &pose_Next->spatialPose[i], u);
	}

	// done
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpCopy(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose_in, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpCopy(&pose_out->spatialPose[i], &pose_in->spatialPose[i]);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpTriangularLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3real const u1, a3real const u2, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpTriangularLERP(&pose_out->spatialPose[i], &pose0->spatialPose[i], &pose1->spatialPose[i], &pose2->spatialPose[i], u1, u2);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpBiNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose00, a3_HierarchyPose const* pose01, a3_HierarchyPose const* pose10, a3_HierarchyPose const* pose11, a3real const u0, a3real const u1, a3real const u)
{
	return NULL;
}

inline a3_HierarchyPose* a3hierarchyPoseOpBiLerp(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose00, a3_HierarchyPose const* pose01, a3_HierarchyPose const* pose10, a3_HierarchyPose const* pose11, a3real const u0, a3real const u1, a3real const u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpBiLerp(&pose_out->spatialPose[i], &pose00->spatialPose[i], &pose01->spatialPose[i], &pose10->spatialPose[i], &pose11->spatialPose[i], u0, u1, u);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpBiCubic(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose_prev00, a3_HierarchyPose const* pose_prev01, a3_HierarchyPose const* pose_prev10, a3_HierarchyPose const* pose_prev11, a3_HierarchyPose const* pose00, a3_HierarchyPose const* pose01, a3_HierarchyPose const* pose10, a3_HierarchyPose const* pose11, a3_HierarchyPose const* pose12, a3_HierarchyPose const* pose20, a3_HierarchyPose const* pose02, a3_HierarchyPose const* pose22, a3_HierarchyPose const* pose_next00, a3_HierarchyPose const* pose_next01, a3_HierarchyPose const* pose_next10, a3_HierarchyPose const* pose_next11, a3real const u_prev, a3real const u0, a3real const u1, a3real const u_next, a3real u)
{
	return NULL;
}

inline a3_HierarchyPose* a3hierarchyPoseOpScale(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose_in, a3real const u, const a3ui32 nodeCount)
{
	for (a3ui32 i = 0; i < nodeCount; i++)
	{
		a3spatialPoseOpScale(&pose_out->spatialPose[i], &pose_in->spatialPose[i], u);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpConst(a3_HierarchyPose* pose_inout)
{
	return pose_inout;
}
//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_INL
#endif	// __ANIMAL3D_HIERARCHYSTATEBLEND_H
