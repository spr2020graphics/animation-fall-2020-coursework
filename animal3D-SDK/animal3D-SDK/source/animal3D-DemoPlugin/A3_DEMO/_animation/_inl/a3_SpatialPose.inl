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
	
	a3_SpatialPose.inl
	Implementation of inline spatial pose operations.
*/

/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
*/

#ifdef __ANIMAL3D_SPATIALPOSE_H
#ifndef __ANIMAL3D_SPATIALPOSE_INL
#define __ANIMAL3D_SPATIALPOSE_INL


//-----------------------------------------------------------------------------

// set rotation values for a single node pose
inline a3i32 a3spatialPoseSetRotation(a3_SpatialPose* spatialPose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees)
{
	if (spatialPose)
	{
		spatialPose->orientation.x = rx_degrees;
		spatialPose->orientation.y = ry_degrees;
		spatialPose->orientation.z = rz_degrees;

		return 1;
	}
	return -1;
}

// scale
inline a3i32 a3spatialPoseSetScale(a3_SpatialPose* spatialPose, const a3f32 sx, const a3f32 sy, const a3f32 sz)
{
	if (spatialPose)
	{
		spatialPose->scale.x = sx;
		spatialPose->scale.y = sy;
		spatialPose->scale.z = sz;

		return 1;
	}
	return -1;
}

// translation
inline a3i32 a3spatialPoseSetTranslation(a3_SpatialPose* spatialPose, const a3f32 tx, const a3f32 ty, const a3f32 tz)
{
	if (spatialPose)
	{
		spatialPose->position.x = tx;
		spatialPose->position.y = ty;
		spatialPose->position.z = tz;

		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// reset single node pose
inline a3i32 a3spatialPoseReset(a3_SpatialPose* spatialPose)
{
	if (spatialPose)
	{
		a3spatialPoseSetRotation(spatialPose, 0.0f, 0.0f, 0.0f);
		a3spatialPoseSetScale(spatialPose, 1.0f, 1.0f, 1.0f);
		a3spatialPoseSetTranslation(spatialPose, 0.0f, 0.0f, 0.0f);
		spatialPose->transform = a3mat4_identity;

		return 1;
	}
	return -1;
}

// convert single node pose to matrix. This is used BEFORE the FK. FK is just the matrix multiplication
// channels do not need to be used for this lab
inline a3i32 a3spatialPoseConvert(a3mat4* mat_out, const a3_SpatialPose* spatialPose_in, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (mat_out && spatialPose_in)
	{
		//create translate matrix. There's probably an a3 call for this, but it works.
		a3mat4 translate = a3mat4_identity;
		a3vec4 spatial_translate;
		spatial_translate.xyz = spatialPose_in->position;
		spatial_translate.w = 1;
		translate.v3 = spatial_translate;

		//create rotation matrix
		a3mat4 rotate = a3mat4_identity;
		a3mat4 xRot, yRot, zRot, tmp;
		switch (order)
		{
		case a3poseEulerOrder_xyz:
			a3real4x4SetRotateXYZ(rotate.m, spatialPose_in->orientation.x, spatialPose_in->orientation.y, spatialPose_in->orientation.z);
			break;
		case a3poseEulerOrder_zyx:
			a3real4x4SetRotateZYX(rotate.m, spatialPose_in->orientation.x, spatialPose_in->orientation.y, spatialPose_in->orientation.z);
			break;
		case a3poseEulerOrder_zxy:
			//create the y, x, and z matrices
			//xytmp = mult(x,y)
			//zxy = mult(z, xytmp)
			break;
		case a3poseEulerOrder_yxz:
			xRot = a3mat4_identity;
			a3real4x4SetRotateX(xRot.m, spatialPose_in->orientation.x);
			yRot = a3mat4_identity;
			a3real4x4SetRotateY(yRot.m, spatialPose_in->orientation.y);
			zRot = a3mat4_identity;
			a3real4x4SetRotateZ(zRot.m, spatialPose_in->orientation.z);
			tmp = a3mat4_identity;
			a3real4x4Product(tmp.m, xRot.m, zRot.m);
			a3real4x4Product(rotate.m, yRot.m, tmp.m);
			break;
		}

		//create scale matrix
		a3mat4 scale = a3mat4_identity;
		scale.m00 = spatialPose_in->scale.x;
		scale.m11 = spatialPose_in->scale.y;
		scale.m22 = spatialPose_in->scale.z;
		// Algorithm in slides

		//form TRS matrix
		a3mat4 rs;
		a3real4x4Product(rs.m, rotate.m, scale.m);
		a3real4x4Product(mat_out->m, translate.m, rs.m);
		return 1;
	}
	return -1;
}

// copy operation for single node pose
inline a3i32 a3spatialPoseCopy(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_in)
{
	if (spatialPose_out && spatialPose_in)
	{
		// essentially step function if we were to use this to animate
		spatialPose_out->position = spatialPose_in->position;
		spatialPose_out->orientation = spatialPose_in->orientation;
		spatialPose_out->scale = spatialPose_in->scale;


		return 1;
	}
	return -1;
}

inline a3i32 a3spatialPoseConcat(a3_SpatialPose* spatialPose_out, a3_SpatialPose* spatialPose_lhs, a3_SpatialPose* spatialPose_rhs, const a3boolean usingQuaternions)
{
	if (spatialPose_out && spatialPose_lhs && spatialPose_rhs)
	{
		// orientation -> o_lhs + o_rhs
		// scale -> s_lhs * s_rhs (component-wise)
		// translation -> t_lhs + t_rhs

//		a3real3SetReal3(spatialPose_out->orientation.v, a3real3Add(spatialPose_lhs->orientation.v, spatialPose_rhs->orientation.v));

		if (usingQuaternions)
		{
			//quaternion code
		}
		else
		{
			a3real3Sum(spatialPose_out->orientation.v, spatialPose_lhs->orientation.v, spatialPose_rhs->orientation.v);

			// Keep our angles within range -360, 360 !
			while (spatialPose_out->orientation.x < -360.0f || spatialPose_out->orientation.x > 360.0f)
			{
				if (spatialPose_out->orientation.x < -360.0f)
				{
					spatialPose_out->orientation.x += 720.0f;
				}
				if (spatialPose_out->orientation.x > 360.0f)
				{
					spatialPose_out->orientation.x -= 720.0f;
				}
			}
			while (spatialPose_out->orientation.y < -360.0f || spatialPose_out->orientation.y > 360.0f)
			{
				if (spatialPose_out->orientation.y < -360.0f)
				{
					spatialPose_out->orientation.y += 720.0f;
				}
				if (spatialPose_out->orientation.y > 360.0f)
				{
					spatialPose_out->orientation.y -= 720.0f;
				}
			}
			while (spatialPose_out->orientation.z < -360.0f || spatialPose_out->orientation.z > 360.0f)
			{
				if (spatialPose_out->orientation.z < -360.0f)
				{
					spatialPose_out->orientation.z += 720.0f;
				}
				if (spatialPose_out->orientation.z > 360.0f)
				{
					spatialPose_out->orientation.z -= 720.0f;
				}
			}
		}
		

		//make sure this stays within range
//		a3real3SetReal3(spatialPose_out->position.v, a3real3Add(spatialPose_lhs->position.v, spatialPose_rhs->position.v));
		a3real3Sum(spatialPose_out->position.v, spatialPose_lhs->position.v, spatialPose_rhs->position.v);
		a3real3ProductComp(spatialPose_out->scale.v, spatialPose_lhs->scale.v, spatialPose_rhs->scale.v);

		return 1;
	}

	return -1;
}

//lerps each component. Doesn't work correctly yet
inline a3i32 a3spatialPoseLerp(a3_SpatialPose* spatialPose_out, a3_SpatialPose* spatialPose_0, a3_SpatialPose* spatialPose_1, const a3real u)
{
	if (spatialPose_out && spatialPose_0 && spatialPose_1)
	{
		// transform - > no
		// orientation -> lerp(o0, o1, u)
		// scale -> lerp(s0, s1, u)
		// translation -> lerp(t0, t1, u)

		//quaternion
		//orientation -> lerp(o1, o2, u) -> 4d vec lerp
		//			ends up scaling though. |q| < 1 -> s = |q|^2
		//use nlerp.


		a3real3Lerp(spatialPose_out->orientation.v, spatialPose_0->orientation.v, spatialPose_1->orientation.v, u);
		a3real3Lerp(spatialPose_out->scale.v, spatialPose_0->scale.v, spatialPose_1->scale.v, u);
		a3real3Lerp(spatialPose_out->position.v, spatialPose_0->position.v, spatialPose_1->position.v, u);

		return 1;
	}

	return -1;
}

inline a3i32 a3spatialPoseCatRom(a3_SpatialPose* spatialPose_out, a3_SpatialPose* spatialPose_Prev, a3_SpatialPose* spatialPose_0, a3_SpatialPose* spatialPose_1, a3_SpatialPose* spatialPose_Next, const a3real u)
{
	if (spatialPose_out && spatialPose_0 && spatialPose_1 && spatialPose_Prev && spatialPose_Next)
	{
		a3real3CatmullRom(spatialPose_out->orientation.v, spatialPose_Prev->orientation.v, spatialPose_0->orientation.v, spatialPose_1->orientation.v, spatialPose_Next->orientation.v, u);
		a3real3CatmullRom(spatialPose_out->scale.v, spatialPose_Prev->scale.v, spatialPose_0->scale.v, spatialPose_1->scale.v, spatialPose_Next->scale.v, u);
		a3real3CatmullRom(spatialPose_out->position.v, spatialPose_Prev->position.v, spatialPose_0->position.v, spatialPose_1->position.v, spatialPose_Next->position.v, u);

		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_SPATIALPOSE_INL
#endif	// __ANIMAL3D_SPATIALPOSE_H
