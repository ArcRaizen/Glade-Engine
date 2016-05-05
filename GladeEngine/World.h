#pragma once
#ifndef GLADE_WORLD_H
#define GLADE_WORLD_H

#include "GladeConfig.h"
#include "RigidBody.h"
//#include "Force Generators\ForceGenerator.h"
//#include "Contact Generators\ContactGenerator.h"
#include "Contacts\ContactResolver.h"
#include "CollisionTests.h"
#include <algorithm>
#include <map>

namespace Glade {
/*
	Keeps track of a set of Rigid Bodies and provides the means to update all of them.
*/
class World
{
public:
	World(unsigned int maxContacts_, unsigned int iterations=0, Vector size=Vector(1000.0f,1000.0f,1000.0f));
	~World();

	unsigned int GenerateContacts();
	void PhysicsUpdate(gFloat dt);

	void AddRigidBody(RigidBody* rb);
//	void AddForceGenerator(ForceGenerateor* fg);
//	void AddContactGenerator(ContactGenerator* cg);

	std::vector<RigidBody*>& GetRigidBodies();
//	std::map<int, ForceGenerator*>& GetForceGenerators();
//	std::vector<ContactGenerator*>& GetContactGenerators();

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
	std::set<int>			GetHashIndices(RigidBody* o);
	void					ClearHash();
	void					AddToHash(RigidBody* o);
	void					UpdateHashedObject(RigidBody* o);
	std::vector<RigidBody*>*QueryHash(Vector v);
	void					RemoveFromHash(RigidBody* o);
	bool					RayCast(Vector start, Vector dir, int mask, gFloat len=G_MAX);

	std::vector<RigidBody*>**	hashTable;
	int							numBuckets;
	int							cellSize;
	Vector						worldSize;		// There is an issue with Spatial Hashing in that the hashing algorithm will not work properly for negative positions.
												// It will return negative indices for negative positions, which will error when attempting to place/locate objects in the hash.
												// To resolve this, a "Maximum Size" of the world is to be given, and all positions will be offset by half that size when being hashed
												// so that real positions from -worldSize/2 -> worldSize/2 will be valid.
												// This way, objects can be located at (-100, 50, 0) instead of having to have everything shifted several hundred units positive.
												// Multiple options for what actions to take when an Object exceeds this "Maximum Size" of the world will be provided and can be
												// swapped between with pre-processor defined macros in "Core.h"
};
}	// namespace
#endif	// GLADE_WORLD_H