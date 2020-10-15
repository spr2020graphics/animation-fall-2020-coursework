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
	
	a3_HierarchyStateBlend.h
	Hierarchy blend operations.
*/

#ifndef __ANIMAL3D_HIERARCHYSTATEBLEND_H
#define __ANIMAL3D_HIERARCHYSTATEBLEND_H


#include "a3_HierarchyState.h"

#include "a3_Kinematics.h"


#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus

#endif	// __cplusplus
	

//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for single spatial pose
a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out);

// init with values
a3_SpatialPose* a3spatialPoseOpInit(a3_SpatialPose* pose_out, a3vec3 scale, a3vec3 orientation, a3vec3 translation);

a3_SpatialPose* a3spatialPoseOpNearest(a3_SpatialPose* spatialPose_out, a3_SpatialPose* spatialPose_0, a3_SpatialPose* spatialPose_1, const a3real u);

// pointer-based LERP operation for single spatial pose
a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u);

// pointer-based cubic lerp operation for spatial poses
a3_SpatialPose* a3spatialPoseOpCubic(a3_SpatialPose* spatialPose_out, a3_SpatialPose const* spatialPose_Prev, a3_SpatialPose const* spatialPose_0, a3_SpatialPose const* spatialPose_1, a3_SpatialPose const* spatialPose_Next, const a3real u);

// pointer-based triangular lerp operation for spatial poses
a3_SpatialPose* a3spatialPoseOpTriangularLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose2, a3real const u1, a3real const u2);

a3_SpatialPose* a3spatialPoseOpBiNearest(a3_SpatialPose* pose_out, a3_SpatialPose const* pose00, a3_SpatialPose const* pose01, a3_SpatialPose const* pose10, a3_SpatialPose const* pose11, a3real const u0, a3real const u1, a3real const u);

a3_SpatialPose* a3spatialPoseOpBiLerp(a3_SpatialPose* pose_out, a3_SpatialPose const* pose00, a3_SpatialPose const* pose01, a3_SpatialPose const* pose10, a3_SpatialPose const* pose11, a3real const u0, a3real const u1, a3real const u);

a3_SpatialPose* a3spatialPoseOpBiCubic(a3_SpatialPose* pose_out, 
	a3_SpatialPose const* pose_prev00, a3_SpatialPose const* pose_prev01, 
	a3_SpatialPose const* pose_prev10, a3_SpatialPose const* pose_prev11, 
	a3_SpatialPose const* pose00, a3_SpatialPose const* pose01, 
	a3_SpatialPose const* pose10, a3_SpatialPose const* pose11, 
	a3_SpatialPose const* pose12, a3_SpatialPose const* pose20, 
	a3_SpatialPose const* pose02, a3_SpatialPose const* pose22, 
	a3_SpatialPose const* pose_next00, a3_SpatialPose const* pose_next01, 
	a3_SpatialPose const* pose_next10, a3_SpatialPose const* pose_next11, 
	a3real const u_prev, a3real const u0, a3real const u1, a3real const u_next, a3real u);

// pointer-based scale operator for spatial poses
a3_SpatialPose* a3spatialPoseOpScale(a3_SpatialPose* pose_out, a3_SpatialPose const* pose_in, a3real u);

// const operator for spatial poses
a3_SpatialPose* a3spatialPoseOpConst(a3_SpatialPose* pose_inout);

// pointer-based copy operation
a3_SpatialPose* a3spatialPoseOpCopy(a3_SpatialPose* pose_out, a3_SpatialPose* pose_in);

//pointer-based negate operation
a3_SpatialPose* a3spatialPoseOpNegate(a3_SpatialPose* pose_out, a3_SpatialPose* pose_in);

a3_SpatialPose* a3spatialPoseOpConcat(a3_SpatialPose* pose_out, a3_SpatialPose* pose0, a3_SpatialPose* pose1);

a3_SpatialPose* a3spatialPoseOpDeconcat(a3_SpatialPose* pose_out, a3_SpatialPose* pose0, a3_SpatialPose* pose1);
//-----------------------------------------------------------------------------

// data-based reset/identity
a3_SpatialPose a3spatialPoseDOpIdentity();

// data-based constructor
a3_SpatialPose a3spatialPoseDOpInit(const a3vec3 scale, const a3vec3 orientation, const a3vec3 translation);

a3_SpatialPose a3spatialPoseDOpNearest(a3_SpatialPose* spatialPose_0, a3_SpatialPose* spatialPose_1, const a3real u);

// data-based LERP
a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u);

// data-based cubic lerp
a3_SpatialPose a3spatialPoseDOpCubic(a3_SpatialPose spatialPose_Prev, a3_SpatialPose spatialPose_0, a3_SpatialPose spatialPose_1, a3_SpatialPose spatialPose_Next, const a3real u);

// data-based triangular lerp
a3_SpatialPose a3spatialPoseDOpTriangularLERP(a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose2, a3real const u1, a3real const u2);

a3_SpatialPose a3spatialPoseDOpBiNearest(a3_SpatialPose const* pose00, a3_SpatialPose const* pose01, a3_SpatialPose const* pose10, a3_SpatialPose const* pose11, a3real const u0, a3real const u1, a3real const u);

a3_SpatialPose a3spatialPoseDOpBiLerp(a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose2, a3_SpatialPose const* pose3, a3real const u0, a3real const u1, a3real const u);

a3_SpatialPose* a3spatialPoseDOpBiCubic(
	a3_SpatialPose const* pose_prev00, a3_SpatialPose const* pose_prev01,
	a3_SpatialPose const* pose_prev10, a3_SpatialPose const* pose_prev11,
	a3_SpatialPose const* pose00, a3_SpatialPose const* pose01,
	a3_SpatialPose const* pose10, a3_SpatialPose const* pose11,
	a3_SpatialPose const* pose12, a3_SpatialPose const* pose20,
	a3_SpatialPose const* pose02, a3_SpatialPose const* pose22,
	a3_SpatialPose const* pose_next00, a3_SpatialPose const* pose_next01,
	a3_SpatialPose const* pose_next10, a3_SpatialPose const* pose_next11,
	a3real const u_prev, a3real const u0, a3real const u1, a3real const u_next, a3real u);

// data-based scaling operator for spatial poses
a3_SpatialPose a3spatialPoseDOpScale(a3_SpatialPose const* pose_in, a3real u);

// data-based negate
a3_SpatialPose a3spatialPoseDOpNegate(a3_SpatialPose pose_in);

a3_SpatialPose a3spatialPoseDOpConst(a3_SpatialPose pose_in);

a3_SpatialPose a3spatialPoseDOpCopy(a3_SpatialPose pose_in);

a3_SpatialPose a3spatialPoseDOpConcat(a3_SpatialPose pose0, a3_SpatialPose pose1);

a3_SpatialPose a3spatialPoseDOpDeconcat(a3_SpatialPose* pose0, a3_SpatialPose* pose1);
//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for hierarchical pose
a3_HierarchyPose* a3hierarchyPoseOpIdentity(a3_HierarchyPose* pose_out, const a3ui32 nodeCount);

// pointer-based constructor operation for hierarchical pose
a3_HierarchyPose* a3hierarchyPoseOpInit(a3_HierarchyPose* pose_out, a3vec3 const* orientations, a3vec3 const* scales, a3vec3 const* translations, const a3ui32 nodeCount);

a3_HierarchyPose* a3hierarchyPoseOpNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose_0, a3_HierarchyPose* pose_1, const a3real u, const a3ui32 nodeCount);

// pointer-based LERP operation for hierarchical pose
a3_HierarchyPose* a3hierarchyPoseOpLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u, const a3ui32 nodeCount);

// pointer-based cubic lerp operation for hierarchical poses
a3_HierarchyPose* a3hierarchyPoseOpCubic(a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_Prev, const a3_HierarchyPose* pose_0, const a3_HierarchyPose* pose_1, const a3_HierarchyPose* pose_Next, const a3ui32 nodeCount, const a3real u);

// pointer-based triangular lerp operation for hierarchy poses
a3_HierarchyPose* a3hierarchyPoseOpTriangularLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3real const u1, a3real const u2, const a3ui32 nodeCount);

a3_HierarchyPose* a3hierarchyPoseOpBiNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose00, a3_HierarchyPose const* pose01, a3_HierarchyPose const* pose10, a3_HierarchyPose const* pose11, a3real const u0, a3real const u1, a3real const u);

a3_HierarchyPose* a3hierarchyPoseOpBiLerp(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3_HierarchyPose const* pose3, a3real const u0, a3real const u1, a3real const u, const a3ui32 nodeCount);

a3_HierarchyPose* a3hierarchyPoseOpBiCubic(a3_HierarchyPose* pose_out,
	a3_HierarchyPose const* pose_prev00, a3_HierarchyPose const* pose_prev01,
	a3_HierarchyPose const* pose_prev10, a3_HierarchyPose const* pose_prev11,
	a3_HierarchyPose const* pose00, a3_HierarchyPose const* pose01,
	a3_HierarchyPose const* pose10, a3_HierarchyPose const* pose11,
	a3_HierarchyPose const* pose12, a3_HierarchyPose const* pose20,
	a3_HierarchyPose const* pose02, a3_HierarchyPose const* pose22,
	a3_HierarchyPose const* pose_next00, a3_HierarchyPose const* pose_next01,
	a3_HierarchyPose const* pose_next10, a3_HierarchyPose const* pose_next11,
	a3real const u_prev, a3real const u0, a3real const u1, a3real const u_next, a3real u);

// pointer-based scale operation for hierarchy poses
a3_HierarchyPose* a3hierarchyPoseOpScale(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose_in, a3real const u, const a3ui32 nodeCount);

// pointer-based negate operation for hierarchical poses
a3_HierarchyPose* a3hierarchyPoseOpNegate(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose_in, const a3ui32 nodeCount);

a3_HierarchyPose* a3hierarchyPoseOpConcat(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose0, a3_HierarchyPose* pose1, const a3ui32 nodeCount);

a3_HierarchyPose* a3hierarchyPoseOpDeconcat(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose0, a3_HierarchyPose* pose1, const a3ui32 nodeCount);

a3_HierarchyPose* a3hierarchyPoseOpCopy(a3_HierarchyPose* pose_out, a3_HierarchyPose* pose_in, const a3ui32 nodeCount);

a3_HierarchyPose* a3hierarchyPoseOpConst(a3_HierarchyPose* pose_inout);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_HierarchyStateBlend.inl"


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_H
