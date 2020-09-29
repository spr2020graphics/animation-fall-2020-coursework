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
	
	a3_HierarchyState.inl
	Implementation of inline transform hierarchy state operations.
*/

/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
*/


#ifdef __ANIMAL3D_HIERARCHYSTATE_H
#ifndef __ANIMAL3D_HIERARCHYSTATE_INL
#define __ANIMAL3D_HIERARCHYSTATE_INL


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

// get offset to hierarchy pose in contiguous set
inline a3i32 a3hierarchyPoseGroupGetPoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex)
{
	if (poseGroup && poseGroup->hierarchy)
		return (poseIndex * poseGroup->hierarchy->numNodes);
	return -1;
}

// get offset to single node pose in contiguous set
inline a3i32 a3hierarchyPoseGroupGetNodePoseOffsetIndex(const a3_HierarchyPoseGroup *poseGroup, const a3ui32 poseIndex, const a3ui32 nodeIndex)
{
	if (poseGroup && poseGroup->hierarchy)
		return (poseIndex * poseGroup->hierarchy->numNodes + nodeIndex);
	return -1;
}

//-----------------------------------------------------------------------------

// reset full hierarchy pose
inline a3i32 a3hierarchyPoseReset(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount)
{
	if (pose_inout && nodeCount)
	{
		for (a3ui32 i = 0; i < nodeCount; i++)
		{
			a3spatialPoseReset(pose_inout->spatialPose + i);
		}
		return 1;
	}
	return -1;
}

// convert full hierarchy pose to hierarchy transforms
inline a3i32 a3hierarchyPoseConvert(const a3_HierarchyPose* pose_inout, const a3ui32 nodeCount, const a3_SpatialPoseChannel* channel, const a3_SpatialPoseEulerOrder order)
{
	if (pose_inout && nodeCount)
	{
		for (a3ui32 i = 0; i < nodeCount; i++)
		{
			a3spatialPoseConvert(&pose_inout->spatialPose[i].transform, &pose_inout->spatialPose[i], channel[i], order);
		}

		return 1;
	}
	return -1;
}

// copy full hierarchy pose
inline a3i32 a3hierarchyPoseCopy(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_in, const a3ui32 nodeCount)
{
	if (pose_out && pose_in && nodeCount)
	{
		for (a3ui32 i = 0; i < nodeCount; i++)
		{
			a3spatialPoseCopy(&pose_out->spatialPose[i], &pose_in->spatialPose[i]);
		}

		return 1;
	}
	return -1;
}

//concat each spatial pose within the hierarchy poses, output to pose_out
inline a3i32 a3hierarchyPoseConcat(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_lhs, const a3_HierarchyPose* pose_rhs, const a3ui32 nodeCount)
{
	if (pose_out && pose_lhs && pose_rhs)
	{
		for (a3ui32 i = 0; i < nodeCount; i++)
		{
			a3spatialPoseConcat(&pose_out->spatialPose[i], &pose_lhs->spatialPose[i], &pose_rhs->spatialPose[i], false);
		}

		return 1;
	}

	return -1;
}

//lerp each spatial pose within the hierarchy poses, output to pose_out
inline a3i32 a3hierarchyPoseLerp(const a3_HierarchyPose* pose_out, const a3_HierarchyPose* pose_0, const a3_HierarchyPose* pose_1, const a3ui32 nodeCount, const a3real u)
{
	if (pose_out && pose_0 && pose_1)
	{
		for (a3ui32 i = 0; i < nodeCount; i++)
		{
			a3spatialPoseLerp(&pose_out->spatialPose[i], &pose_0->spatialPose[i], &pose_1->spatialPose[i], u);
		}

		return 1;
	}

	return -1;
}

//-----------------------------------------------------------------------------

// update inverse object-space matrices
inline a3i32 a3hierarchyStateUpdateObjectInverse(const a3_HierarchyState *state)
{
	return -1;
}


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATE_INL
#endif	// __ANIMAL3D_HIERARCHYSTATE_H
