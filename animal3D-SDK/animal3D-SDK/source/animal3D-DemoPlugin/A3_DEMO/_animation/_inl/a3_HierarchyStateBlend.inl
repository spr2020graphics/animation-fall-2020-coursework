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


//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out)
{
	// ...
	pose_out->transform = a3mat4_identity;
	pose_out->orientation = a3vec3_zero;
	pose_out->scale = a3vec3_one;
	pose_out->position = a3vec3_zero;
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
	//a3spatialPoseConvert(pose_out->transform.m, pose_out, a3poseChannel_translate_xyz | a3poseChannel_orient_xyz | a3poseChannel_scale_xyz, 0); //how to handle different euler orders?
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpCopy(a3_SpatialPose* pose_out, a3_SpatialPose* pose_in)
{
	memcpy(pose_out, pose_in, sizeof(a3_SpatialPose));
	return pose_out; //there's no reason to do anything other than this, it's a direct bit-for-bit copy
}

// pointer-based LERP operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u)
{
	pose_out->transform = a3mat4_identity;
	// ...
	a3real3Slerp(pose_out->orientation.v, pose0->orientation.v, pose1->orientation.v, u);
	a3real3Lerp(pose_out->scale.v, pose0->scale.v, pose1->scale.v, u);
	a3real3Lerp(pose_out->position.v, pose0->position.v, pose1->position.v, u);

	//a3spatialPoseConvert(pose_out->transform.m, pose_out, a3poseChannel_translate_xyz | a3poseChannel_orient_xyz | a3poseChannel_scale_xyz, 0); //how to handle different euler orders?
	// done
	return pose_out;
}


//-----------------------------------------------------------------------------

// data-based reset/identity
inline a3_SpatialPose a3spatialPoseDOpIdentity()
{
	a3_SpatialPose const result = { a3mat4_identity, a3vec3_zero, a3vec3_one, a3vec3_zero };
	return result;
}

inline a3_SpatialPose a3spatialPoseDOpInit(const a3vec3 scale, const a3vec3 orientation, const a3vec3 translation)
{
	a3_SpatialPose result = { 0 };

	a3spatialPoseOpInit(&result, scale, orientation, translation);

	return result;
}

// data-based LERP
inline a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u)
{
	a3_SpatialPose result = { 0 };
	
	a3spatialPoseOpLERP(&result, &pose0, &pose1, u);

	// done
	return result;
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


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_INL
#endif	// __ANIMAL3D_HIERARCHYSTATEBLEND_H
