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
	
	a3_KeyframeAnimationController.c
	Implementation of keyframe animation controller.
*/

#include "../a3_KeyframeAnimationController.h"

#include <string.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------

// initialize clip controller
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	//don't re-alloc if it already exists
	if (clipCtrl_out == NULL)
	{
		clipCtrl_out = (a3_ClipController*)malloc(sizeof(a3_ClipController));
	}

	memcpy(clipCtrl_out->name, ctrlName, a3keyframeAnimation_nameLenMax);

	clipCtrl_out->clipPool = clipPool;
	clipCtrl_out->clipIndex = clipIndex_pool; //index within pool

	clipCtrl_out->clipTime = 0.0f;
	clipCtrl_out->clipParameter = 0.0f;

	//set keyframe info, including grabbing the first keyframe index
	clipCtrl_out->keyframeIndex = clipCtrl_out->clipPool->clipArray[clipCtrl_out->clipIndex].firstKeyframeIndex;
	clipCtrl_out->keyframeTime = 0.0f;
	clipCtrl_out->keyframeParameter = 0.0f;

	clipCtrl_out->playbackDir = 0;
	clipCtrl_out->speedMod = 1;
	return 1;

}


//-----------------------------------------------------------------------------
