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
	
	a3_HierarchyState.h
	Hierarchy transformation state.
*/

/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
*/

#ifndef __ANIMAL3D_HIERARCHYSTATE_H
#define __ANIMAL3D_HIERARCHYSTATE_H


// A3 hierarchy
#include "a3_Hierarchy.h"

// A3 spatial pose
#include "a3_SpatialPose.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_HierarchyPose			a3_HierarchyPose;
typedef struct a3_HierarchyPoseGroup	a3_HierarchyPoseGroup;
typedef struct a3_HierarchyState		a3_HierarchyState;
#endif	// __cplusplus
	

//-----------------------------------------------------------------------------

// single pose for a collection of nodes
// makes algorithms easier to keep this as a separate data type
struct a3_HierarchyPose
{
	a3_SpatialPose* spatialPose;
};


// pose group
struct a3_HierarchyPoseGroup
{
	// pointer to hierarchy. This has the nodes, which are labels and indices for the hierarchy poses
	const a3_Hierarchy* hierarchy;

	// Similar to keyframePool, this is just an array of spatial poses associated with the hierarchy and its poses
	a3_SpatialPose* spatialPosePool; //contains all the SPs from the below data structure

	// Array of hierarchy poses to organize above spatial poses
	a3_HierarchyPose* hierarchyPosePool; //array of HP {array of SP}

	// Describes transform channels used by each node in the hierarchy
	a3_SpatialPoseChannel* channels;

	// Euler order flag for concatenation order or orientation channels
	a3_SpatialPoseEulerOrder* eulerOrders;

	// number of spatial poses
	a3ui32 spatialPoseCount;

	// number of hierarchical poses
	a3ui32 poseCount;
};


// hierarchy state structure
struct a3_HierarchyState
{
	// pointer to hierarchy
	const a3_Hierarchy* hierarchy;

	// A pose representing the animated pose at the current time (e.g a temp pose while lerping to the next pose)
	a3_HierarchyPose sampleHPose[1];

	// A pose representing each node's transformation relative to its parent
	a3_HierarchyPose localHPose[1];

	// A pose representing each node's transformation relative to the root's parent space
	a3_HierarchyPose objectHPose[1];
};
	

//-----------------------------------------------------------------------------

// initialize pose set given an initialized hierarchy and key pose count
a3i32 a3hierarchyPoseGroupCreate(a3_HierarchyPoseGroup *poseGroup_out, const a3_Hierarchy *hierarchy, const a3ui32 poseCount);

// release pose set
a3i32 a3hierarchyPoseGroupRelease(a3_HierarchyPoseGroup *poseGroup);

// get offset to hierarchy pose in contiguous set
a3i32 a3hierarchyPoseGroupGetPoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex);

// get offset to single node pose in contiguous set
a3i32 a3hierarchyPoseGroupGetNodePoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex, const a3ui32 nodeIndex);


//-----------------------------------------------------------------------------

// reset full hierarchy pose
a3i32 a3hierarchyPoseReset(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount);

// convert full hierarchy pose to hierarchy transforms
a3i32 a3hierarchyPoseConvert(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount, const a3_SpatialPoseChannel* channel, const a3_SpatialPoseEulerOrder* orders);

// copy full hierarchy pose. Also works as a step interpolation as it overrides the previous.
a3i32 a3hierarchyPoseCopy(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_in, const a3ui32 nodeCount);

// concat a pose with another
a3i32 a3hierarchyPoseConcat(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_lhs, const a3_HierarchyPose* pose_rhs, const a3ui32 nodeCount);

// lerp between poses
a3i32 a3hierarchyPoseLerp(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_0, const a3_HierarchyPose* pose_1, const a3ui32 nodeCount, const a3real u);

//find the nearest of two poses
a3i32 a3hierarchyPoseNearest(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_0, const a3_HierarchyPose* pose_1, const a3ui32 nodeCount, const a3real u);

// lerp between poses via catmull-rom
a3i32 a3hierarchyPoseCatRom(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_Prev, const a3_HierarchyPose* pose_0, const a3_HierarchyPose* pose_1, const a3_HierarchyPose* pose_Next, const a3ui32 nodeCount, const a3real u);

// initializes a hierarchy pose
a3i32 a3hierarchyPoseInit(a3_HierarchyPose* pose, a3ui32 numPoses);
//-----------------------------------------------------------------------------

// initialize hierarchy state given an initialized hierarchy
a3i32 a3hierarchyStateCreate(a3_HierarchyState *state_out, const a3_Hierarchy *hierarchy);

// release hierarchy state
a3i32 a3hierarchyStateRelease(a3_HierarchyState *state);

// update inverse object-space matrices
a3i32 a3hierarchyStateUpdateObjectInverse(const a3_HierarchyState *state);


//-----------------------------------------------------------------------------

// load HTR file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadHTR(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath);

// load BVH file, read and store complete pose group and hierarchy
a3i32 a3hierarchyPoseGroupLoadBVH(a3_HierarchyPoseGroup* poseGroup_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_HierarchyState.inl"


#endif	// !__ANIMAL3D_HIERARCHYSTATE_H
