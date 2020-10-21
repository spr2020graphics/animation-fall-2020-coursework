#ifndef A3_HIERARCHYBLENDNODE_H

#include "a3_SpatialPose.h"
#include "a3_Hierarchy.h"

typedef struct a3_SpatialBlendNode a3_SpatialBlendNode;
typedef void (*a3_SpatialBlendOp)(a3_SpatialPose* pose, ...);
typedef void (*a3_SpatialBlendExec)(a3_SpatialBlendNode* node_in);

void a3spatialBlendExec0C(a3_SpatialBlendNode* node_inout);
void a3spatialBlendExec1C(a3_SpatialBlendNode* node_inout);

struct a3_SpatialBlendNode
{
	a3f32* uVals[8];
	a3_SpatialBlendOp operation;
	a3_SpatialBlendExec exec;
	a3_SpatialPose* pose;
	a3_SpatialPose* controls[16];
};

struct a3_SpatialBlendTree
{
	a3_Hierarchy* hierarchy;
	a3ui32 leafIndices;

	/*
	LEAVES 1 5 18 34

	5 BLENDNODE LERP CLIP skel CLIP fish 0.5 END
	6 BLENDNODE SCALE NODE 5 CONTROL 0.5
	*/
};

#endif // !A3_HIERARCHYBLENDNODE_H