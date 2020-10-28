/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
	a3_HierarchyBlendNode.h: declarations for blend trees and blend nodes.
*/

#ifndef A3_HIERARCHYBLENDNODE_H

#include "a3_HierarchyState.h"
#include "a3_Hierarchy.h"
#include "a3_KeyframeAnimation.h"

typedef struct a3_SpatialBlendNode a3_SpatialBlendNode;
typedef struct a3_SpatialBlendTree a3_SpatialBlendTree;
typedef void (*a3_SpatialBlendOp)(a3_SpatialPose* pose, ...);
typedef void (*a3_SpatialBlendExec)(a3_SpatialBlendNode* node_in);

void a3spatialBlendExec0C(a3_SpatialBlendNode* node_inout);
void a3spatialBlendExec1C(a3_SpatialBlendNode* node_inout);
void a3spatialBlendExec1C1I(a3_SpatialBlendNode* node_inout);
void a3spatialBlendExec2C(a3_SpatialBlendNode* node_inout);
void a3spatialBlendExec2C1I(a3_SpatialBlendNode* node_inout);
void a3spatialBlendExec3C2I(a3_SpatialBlendNode* node_inout);
void a3spatialBlendExec4C1I(a3_SpatialBlendNode* node_inout);
void a3spatialBlendExec4C3I(a3_SpatialBlendNode* node_inout);
void a3spatialBlendExec16C5I(a3_SpatialBlendNode* node_inout);

struct a3_SpatialBlendNode
{
	a3f32* uVals[8];
	a3_SpatialBlendOp operation;
	a3_SpatialBlendExec exec;
	a3_SpatialPose* pose;
	a3_SpatialPose* controls[16];
	a3_Clip* controlClips[16];
};

struct a3_SpatialBlendTree
{
	a3_Hierarchy* hierarchy;
	a3ui32* leafIndices;
	a3_SpatialBlendNode* blendNodes;

	/*
	LEAVES 1 5 18 34

	5 BLENDNODE LERP CLIP skel CLIP fish 0.5 END
	6 BLENDNODE SCALE NODE 5 CONTROL 0.5
	*/
};

///Hierarchy Version

typedef struct a3_HierarchyBlendNode a3_HierarchyBlendNode;
typedef struct a3_HierarchyBlendTree a3_HierarchyBlendTree;
typedef a3_HierarchyPose* (*a3_HierarchyBlendOp)(a3_HierarchyPose* pose, ...);
typedef void (*a3_HierarchyBlendExec)(a3_HierarchyBlendNode* node_in, a3ui32 nodeCount);

void a3hierarchyBlendExec0C(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount);
void a3hierarchyBlendExec1C(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount);
void a3hierarchyBlendExec1C1I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount);
void a3hierarchyBlendExec2C(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount);
void a3hierarchyBlendExec2C1I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount);
void a3hierarchyBlendExec3C2I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount);
void a3hierarchyBlendExec4C1I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount);
void a3hierarchyBlendExec4C3I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount);
void a3hierarchyBlendExec16C5I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount);

struct a3_HierarchyBlendNode
{
	a3f32* uVals[8];
	a3_HierarchyBlendOp operation;
	a3_HierarchyBlendExec exec;
	a3_HierarchyPose* pose;
	a3_HierarchyPose* controls[16];
	a3byte* clipNames[16]; //array of 16 cstrings
};

struct a3_HierarchyBlendTree
{
	a3_Hierarchy hierarchy[1];
	a3i32* leafIndices;
	a3ui32 leafCount;
	a3_HierarchyBlendNode* blendNodes;
};


a3i32 a3hierarchyBlendTreeLoad(a3_HierarchyBlendTree* blendTree_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath);
#endif // !A3_HIERARCHYBLENDNODE_H