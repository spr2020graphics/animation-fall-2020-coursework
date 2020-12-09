/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
	a3_Tree.h: A general-purpose tree that stores integers. Useful as a backing structure for another array.
*/

#ifndef __ANIMAL3D_TREE_H
#define __ANIMAL3D_TREE_H

#include "animal3D-A3DM/animal3D-A3DM.h"

typedef struct a3_TreeNode a3_TreeNode;

struct a3_TreeNode
{
	a3_TreeNode** children;
	a3i32 value;
	a3i32 childCount;
	a3i32 maxChildren;
};

/// <summary>
/// Initializes a tree node with a maximum amount of children.
/// </summary>
/// <param name="node"></param>
/// <param name="maxChildren"></param>
/// <returns></returns>
a3i32 a3TreeNodeInit(a3_TreeNode* node, a3i32 maxChildren);
a3i32 a3TreeNodeAddChildNode(a3_TreeNode* parent, a3_TreeNode* child);

/// <summary>
/// Creates a new tree node and adds it as a child of the parent. The parent's maxChildren value will be copied.
/// </summary>
/// <param name="node"></param>
/// <param name="childValue"></param>
/// <returns></returns>
a3i32 a3TreeNodeAddChildValue(a3_TreeNode* node, a3i32 childValue);

a3i32 a3TreeNodeRemoveChild(a3_TreeNode* node, a3i32 index);

a3i32 a3TreeNodeResizeChildArray(a3_TreeNode* node, a3i32 newMax);

a3i32 a3TreeNodeRelease(a3_TreeNode* node);
a3i32 a3TreeGetDescendantCount(a3_TreeNode* node);

a3i32 a3TreeEnsureUnique(a3_TreeNode* root, a3ui32 maxScanDepth, a3ui32 minVal, a3ui32 maxVal);
a3_TreeNode* a3TreeRetrieveNodeWithValue(a3_TreeNode* root, a3i32 value, a3ui32 scanDepth);
#endif