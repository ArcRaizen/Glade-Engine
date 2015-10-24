#include "Octnode.h"
#include "Octree.h"

using namespace Glade;

Octnode::Octnode() : boundingBox(Vector(), Vector()) { }

Octnode::Octnode(Octree* t, Vector center, gFloat width, gFloat height, gFloat depth, uint64_t loc) :  tree(t), numElements(0), numElementsBranch(0), locationCode(loc),
																										boundingBox(center, Vector(width/2, height/2, depth/2))
{
	maxElements = tree->maxElementsPerPartition;
	elements = new NodeObject*[maxElements];
}

Octnode::~Octnode() 
{
	delete[] elements;
}

void Octnode::Update()
{
	for(unsigned int i = 0; i < numElements; i++)
	{
		if(elements[i]->HasMoved())
		{
			elements[i]->center = elements[i]->o->GetPosition();		// Reset its saved position
			if(Encapsulates(elements[i]->center, elements[i]->radius))	// Still in this node, try to move to children
			{
				// Already have children or need to create children, attempt to pass down to them
				if(children != 0 || numElements > tree->maxElementsPerPartition)
				{
					int best = BestChild(elements[i]->center);	// Find best child for moving element
					Split(1<<best);								// Create that child (if it hasn't been already)
					Octnode* child = tree->LookUpNode(locationCode<<3|best);	// Get pointer to best child

					// If child can contain element, remove element from self and pass down to child
					if(child->Encapsulates(elements[i]->center, elements[i]->radius))
					{
						child->AddElement(elements[i]);		// Put into child

						// Slide remaining elements down in array (so array remains contiguous)
						for(unsigned int j = i; j < numElements-1; j++)
							elements[j] = elements[j+1];
						numElements--;
						i--;
					}
					// Element could not be store in child, remains in self
				}
				// No children or need to split
			}
			else	// No longer in this node, pass up to parent and let them figure it out
			{
				NodeObject* temp = elements[i];

				// Slide remaining elements down in array (so array remains contiguous)
				for(unsigned int j = i; j < numElements-1; j++)
					elements[j] = elements[j+1];
				numElements--;			// one less element in self
				numElementsBranch--;	//	and one less in branch

				// Pass up to parent and try again
				if(locationCode > 1)
					tree->LookUpNode(locationCode>>3)->PassUpObject(temp);
				else if(tree->resizeable)
					tree->Grow(temp->o);
				i--;

				assert(numElementsBranch >= numElements);
			}
		}
	}

	// If immediate branch (self and children) have a small amount of elements, merge together and destroy children
	if(children > 0)
	{
		assert(numElementsBranch >= numElements);
		if(locationCode == 1 && numElements == 0)
		{
			int numChildrenWithElements = 0;
			for(unsigned int i = 0; i < 8; i++)
					numChildrenWithElements += children & (1<<i) ? (tree->LookUpNode(locationCode<<3|i)->GetNumElementsBranch() > 0 ? 1 : 0) : 0;

			if(numChildrenWithElements == 1)
			{
				tree->Shrink();
				delete this;
				return;
			}
		}
		if(numElementsBranch <= tree->minElementsBeforeMerging)
			Merge();
	}
}

bool Octnode::AddElement(Object* o, Vector c, gFloat rad)
{
	if(!Encapsulates(c, rad))
		return false;

	// Object definitely being added at at-least this level, inform parents that the branch grew
	if(locationCode > 1)
		tree->LookUpNode(locationCode>>3)->UpdateBranch();

	AddElement(new NodeObject(c, rad, o));
	return true;
}

void Octnode::AddElement(NodeObject* no)
{
	numElementsBranch++;	// Branch increases inside as long as something is added to itself or its children (or their children etc...)
	if(numElements < tree->maxElementsPerPartition)	// Add to self if possible
		elements[numElements++] = no;
	else if(children == 0)							// Not possible, self is full. Split to children, add to them
	{
		int best;
		Octnode* child;
		maxElements = tree->maxElementsPerPartition;

		// Split into children (as applicable) and add each element in self to appropriate child (if possible)
		for(unsigned int i = 0; i < numElements; i++)
		{
			best = BestChild(elements[i]->center);	// Find best child for element
			Split(1<<best);							// Create that child (if it hasn't been already)
			child = tree->LookUpNode(locationCode<<3|best);	// Get pointer to best child

			// If child can contain element, remove element from self and pass down to child
			if(child->Encapsulates(elements[i]->center, elements[i]->radius))
			{
				child->AddElement(elements[i]);		// Put into child

				// Slide remaining elements down in array (so array remains contiguous)
				for(unsigned int j = i; j < numElements-1; j++)
					elements[j] = elements[j+1];
				numElements--;
				i--;
			}
			// Element could not be store in child, remains in self
		}

		// Add new object passed in
		best = BestChild(no->center);	// Find best child for new object
		Split(1<<best);					// Create the child (if it hasn't been already)
		child = tree->LookUpNode(locationCode<<3|best);	// Get pointer to best child

		// If child can contain new object, add to child
		if(child->Encapsulates(no->center, no->radius))
			child->AddElement(no);
		else	// New Object could not be added to child, add to self
		{
			// If self is somehow full, resize array to allow storage of more NodeObjects
			if(numElements == maxElements)
				ResizeArray();

			// Add to self
			elements[numElements++] = no;
		}
	}
	else // Children couldn't contain object AND elements list is filled already
	{
		if(numElements == maxElements)
			ResizeArray();
		elements[numElements++] = no;
	}
}

void Octnode::PassUpObject(NodeObject* no)
{
	// This Octnode can take NodeObject, so add it in
	if(Encapsulates(no->center, no->radius))
	{
		// Attempt children first (first child might not hold it, but Object may have moved into a different child)
		int best = BestChild(no->center);	// Find best child for element
		Split(1<<BestChild(no->center));	// Create that child (if it hasn't been already)
		Octnode* child = tree->LookUpNode(locationCode<<3|best);	// Get pointer to best child

		// If new/different child can contain element, add to that child
		if(child->Encapsulates(no->center, no->radius))
			child->AddElement(no);
		else	// Object could not be store in new/different child, so add to self instead
		{
			if(numElements == maxElements)
				ResizeArray();
			elements[numElements++] = no;
		}
		return;
	}

	if(locationCode == 1)	// At root node and it still doesn't fit anywhere. Time to grow the tree
	{
		if(tree->resizeable)
		{
			numElementsBranch--;
			tree->Grow(no->o);
			delete no;
			return;
		}
		else
			assert(locationCode != 1);
	}
	else	// Can't take NodeObject, continue passing up to next parent
	{
		numElementsBranch--;	// One less object in branch now
		tree->LookUpNode(locationCode>>3)->PassUpObject(no);
		assert(numElementsBranch >= numElements);
	}
}

void Octnode::Split(unsigned int i)
{
	if(i & 1<<0 && !(children & 1<<0))
		tree->CreateNode(new Octnode(tree, Vector(boundingBox.center.x-boundingBox.halfWidths[0]/2, boundingBox.center.y-boundingBox.halfWidths[1]/2, boundingBox.center.z-boundingBox.halfWidths[2]/2),
			boundingBox.halfWidths[0], boundingBox.halfWidths[1], boundingBox.halfWidths[2], (locationCode<<3)|0));
	if(i & 1<<1 && !(children & 1<<1))
		tree->CreateNode(new Octnode(tree, Vector(boundingBox.center.x+boundingBox.halfWidths[0]/2, boundingBox.center.y-boundingBox.halfWidths[1]/2, boundingBox.center.z-boundingBox.halfWidths[2]/2),
				boundingBox.halfWidths[0], boundingBox.halfWidths[1], boundingBox.halfWidths[2], (locationCode<<3)|1));
	if(i & 1<<2 && !(children & 1<<2))
		tree->CreateNode(new Octnode(tree, Vector(boundingBox.center.x-boundingBox.halfWidths[0]/2, boundingBox.center.y-boundingBox.halfWidths[1]/2, boundingBox.center.z+boundingBox.halfWidths[2]/2),
				boundingBox.halfWidths[0], boundingBox.halfWidths[1], boundingBox.halfWidths[2], (locationCode<<3)|2));
	if(i & 1<<3 && !(children & 1<<3))
		tree->CreateNode(new Octnode(tree, Vector(boundingBox.center.x+boundingBox.halfWidths[0]/2, boundingBox.center.y-boundingBox.halfWidths[1]/2, boundingBox.center.z+boundingBox.halfWidths[2]/2),
				boundingBox.halfWidths[0], boundingBox.halfWidths[1], boundingBox.halfWidths[2], (locationCode<<3)|3));
	if(i & 1<<4 && !(children & 1<<4))
		tree->CreateNode(new Octnode(tree, Vector(boundingBox.center.x-boundingBox.halfWidths[0]/2, boundingBox.center.y+boundingBox.halfWidths[1]/2, boundingBox.center.z-boundingBox.halfWidths[2]/2),
				boundingBox.halfWidths[0], boundingBox.halfWidths[1], boundingBox.halfWidths[2], (locationCode<<3)|4));
	if(i & 1<<5 && !(children & 1<<5))
		tree->CreateNode(new Octnode(tree, Vector(boundingBox.center.x+boundingBox.halfWidths[0]/2, boundingBox.center.y+boundingBox.halfWidths[1]/2, boundingBox.center.z-boundingBox.halfWidths[2]/2),
				boundingBox.halfWidths[0], boundingBox.halfWidths[1], boundingBox.halfWidths[2], (locationCode<<3)|5));
	if(i & 1<<6 && !(children & 1<<6))
		tree->CreateNode(new Octnode(tree, Vector(boundingBox.center.x-boundingBox.halfWidths[0]/2, boundingBox.center.y+boundingBox.halfWidths[1]/2, boundingBox.center.z+boundingBox.halfWidths[2]/2),
				boundingBox.halfWidths[0], boundingBox.halfWidths[1], boundingBox.halfWidths[2], (locationCode<<3)|6));
	if(i & 1<<7 && !(children & 1<<7))
		tree->CreateNode(new Octnode(tree, Vector(boundingBox.center.x+boundingBox.halfWidths[0]/2, boundingBox.center.y+boundingBox.halfWidths[1]/2, boundingBox.center.z+boundingBox.halfWidths[2]/2),
				boundingBox.halfWidths[0], boundingBox.halfWidths[1], boundingBox.halfWidths[2], (locationCode<<3)|7));

	children |= i;
}

void Octnode::Merge()
{
	CONSOLEOUTPUT("Mergining Node " << locationCode << "\n");

	Octnode* temp;		// temporary pointer to child nodes
	NodeObject** e;		// temporary pointer to list of objects in children
	int count;			// number of objects in each child
	for(unsigned int i = 0; i < 8; i++)
	{
		// Child doesn't exist
		if(!(children & 1<<i)) continue;

		temp = tree->LookUpNode((locationCode<<3)|i);	// Get Child
		e = temp->GetElements();						// Get list of Objects in child
		count = temp->GetNumElements();					// Get number of objects in child
		CONSOLEOUTPUT("Moving " << count << " elements up from " << (int)(locationCode<<3|i) << "\n");

		// Add objects in child to self
		for(unsigned int j = 0; j < count; j++)
			elements[numElements++] = e[j];

		tree->DeleteNode((locationCode<<3)|i);
	}

	children = 0;
}

int Octnode::BestChild(const Vector v) const
{
	return  (v.x < boundingBox.center.x ? 0 : 1) + 
			(v.y < boundingBox.center.y ? 0 : 4) +
			(v.z < boundingBox.center.z ? 0 : 2);
}

int Octnode::GrowLocCode(int newCode)
{
	int count = 0;
	while(locationCode >> count != 1)
		count++;

	locationCode = (1 << (count+3)) | (newCode << count) | ((locationCode << (64-count)) >> (64-count));
	return locationCode;
}

int Octnode::ShrinkLocCode()
{
	int count = 0;
	while(locationCode >> count != 1)
		count++;

	locationCode = ((locationCode << 64-(count-3)) >> (64-(count-3)) | (1 << count-3));
	return locationCode;
}

void Octnode::AssignNewRoot(int c, int branch)
{
	children |= 1<<c;
	numElementsBranch = branch;
}

bool Octnode::Encapsulates(Vector c, gFloat rad) const
{
	NodeAABB test(boundingBox.center, Vector((boundingBox.halfWidths.x * tree->looseness) - rad, 
										 (boundingBox.halfWidths.y * tree->looseness) - rad,
										 (boundingBox.halfWidths.z * tree->looseness) - rad));
	return test.ContainsPoint(c);
}

void Octnode::ResizeArray()
{
	maxElements *= 2;
	NodeObject** temp = new NodeObject*[maxElements];
	for(unsigned int i = 0; i < numElements; i++)
		temp[i] = elements[i];

	delete [] elements;
	elements = temp;
}

void Octnode::UpdateBranch()
{
	numElementsBranch++;
	if(locationCode != 1)
		tree->LookUpNode(locationCode>>3)->UpdateBranch();
}

int Octnode::CalcNumBranch()
{
	int count = 0;
	if(children)
	{
		for(int i = 0; i < 8; i++)
			count += children & 1<<i ? tree->LookUpNode(locationCode<<3|i)->CalcNumBranch() : 0;
	}
	return count + numElements;
}