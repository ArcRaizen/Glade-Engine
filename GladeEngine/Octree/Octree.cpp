#include "Octree.h"

using namespace Glade;

Octree::Octree(OctreeType t, bool resize, Vector center, gFloat width, gFloat height, gFloat depth, int maxE, int minE, gFloat loose) : type(t), resizeable(resize), maxElementsPerPartition(maxE >= 1 ? maxE : 1), 
																																		minElementsBeforeMerging(minE >= 0 ? (minE > maxE ? maxE : minE) : 0),
																																		looseness(loose), boundingBox(center, Vector(width/2,height/2,depth/2)), beganUpdate(false)
{
	nodes[1] = new Octnode(this, center, width, height, depth, 1);
}

Octree::~Octree()
{
	for(auto iter = nodes.begin(); iter != nodes.end(); iter++)
		delete iter->second;
	nodes.clear();
}

void Octree::Update()
{
	if(type == OctreeType::STATIC) return;
	beganUpdate = true;
	// std::map is already zsorted by operator< order.
	// Deeper Octnodes have larger locationCodes, so by going in reverse order
	// we can loop from the deepest node up to the root
	iter = nodes.end();
	for(iter--; iter != nodes.begin(); iter--)
		iter->second->Update();
	iter->second->Update();
	beganUpdate = false;

}

void Octree::AddElement(Object* o)
{
	uint64_t loc = 1;
	Octnode* curNode = LookUpNode(loc);
	Vector c = o->GetPosition();
	gFloat rad = o->GetRadius();

	// Locate deepest node that contains center-point of Object
	int best;
	while(curNode->HasChildren())
	{
		best = curNode->BestChild(c);
		loc = (loc<<3) | best;
		curNode = LookUpNode(loc);

		if(curNode == nullptr)
		{
			LookUpNode(loc>>3)->Split(1<<best);
			curNode = LookUpNode(loc);
			break;
		}
	}

	// Find smallest node that encapsulates Object
	while(loc > 0)
	{
		if(curNode->AddElement(o, c, rad))
			return;
		else	// Loop up tree to curNode's parent
		{
			curNode = GetParentNode(curNode);
			loc >>= 3;
		}
	}

	// Root node does not encapsulate Object
	if(resizeable)
		Grow(o);
	else
		assert(false);
}

void Octree::Grow(Object* o)
{
	Octnode* newRoot;
	std::map<uint64_t, Octnode*> test;
	int grewTreeKey = beganUpdate ? iter->first : -1;
	do
	{
		// Find nearest corner of root's AABB to Obejct
		NodeAABB rootBox = nodes[1]->GetBox();
		Vector diff = o->GetPosition() - rootBox.center;
		diff.x = diff.x == 0 ? 0 : (diff.x / Abs(diff.x)) * rootBox.halfWidths[0];
		diff.y = diff.y == 0 ? 0 : (diff.y / Abs(diff.y)) * rootBox.halfWidths[1];
		diff.z = diff.z == 0 ? 0 : (diff.z / Abs(diff.z)) * rootBox.halfWidths[2];

		// Create new Root Node with the nearest corner of current root node as center
		newRoot = new Octnode(this, rootBox.center+diff, rootBox.halfWidths[0]*4, rootBox.halfWidths[1]*4, rootBox.halfWidths[2]*4, 1);

		// Determine which child the current root is of the new root
		int best = newRoot->BestChild(rootBox.center);
		newRoot->AssignNewRoot(best, nodes[1]->GetNumElementsBranch());

		// Save key of node in Update loop that activated Grow and update it
		if(grewTreeKey != -1)
		{
			int count = 0;
			while(grewTreeKey >> count != 1)
				count++;
			grewTreeKey = (1 << (count+3)) | (best << count) | ((grewTreeKey << (64-count)) >> (64-count));
		}
		// Update locationCode of existing nodes to account for a new root 
		test = nodes;
		nodes.clear();
		for(auto it = test.rbegin(); it != test.rend(); it++)
			nodes[it->second->GrowLocCode(best)] = it->second;
		test.clear();
		
		// Update iterator to find current node again
		if(grewTreeKey != -1)
			iter = nodes.find(grewTreeKey);

		// Add new root into tree
		nodes[1] = newRoot;
	} while(!newRoot->AddElement(o, o->GetPosition(), o->GetRadius()));
}

void Octree::Shrink()
{
	std::map<uint64_t, Octnode*> test = nodes;

	// Delete all child nodes of the root node that have no elements
	for(uint64_t i = 8; i < 16; i++)
	{
		auto it = test.find(i);
		if(it != test.end() && it->second->GetNumElementsBranch() == 0)
		{
			delete it->second;
			test.erase(it);
		}
	}

	test.erase(1);
	nodes.clear();
	for(auto it = test.begin(); it != test.end(); it++)
		nodes[it->second->ShrinkLocCode()] = it->second;
}

void Octree::CreateNode(Octnode* node)
{
	nodes[node->GetLocCode()] = node;
}

Octnode* Octree::GetParentNode(Octnode* node) const
{
	return LookUpNode(node->GetLocCode() >> 3);
}

Octnode* Octree::LookUpNode(uint64_t locCode) const
{
	const auto iter = nodes.find(locCode);
	return (iter == nodes.end() ? nullptr : iter->second);
}

void Octree::DeleteNode(uint64_t locCode)
{
	CONSOLEOUTPUT("Deleting Node " << locCode << "\n");
	assert(!LookUpNode(locCode)->HasChildren());
	delete nodes[locCode];
	nodes.erase(locCode);
}

unsigned int Octree::CalcNodeDepth(uint64_t locCode) const
{
	for(unsigned int d = 0; locCode; d++)
	{
		if(locCode == 1) 
			return d;
		locCode >>= 3;
	}

	// Bad Location Code
	assert(false);
}