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

/*
	Animation Framework Addons by Scott Dagen
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
	if (clipCtrl_out != NULL)
	{
		memcpy(clipCtrl_out->name, ctrlName, a3keyframeAnimation_nameLenMax);

		clipCtrl_out->clipPool = clipPool;
		clipCtrl_out->clipIndex = clipIndex_pool; //index within pool

		clipCtrl_out->clipTime = 0;
		clipCtrl_out->clipParameter = 0;

		//set keyframe info, including grabbing the first keyframe index
		clipCtrl_out->keyframeIndex = clipCtrl_out->clipPool->clipArray[clipCtrl_out->clipIndex].firstKeyframeIndex;
		clipCtrl_out->keyframeTime = 0;
		clipCtrl_out->keyframeParameter = 0;

		clipCtrl_out->playbackDir = 0;
		clipCtrl_out->speedMod = 1;
		return 1;
	}
	return -1; //if it failed to malloc, return -1.
}


// Evaluate controller
a3i32 a3clipControllerEvaluate(a3_ClipController const* clipCtrl, a3_Sample* sample_out)
{
	if (clipCtrl && clipCtrl->clipPool && sample_out)
	{
		 // 0: No interpolation - step function
		 *sample_out = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].keyframes->keyframeArray[clipCtrl->keyframeIndex].sample;

		 // 0: step: return current keyframe sample
		 // 1: nearest: (u is keyframeParameter) - if (u < 0.5) return currentKeyframe; else return nextKeyframe
		 // 2: lerp: k0 + (k1 - k0) * u
		 // 3: Catmull-Rom: CatmullRom(kp, k0, k1, k2, u) (kp is previous)
		 // 4: Cubic-Hermite: Hermite(k0, m0, k1, m1, u) = Hermite(k0, h0 - k0, k1, h1 - k1, u); (using rate-of-change as m0, m1)

		 // 1: nearest
		 // *sample_out = clipCtrl->keyframeParameter < a3real_half
		 //					? clipCtrl->keyframe0Ptr->sample
		 //					: clipCtrl->keyframe1Ptr->sample;

		 // 2: lerp
		 sample_out->time = clipCtrl->keyframeTime;
		 sample_out->value = a3lerp(0, 0, 1); //a3lerp(clipCtrl->keyframe0Ptr->sample.value, clipCtrl->keyframe1Ptr->sample.value, clipCtrl->keyframeParam)


		 // 3: Catmull-Rom
		 // sample_out->time = clipCtrl->keyframeTime;
		 // sample_out->value = a3CatmullRom(kp->sample.value, k0->sample.value, k1->sample.value, k2->sample.value, clipCtrl->keyframeParameter)


		 // 4: Hermite
		 // a3HermiteControl(k0->sample.value, k1->sample.value, k0->handle.value, k1->handle.value, clipCtrl->keyframeParam);

		 return clipCtrl->keyframeIndex;
	}

	return -1;
}

//-----------------------------------------------------------------------------
