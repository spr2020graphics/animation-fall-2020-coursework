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
	Addition to animal3D SDK by Scott Dagen and Cameron Schneider
	
	a3_Raycasting.h
	Basic Raycasting between a ray and plane
*/

#ifndef __ANIMAL3D_RAYCASTING_H
#define __ANIMAL3D_RAYCASTING_H


#include "animal3D-A3DM/animal3D-A3DM.h"
#include "A3_DEMO/_a3_demo_utilities/a3_DemoSceneObject.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_Ray			a3_Ray;
typedef struct a3_Plane			a3_Plane;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

struct a3_Ray
{
	a3vec3* origin;
	a3vec3* direction;
};

struct a3_Plane
{
	a3vec3* center;
	a3vec3* normal;
	a3mat4* transform;
	a3mat4* parentObjInv;

	// basically the "localScale" of the plane, a plane's z-scale should always be 1 in animal (the vertical axis, since they have no thickness)
	a3vec3* boundSize;
};

a3_Ray* a3createRay(a3_Ray* out, a3vec3* orig, a3vec3* dir);

a3_Plane* a3createPlane(a3_Plane* out, a3mat4* trans, a3mat4* parObjInv);

a3boolean a3raycastGetCollisionUnboundedPlane(a3_Ray* ray, a3_Plane* plane, a3boolean isRayBackwards, a3vec3* out_point);
a3boolean a3raycastGetCollisionBoundedPlane(a3_Ray* ray, a3_Plane* plane, a3boolean isRayBackwards, a3vec3* out_point);

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_RAYCASTING_H