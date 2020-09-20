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
	
	a3_SpatialPose.c
	Implementation of spatial pose.
*/

#include "../a3_SpatialPose.h"
#include <stdlib.h>


//-----------------------------------------------------------------------------

a3i32 a3spatialPoseInitDefault(a3_SpatialPose* pose)
{
	if (!pose)
	{
		pose = calloc(1, sizeof(pose));
	}
	if (pose)
	{
		pose->xPos = 0.0f;
		pose->yPos = 0.0f;
		pose->zPos = 0.0f;

		pose->xRot = 0.0f;
		pose->yRot = 0.0f;
		pose->zRot = 0.0f;

		pose->xScale = 0.0f;
		pose->yScale = 0.0f;
		pose->zScale = 0.0f;
	}
	else
	{
		return -1;
	}

	return 1;
}

a3i32 a3spatialPoseInit(a3_SpatialPose* pose,
	a3real xPos, a3real yPos, a3real zPos,
	a3real xRot, a3real yRot, a3real zRot,
	a3real xScale, a3real yScale, a3real zScale)
{
	if (!pose)
	{
		pose = calloc(1, sizeof(pose));
	}
	if (pose)
	{
		pose->xPos = xPos;
		pose->yPos = yPos;
		pose->zPos = zPos;

		pose->xRot = xRot;
		pose->yRot = yRot;
		pose->zRot = zRot;

		pose->xScale = xScale;
		pose->yScale = yScale;
		pose->zScale = zScale;
	}
	else
	{
		return -1;
	}

	return 1;
}

//-----------------------------------------------------------------------------
