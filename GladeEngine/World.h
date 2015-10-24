#pragma once
#ifndef GLADE_WORLD_H
#define GLADE_WORLD_H

#include "Core.h"
#include "RigidBody.h"
//#include "Force Generators\ForceGenerator.h"
//#include "Contact Generators\ContactGenerator.h"
#include "ContactResolver.h"
#include "CollisionTests.h"
#include <algorithm>
#include <set>
#include <map>

namespace Glade {
/*
	Keeps track of a set of Rigid Bodies and provides the means to update all of them.
*/
class World
{
public:
	World(unsigned int maxContacts_, unsigned int iterations=0);
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

	bool calculateIterations;
	unsigned int maxContacts;

	// Engine/Game can run on variable framerate, but physicss runs at a fixed rate
	// This tracks time as it passes and updates physics properly at fixed steps
	gFloat timeAccumulator;

// ~~~~ SPATIAL HASH ~~~~
	int						Hash(Vector v);
	void					ClearHash();
	void					AddToHash(Object* o);
	void					UpdateHashedObject(Object* o);
	std::vector<Object*>*	QueryHash(Vector v);
	void					RemoveFromHash(Object* o);
	bool					RayCast(Vector start, Vector dir, int mask, gFloat len=G_MAX);

	std::vector<Object*>**	hashTable;
	std::set<int>			hashIndices;
	int						numBuckets;
	int						cellSize;
};
}	// namespace
#endif	// GLADE_WORLD_H