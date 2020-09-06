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
	
	a3_KeyframeAnimation.inl
	Inline definitions for keyframe animation.
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATION_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATION_INL
#define __ANIMAL3D_KEYFRAMEANIMATION_INL


//-----------------------------------------------------------------------------

// calculate clip duration as sum of keyframes' durations
inline a3i32 a3clipCalculateDuration(a3_Clip* clip)
{
	for (a3ui32 frame = clip->firstKeyframeIndex; frame <= clip->lastKeyframeIndex; frame++)
	{
		clip->duration += clip->keyframes->keyframeArray[frame].duration;
	}
	if (clip->duration == 0)
	{
		clip->duration = 1;
		clip->durInv = 1;
		return -1;
	}
	else
	{
		clip->durInv = 1.0f / clip->duration;
	}
	return 1;
}

// calculate keyframes' durations by distributing clip's duration
inline a3i32 a3clipDistributeDuration(a3_Clip* clip, const a3real newClipDuration)
{
	if (newClipDuration == 0)
	{
		return -1;
	}
	a3clipCalculateDuration(clip);
	a3f32 durationRatio = newClipDuration / clip->duration;
	for (a3ui32 frame = clip->firstKeyframeIndex; frame <= clip->lastKeyframeIndex; frame++)
	{
		a3f32 cachedDur = clip->keyframes->keyframeArray[frame].duration;
		a3f32 newDur = cachedDur * durationRatio;
		clip->keyframes->keyframeArray[frame].duration = newDur;
		clip->keyframes->keyframeArray[frame].durInv = 1.0f / newDur;
	}
	return 1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATION_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATION_H