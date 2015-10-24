#pragma once
#ifndef GLADE_OCTNODE_H
#define GLADE_OCTNODE_H

#ifndef GLADE_OBJECT_H
#include "Object.h"
#endif
#include <vector>
#include <stdint.h>

#include <sstream>
#include <Windows.h>
#define CONSOLEOUTPUT(s)					\
	{											\
		std::wostringstream os_;				\
		os_ << s;								\
		OutputDebugStringW(os_.str().c_str());	\
	}

namespace Glade {

class Octree;

// Struct containing all information about an Object stored in an Octnode
struct NodeObject
{
	NodeObject(Vector c, gFloat r, Object* o_) : center(c), radius(r), o(o_) { }
	Vector center;		// Center pointer of object (unless it moved)
	gFloat radius;		// Radius of circle totally encompassing 't'
	Object* o;			// Pointer to object represented by this struct

	bool HasMoved() { return !(center == o->GetPosition()); }
};

// Struct representing the BoundingBox associated with an Octnode
struct NodeAABB
{
	NodeAABB(Vector c, Vector extents) : center(c), halfWidths(extents) { }
	Vector center;
	Vector halfWidths;

	bool ContainsPoint(Vector p)
	{
		if(p.x < center.x - halfWidths.x) return false;
		if(p.x > center.x + halfWidths.x) return false;
		if(p.y < center.y - halfWidths.y) return false;
		if(p.y > center.y + halfWidths.y) return false;
		if(p.z < center.z - halfWidths.z) return false;
		if(p.z > center.z + halfWidths.z) return false;
		return true;
	}
};

/*
	A Node in an Octree that contains a list of all Objects totally encapsulated inside itself
*/
class Octnode
{
public:
	Octnode();
	Octnode(Octree* t, Vector center, gFloat width, gFloat height, gFloat depth, uint64_t loc);
	~Octnode();

	void	Update();
	bool	AddElement(Object* o, Vector c, gFloat rad);	// Add a new Object to the list of elements
	void	Split(unsigned int i);							// Create specified child nodes if they dont already exist
	void	Merge();										// Add all NodeObjects from children to self and delete children
	int		BestChild(const Vector v) const;				// Calculate the child an object with center-pointer 'v' would most likely fit in
	int		GrowLocCode(int newCode);						// Edit the locationCode of this node to account for a new root when the tree grows
	int		ShrinkLocCode();
	void	AssignNewRoot(int c, int branch);				// Manually assign some stats to a new root node. DO NOT USE except for when a tree grows

private:
	void AddElement(NodeObject* no);						// Add a NodeObject from parent to list of elements
	void PassUpObject(NodeObject* no);						// Take NodeObject from child that no longer fits in child and attempt to add it to self. Otherwise, pass to parent
	bool Encapsulates(Vector c, gFloat rad) const;			// Calculate if this node's bounding box completely encapsulates a sphere of radius 'rad' at center-point 'c'
	void ResizeArray();										// Resize the elements array if a situation arises where it mush containt more nodes than originally allowed

	void UpdateBranch();				// Increase numElementsBranch and all parent nodes
	int	 CalcNumBranch();

public:
	NodeObject** GetElements();			// return elements array
	int  GetNumElements();				// return number of NodeObject*'s in elements array
	int  GetNumElementsBranch();		// return numbe of NodeObjects*'s in this node and all nodes below it
	uint64_t GetLocCode();				// return locationCode
	bool HasChildren();					// Does this Octnode have any children?
	bool HasChild(unsigned int i);		// Does this Octnode have a specific child?
	NodeAABB GetBox();

private:
	Octree*			tree;				// Pointer to tree this node belongs to
	uint64_t		locationCode;		// Code that is used to hash this Node in a hash-table. Based off location from parent(s) and used to compute children's code
	NodeObject**	elements;			// Array of elements intersecting this node
	unsigned int	numElements;		// Number of elements in the tree intersecting this node
	unsigned int	numElementsBranch;	// Number of elements in the tree at this node and all nodes below it (the branch starting at this node)
	unsigned int	maxElements;		// Maximum number of elements allowed to be stored in this node
	int				children;			// Bit-flag to track which children of this node exist
	NodeAABB		boundingBox;		// Bounding Box that encompases this Node.
};
inline NodeObject** Octnode::GetElements() { return elements; }
inline int Octnode::GetNumElements() { return numElements; }
inline int Octnode::GetNumElementsBranch() { return numElementsBranch; }
inline uint64_t Octnode::GetLocCode() { return locationCode; }
inline bool Octnode::HasChildren() { return children > 0; }
inline bool Octnode::HasChild(unsigned int i) { return children & i; }
inline NodeAABB Octnode::GetBox() { return boundingBox; }
}	// namespace
#endif	// GLADE_OCTNODE_H

