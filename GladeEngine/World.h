#pragma once
#ifndef GLADE_WORLD_H
#define GLADE_WORLD_H

#include "GladeConfig.h"
#include "RigidBody.h"
//#include "Force Generators\ForceGenerator.h"
//#include "Contact Generators\ContactGenerator.h"
#include "Contacts\ContactResolver.h"
#include "CollisionTests.h"
#include "System\Camera.h"
#include <algorithm>
#include <map>

namespace Glade {
struct SpatialHashCell
{
	AABB boundingBox;
	std::vector<RigidBody*> bucket;
	unsigned int index;
};

/*
	Keeps track of a set of Rigid Bodies and provides the means to update all of them.
*/
class World
{
public:
	World(int worldMin, int worldMax, int cellSize_, unsigned int maxContacts_, unsigned int iterations=0);
	~World();

	unsigned int GenerateContacts();
	void PhysicsUpdate(gFloat dt);

	void AddRigidBody(RigidBody* rb);
//	void AddForceGenerator(ForceGenerateor* fg);
//	void AddContactGenerator(ContactGenerator* cg);

	std::vector<RigidBody*>& GetRigidBodies();
//	std::map<int, ForceGenerator*>& GetForceGenerators();
//	std::vector<ContactGenerator*>& GetContactGenerators();

	void Render(Camera* cam);

protected:
	// List of all Rigid Bodies that exist
	std::vector<RigidBody*> rigidBodies;

	// Map of all Force Generators and their unique IDs
//	std::map<int, ForceGenerator*> forceGenerators;

	// List of all Contact Generators
//	std::vector<ContactGenerator*> contactGenerators;

	// Contact Resolver that calculates and resolves all contacts each frame
	ContactResolver contactResolver;

	// List of all Contacts that occured and are processed each frame
	Contact* contacts;
	std::vector<ContactBatch*> contactBatches;

	bool calculateIterations;
	unsigned int maxContacts;

	// Engine/Game can run on variable framerate, but physicss runs at a fixed rate
	// This tracks time as it passes and updates physics properly at fixed steps
	gFloat timeAccumulator;

// ~~~~ SPATIAL HASH ~~~~
	int						Hash(Vector v);
	AABB					CalcHashCellBounds(int i);
	std::set<int>			GetHashIndices(RigidBody* o);
	void					ClearHash();
	void					AddToHash(RigidBody* o);
	void					UpdateHashedObject(RigidBody* o);
	SpatialHashCell*		QueryHash(Vector v);
	void					RemoveFromHash(RigidBody* o);
public:
	Object*					RayCast(Ray ray, gFloat& t, int mask);
	std::vector<std::pair<Object*, gFloat>>	
							RayCastPenetrate(Ray ray, int mask);
	void					CalcRaycastParams(Ray ray, Vector& tDelta, Vector& tMax);

	SpatialHashCell**	hashTable;
	gFloat worldCoordinateMinimum,			// Minimum and maximum coordinates in the world (Vector(min,min,min) and Vector(max,max,max))
		worldCoordinateMaximum;					// World is always square
	gFloat cellSize;						// Dimension of each cell in hashed world (cubic cells)
	gFloat cellSizeConvFactor;
	int worldHashWidth, numBuckets;			// width = number of cells across one dimension of the hashed world,
											// numBuckets = total number of cells across all dimensions of the hashed world
};
}	// namespace
#endif	// GLADE_WORLD_H