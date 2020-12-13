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
	Additions to animal3d by Cameron Schneider & Scott Dagen

	Basic ray/plane raycasting implementation.
*/

#include "../a3_Raycasting.h"
#include <stdio.h>


//-----------------------------------------------------------------------------

a3_Ray* a3createRay(a3_Ray* out, a3vec3* orig, a3vec3* dir)
{
	out->direction = dir;
	out->origin = orig;

	return out;
}

a3_Plane* a3createPlane(a3_Plane* out, a3mat4* trans, a3mat4* parObjInv)
{
	out->center = &trans->v3.xyz;
	out->normal = &trans->v1.xyz;

	a3real3Normalize(out->normal->v);

	out->transform = trans;
	out->parentObjInv = parObjInv;

	return out;
}

a3boolean a3raycastGetCollisionUnboundedPlane(a3_Ray* ray, a3_Plane* plane, a3vec3* out_point)
{
	a3boolean result = false;

	// line = origin + t * direction
	// <x, y, z> = <x0, y0, z0> + <a, b, c>t;
	// x = x0 + ta
	// y = y0 + tb
	// z = z0 + tc

	// difference: ray_origin - center
	// dot:

	a3real3Normalize(plane->normal->v);
	//a3real3Normalize(ray->direction->v);

	a3real plane_d;
	// THIS IS THE PLANE EQUATION nx + ny + nz = d
	plane_d = (plane->normal->x * plane->center->x + plane->normal->y * plane->center->y + plane->normal->z * plane->center->z);	// negated because it needs to be on the "other' side of the equation

	a3real x_t = plane->normal->x * ray->direction->x;
	a3real x = plane->normal->x * ray->origin->x;

	a3real y_t = plane->normal->y * ray->direction->y;
	a3real y = plane->normal->y * ray->origin->y;

	a3real z_t = plane->normal->z * ray->direction->z;
	a3real z = plane->normal->z * ray->origin->z;

	// get solutions for t term and regular term
	a3real t_term = x_t + y_t + z_t;
	a3real regular_term = x + y + z;

	if (t_term == 0 && regular_term != plane_d)
	{
		// there is NO SOLUTION, i.e line is parallel to the plane
		result = false;
	}
	else if (t_term == 0 && regular_term == plane_d)
	{
		// the line is part of the plane, i.e all values of t are valid solutions (collinear)
		result = false;
	}
	else
	{
		a3real rhs = plane_d - regular_term;

		a3real t_solution = rhs / t_term;

		if (t_solution < 0.0f)
		{
			// there is a single-point solution, determine point here
			result = true;

			out_point->x = ray->origin->x + ray->direction->x * t_solution;
			//out_point->x = t_solution;
			out_point->y = ray->origin->y + ray->direction->y * t_solution;
			out_point->z = ray->origin->z + ray->direction->z * t_solution;
		}
	}

	
	return result;
}

a3boolean a3raycastGetCollisionBoundedPlane(a3_Ray* ray, a3_Plane* plane, a3vec3* out_point)
{
	a3vec4 intersection = a3vec4_zero;

	if (a3raycastGetCollisionUnboundedPlane(ray, plane, &intersection.xyz))
	{
		a3mat4 m = a3mat4_identity;
		a3real4x4TransformInverse(m.m, plane->transform->m);
		// v2 of plane transform mat is the normal, other cols are T and B (Right, Up, Out)
		// diff from origin to point for sq. distance
		// compare sq. distance with square dot of diff and tangent
		//printf("Before: %f, %f, %f\n", intersection.x, intersection.y, intersection.z);
		a3vec4 output = a3vec4_zero;
		a3real4TransformProduct(output.v, m.m, intersection.v);
		printf("After: %f, %f, %f\n", output.x, output.y, output.z);
		a3boolean withinX = intersection.x >= (-plane->boundSize->x) && intersection.x <= (plane->boundSize->x);
		a3boolean withinY = intersection.y >= (-plane->boundSize->y) && intersection.y <= (plane->boundSize->y);
		printf("%i\n", withinX && withinY);
		return withinX && withinY;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------
