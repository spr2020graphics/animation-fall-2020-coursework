#include "..\a3_Tree.h"
#include <stdlib.h>

a3i32 a3TreeNodeInit(a3_TreeNode* node, a3i32 maxChildren)
{
	if (node && !node->children && maxChildren >= 0)
	{
		node->children = malloc(sizeof(a3_TreeNode*) * maxChildren);
		node->value = 0;
		node->childCount = 0;
		node->maxChildren = maxChildren;
		return 1;
	}
	return -1;
}

a3i32 a3TreeNodeAddChildNode(a3_TreeNode* parent, a3_TreeNode* child)
{
	if (parent && child)
	{
		parent->children[parent->childCount++] = child;
		return 1;
	}
	return -1;
}

a3i32 a3TreeNodeAddChildValue(a3_TreeNode* node, a3i32 childValue)
{
	if (node && node->childCount < node->maxChildren)
	{
		a3_TreeNode* child = malloc(sizeof(a3_TreeNode));
		if (child)
		{
			a3TreeNodeInit(child, node->maxChildren);
			child->value = childValue;
			return a3TreeNodeAddChildNode(node, child);
		}
		return -2;
	}
	return -1;
}

a3i32 a3TreeNodeRemoveChild(a3_TreeNode* node, a3i32 index)
{
	if (node && index >= 0 && node->childCount > index && node->children)
	{
		a3TreeNodeRelease(node->children[index]);
		if (index == node->childCount - 1)
		{
			node->childCount--;
		}
		else
		{
			for (int i = index; i < node->childCount - 1; i++)
			{
				node->children[i] = node->children[i + 1]; //shift them to the left
			}
		}
		return 1;
	}
	return -1;
}

a3i32 a3TreeNodeResizeChildArray(a3_TreeNode* node, a3i32 newMax)
{
	if (node && node->children && newMax != node->maxChildren && newMax >= 0)
	{
		if (newMax == 0)
		{
			for (int i = 0; i < node->childCount; i++) //delete all existing children
			{
				if (node->children[i])
				{
					a3TreeNodeRelease(node->children[i]); //recursively free children
				}
			}
			free(node->children); //free child array
			node->children = NULL;
			node->maxChildren = 0;
			node->childCount = 0;
			return 1;
		}
		else //newMax > 0
		{
			a3_TreeNode** children = NULL;
			if (node->childCount > 0)
			{
				children = node->children; //cache children in a separate array
			}
			//create new child array
			node->children = malloc(sizeof(a3_TreeNode*) * newMax);
			if (node->maxChildren > newMax) //reducing array size and freeing all children
			{
				int i;
				if (node->childCount > 0) //if there were children, we want to copy all that fit and delete the rest.
				{
					for (i = 0; i < min(node->childCount, newMax); i++) //copy over either all children or as many as will fit in the array, whichever is smaller
					{
						node->children[i] = children[i];
					}
					for (; i < node->maxChildren; i++) //delete remaining children
					{
						if (children[i])
						{
							a3TreeNodeRelease(children[i]);
						}
					}
				}
				free(children);
				node->maxChildren = newMax;
				node->childCount = newMax;
				return 1;
			}
			else if (node->maxChildren < newMax) //expanding the array
			{
				if (node->childCount > 0) //if we have children to copy over, copy them from the old array
				{
					for (int i = 0; i < node->childCount; i++)
					{
						node->children[i] = children[i];
					}
				}
				free(children); //clear the old array
				node->maxChildren = newMax;
				return 1;
			}
		}
	}
	return -1;
}

a3i32 a3TreeNodeRelease(a3_TreeNode* node)
{
	if (node)
	{
		for (int i = 0; i < node->childCount; i++)
		{
			if (node->children[i])
			{
				a3TreeNodeRelease(node->children[i]);
			}
		}
		free(node->children);
		free(node);
		return 1;
	}
	return -1;
}

a3i32 a3TreeGetDescendantCount(a3_TreeNode* node)
{
	if (node)
	{
		a3i32 result = node->childCount;
		for (int i = 0; i < node->childCount; i++)
		{
			if (node->children[i])
			{
				result += a3TreeGetDescendantCount(node->children[i]);
			}
		}
		return result;
	}
	return -1;
}
