/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
	a3_HierarchyBlendNode.h: declarations for blend trees and blend nodes.
*/

#ifndef A3_HIERARCHYBLENDNODE_H
#define A3_HIERARCHYBLENDNODE_H

#include "a3_HierarchyState.h"
#include "a3_Hierarchy.h"
#include "a3_KeyframeAnimationController.h"
#include <A3_DEMO/_animation/a3_Tree.h>

/// <summary>
/// Enum for node operations
/// </summary>
enum NodeType {
	addClip, lerpClip, scaleClip, negateClip, copyClip,
	identity, init, fk, ik,
	copy, negate, concat, convert, revert,
	scale, biscale,
	deconcat,
	nearest, lerp, easeinout,
	triangular,
	cubic,
	binearest, bilerp,
	bicubic
};
typedef enum NodeType NodeType;

typedef struct a3_HierarchyBlendNode a3_HierarchyBlendNode;
typedef struct a3_HierarchyBlendTree a3_HierarchyBlendTree;
typedef a3_HierarchyPose* (*a3_HierarchyBlendOp)(a3_HierarchyPose* pose, ...);//function pointer to a hierarchy pose op
typedef void (*a3_HierarchyBlendExec)(a3_HierarchyBlendNode* node_in);//function pointer to something that calls that hierarchy pose op and passes the right data in

void a3hierarchyBlendExec0C(a3_HierarchyBlendNode* node_inout);
void a3hierarchyBlendExec1C(a3_HierarchyBlendNode* node_inout);
void a3hierarchyBlendExec1C1I(a3_HierarchyBlendNode* node_inout);
void a3hierarchyBlendExec2C(a3_HierarchyBlendNode* node_inout);
void a3hierarchyBlendExec2C1I(a3_HierarchyBlendNode* node_inout);
void a3hierarchyBlendExec3C2I(a3_HierarchyBlendNode* node_inout);
void a3hierarchyBlendExec4C1I(a3_HierarchyBlendNode* node_inout);
void a3hierarchyBlendExec4C3I(a3_HierarchyBlendNode* node_inout);
void a3hierarchyBlendExec16C5I(a3_HierarchyBlendNode* node_inout);

void a3hierarchyClipExec1C(a3_HierarchyBlendNode* node_inout);
void a3hierarchyClipExec1C1I(a3_HierarchyBlendNode* node_inout);
void a3hierarchyClipExec2C(a3_HierarchyBlendNode* node_inout);
void a3hierarchyClipExec2C1I(a3_HierarchyBlendNode* node_inout);

struct a3_HierarchyBlendNode
{
	a3f32* uVals[8];
	a3_HierarchyBlendOp operation;
	a3_HierarchyBlendExec exec;
	a3_HierarchyState* state_out;
	a3_HierarchyState* controlStates[16];
	a3byte* clipNames[16]; //array of 16 cstrings. Load these clips into clip controllers
	a3_ClipController* clipControllers[16]; //array of 16 pointers to clip controllers.

	//binding information
	NodeType nodeType; //reference for binding
	a3ui32 clipCount; //useful for iterating when binding
	a3ui32 controlNodeIndices[16];
	a3ui32 controlNodeCount; //node equivalent of clipCount
	a3ui32 uValCount;

	a3_HierarchyPoseGroup* poseGroup;
};

struct a3_HierarchyBlendTree
{
	a3_TreeNode* tree;
	a3_HierarchyBlendNode** blendNodes;
	a3i32 maxNodes;
	a3i32 nodeCount;
};

a3_HierarchyBlendExec a3hierarchyBlendNodeGetExec(NodeType nodeType);

a3i32 a3hierarchyBlendNodeCreate(a3_HierarchyBlendNode* node, NodeType nodeType);

a3i32 a3hierarchyBlendNodeGenControllersWithClipNames(a3_HierarchyBlendNode* node, a3i32 ctrlCount, char** ctrlNames, char** clipNames, a3_ClipPool* pool, const a3i32 playback_step, const a3f64 playback_stepPerSec);
a3i32 a3hierarchyBlendNodeGenControllersWithClipIndices(a3_HierarchyBlendNode* node, a3i32 ctrlCount, char** ctrlNames, a3i32* clipIndices, a3_ClipPool* pool, const a3i32 playback_step, const a3f64 playback_stepPerSec);

a3i32 a3hierarchyBlendNodeBindClipController(a3_HierarchyBlendNode* node, a3_ClipController* ctrl, a3ui32 index);
a3i32 a3hierarchyBlendNodeAddControl(a3_HierarchyBlendNode* node, a3ui32 slot, a3f32* controlSource);

a3i32 a3hierarchyBlendTreeCreate(a3_HierarchyBlendTree* blendTree_out, int size, a3boolean allocateNodes);

a3i32 a3hierarchyBlendTreeStoreNode(a3_HierarchyBlendTree* tree, a3_HierarchyBlendNode* node);

//adds a node in the BlendTree's array to the internal tree, creating a parent-child relationship. Currently needs to be done in root->leaf order and in input order.
a3i32 a3hierarchyBlendTreeAddNodeToTree(a3_HierarchyBlendTree* blendTree, int node, int sourceNode);

a3i32 a3hierarchyBlendTreeBindStates(a3_HierarchyBlendTree* blendTree, a3_Hierarchy* hierarchy, a3_HierarchyState** optStates);

//a3i32 a3hierarchyBlendTreeLoad(a3_HierarchyBlendTree* blendTree_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath);
a3i32 a3hierarchyblendTreeUpdate(a3_HierarchyBlendTree* blendTree_out);
#endif // !A3_HIERARCHYBLENDNODE_H