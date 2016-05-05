#pragma once
#ifndef GLADE_OCTREE_H
#define GLADE_OCTREE_H

#include "Octnode.h"
#include <map>

#define BOTTOM_LEFT_FRONT = 0
#define BOTTOM_RIGHT_FRONT = 1
#define BOTTOM_LEFT_BACK = 2
#define BOTTOM_RIGHT_BACK = 3
#define TOP_LEFT_FRONT = 4
#define TOP_RIGHT_FRONT = 5
#define TOP_LEFT_BACK = 6
#define TOP_RIGHT_BACK = 7

namespace Glade { 

class Octree
{
public:
	enum class OctreeType { STATIC, DYNAMIC };
	Octree(OctreeType t, bool resize, Vector center, gFloat width, gFloat height, gFloat depth, int maxE, int minE, gFloat loose=1);
	~Octree();

	friend class Octnode;

	void	Update();
	void	AddElement(Object* o);
	void	Grow(Object* o);
	void	Shrink();

	void	 CreateNode(Octnode* node);
	Octnode* GetParentNode(Octnode* node) const;
	Octnode* LookUpNode(uint64_t locCode) const;
	void	 DeleteNode(uint64_t locCode);
	unsigned int CalcNodeDepth(uint64_t locCode) const;

protected:
	// Map of all nodes in the Octree.
	// Each node is hashed according to its Locational Code which is derived by its depth and location in the tree relative to the root
	std::map<uint64_t, Octnode*> nodes;
	std::map<uint64_t, Octnode*>::iterator iter;
	
	AABB			boundingBox;					// Axis-Aligned bounding box that represents the entire bounds of the Octree
	gFloat			looseness;						// Ratio of the size of the hitbox checked for object containment vs the actual size of boundingBox

	OctreeType		type;
	bool			resizeable;
	bool			beganUpdate;

	// TODO - THIS NAMING STILL HURTS
	unsigned int	maxElementsPerPartition;		// Maximum number of elements in a node before that node partitions into children
	unsigned int	minElementsBeforeMerging;		// Minimum number of elements in a branch before that branch prunes itself and passes the NodeObjects into the branch's parent
};
}	// namespace
#endif // GLADE_OCTREE_H

