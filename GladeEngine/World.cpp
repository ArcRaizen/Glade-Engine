#include "World.h"

using namespace Glade;

World::World(unsigned int maxContacts_, unsigned int iterations, Vector size/*=Vector(1000.0f,1.000.0f,1000.0f)*/) : 
			contactResolver(iterations), maxContacts(maxContacts_), worldSize(size), timeAccumulator(0.0f)
{
	contacts = new Contact[4];
	calculateIterations = (iterations == 0);

	numBuckets = 1000;
	cellSize = 5;
	hashTable = new std::vector<RigidBody*>*[numBuckets];
	for(int i = 0; i < numBuckets; ++i)
		hashTable[i] = nullptr;
}


World::~World()
{
	delete[] contacts;
	for(int i = 0; i < numBuckets; ++i)
		delete hashTable[i];
	delete[] hashTable;
}

unsigned int World::GenerateContacts()
{
	unsigned int limit = maxContacts;
	int used;
	static int tests = 0;

// ~~~~ GENERATE CONTACTS VIA COLLISION DETECTION ~~~~
	std::set<RigidBody*> bodies;
	std::set<std::pair<unsigned int, unsigned int>> testedPairs;
	std::vector<Collider*> aColliders, bColliders;
	unsigned int iID, jID;
	unsigned int aSize, bSize;

	// Loop through each Object/RigidBody in the World
	for(auto i = rigidBodies.begin(); i != rigidBodies.end(); ++i)
	{
		auto indices = (*i)->GetHashIndices();	// Get the indices of the hash cell it's in

		// Loop through each hash cell it's in
		for(auto j = indices.begin(); j != indices.end(); ++j)
		{
			auto bucket = hashTable[*j];	// Get list of Objects/RigidBodies in that cell

			// Loop through each Object/RigidBody in cell, add to master list
			for(auto k = bucket->begin(); k != bucket->end(); ++k)
				bodies.insert(*k);
		}

		// Remove current RigidBody from list
		bodies.erase(*i);
		// 'objects' now contains pointer to every Object/RigidBody that *might* collide with current RigidBody

		for(auto j = bodies.begin(); j != bodies.end(); ++j)
		{
			iID = (*i)->GetID();
			jID = (*j)->GetID();
			if(!testedPairs.insert(std::make_pair((iID<jID?iID:jID), (iID>jID?iID:jID))).second)
				continue;

			// Find correct ContactBatch (if it exists) to add possible Contact to
			// If correct ContactBatch does not exist, create it.
			// If both Objects already collided (from a previous bucket), ignore them
			int batch1 = -1, batch2 = -1;
			unsigned int result;
			ContactBatch* batch = nullptr;
			for(unsigned int k = 0; k < contactBatches.size(); ++k)
			{
				result = contactBatches[k]->ContainsRigidBodies(iID, jID);
					
				// Batch found that contains both Objects already
				// There's a triangle of collisions (we have A-B and A-C already, now we found B-C)
				if(result == 2) 
				{ batch1 = k; batch = contactBatches[k]; break; }
				// Batch found that contains one of the Objects
				else if(result == 1)
				{
					if(batch1 == -1) { batch1 = k; batch = contactBatches[k]; }
					else			 batch2 = k;
				}
			}

			// If the AABB intersect, do more rigorous testing (actual collider tests)
			// TODO - AABB'S NEARLY IN CONTACT BUT NOT QUITE SHOULD BE ADDED TO CONTACT BATCHES
			// IN CASE INTERPENETRATION RESOLUTION OF NEARBY CONTACTS ENDS UP AFFECTING THEM 
			// BY PROXY. CREATE CONTACT WITH NEGATIVE PENETRATION
			if(CollisionTests::AABBTest((*i)->GetBoundingBox(), (*j)->GetBoundingBox()))
			{
				// Get Collider(s) of each Object
				aSize = (*i)->GetColliders(aColliders);
				bSize = (*j)->GetColliders(bColliders);

				// Test all Colliders of each Object against all Colliders of the other
				for(unsigned int a = 0; a < aSize; ++a)
				{
					// Collider 'a' must be Enabled
					if(!aColliders[a]->IsEnabled()) continue;

					for(unsigned int b = 0; b < bSize; ++b)
					{
						// Collider 'b' must be Enabled
						if(!bColliders[b]->IsEnabled()) continue;

						// Query both Colliders' Collision Masks to ensure these Colliders can collider
						//if(!aColliders[a]->QueryCollisionMask(TEST) || !bColliders[b]->QueryCollisionMask(TEST2)) continue;

						// No pre-set reason why Colliders cannot collide - Actually test for intersection now
						used = CollisionTests::TestCollision(aColliders[a], bColliders[b], contacts);							
						limit -= used;

						if(used)
						{
							// Need to create new ContactBatch
							if(batch1 == -1)
							{
								batch = new ContactBatch();
								contactBatches.push_back(batch);
							}
							// Need to MERGE two ContactBatches
							else if(batch2 != -1)
							{
								contactBatches[batch1]->MergeBatch(contactBatches[batch2]);
								delete contactBatches[batch2];
								contactBatches.erase(contactBatches.begin() + batch2);
								batch = contactBatches[batch1];
							}


							// Add generated Contacts to ContactBatch
							for(unsigned int l = 0; l < used; ++l)
								batch->AddContact(contacts[l]);
						}
					}
				}
			}
		}
	}

// ~~~~ GENERATE CONTACTS VIA CONTACT GENERATORS ~~~~
/*	for(auto i = contactGenerators.begin(); i != contactGenerators.end(); ++i)
	{
		used = (*i)->GenerateContact(nextContact, limit);
		
		if(used < 0)
		{
			delete *i;
			i = contactGenerators.erase(i);
			i--;
			continue;
		}

		limit -= used;
		contactsUsedThisFrame += used;
		nextContact += used;

		if(limit <= 0) break;
	}
*/
	return maxContacts - limit;
}

void World::PhysicsUpdate(gFloat dt)
{
	// Accumulate the time that passes between the last frame and now
	timeAccumulator += dt;

	while(timeAccumulator >= PHYSICS_TIMESTEP)
	{
		// Apply Force Generators and Integrate
		for(auto i = rigidBodies.begin(); i != rigidBodies.end(); ++i)
		{
			//auto ids = (*i)->GetRegistedForceGenerators();
			//for(auto j = ids.begin(); j != ids.end() ++j)
				//forceGeneratos[*j]->GenerateForce(*i);

			// Integrate - If Object is moving, rehash it in the Spatial Hash
			if((*i)->Update())
				UpdateHashedObject(*i);
		}

		 // Generate and process (if necessary) contacts
		unsigned int usedContacts = GenerateContacts();
		if(usedContacts)
		{
			if(calculateIterations)
				contactResolver.SetIterations(usedContacts*3);
			for(unsigned int i = 0; i < contactBatches.size(); ++i)
			{
				contactResolver.ResolveContacts(contactBatches[i]);
				delete contactBatches[i];
			}

			// Clear batches for next frame
			contactBatches.clear();
		}

		// Now we have spent one frame of time
		timeAccumulator -= PHYSICS_TIMESTEP;
	}
}

#pragma region Spatial Hash
int World::Hash(Vector v)
{
	unsigned __int64 x = (v.x + worldSize.x/2) / cellSize;
	unsigned __int64 y = (v.y + worldSize.y/2) / cellSize;
	unsigned __int64 z = (v.z + worldSize.z/2) / cellSize;

	return ((x * 73856093) ^ (y * 19349663) ^ (z * 83492791)) % numBuckets;
}

std::set<int> World::GetHashIndices(RigidBody* o)
{
	std::set<int> indices;

	// Calculate number of cells between BoundingBox min/max
	AABB bounds = o->GetBoundingBox();
	int diffX = std::floor((((((int)bounds.maximum.x / cellSize) * cellSize) + cellSize) - bounds.minimum.x) / cellSize);
	int diffY = std::floor((((((int)bounds.maximum.y / cellSize) * cellSize) + cellSize) - bounds.minimum.y) / cellSize);
	int diffZ = std::floor((((((int)bounds.maximum.z / cellSize) * cellSize) + cellSize) - bounds.minimum.z) / cellSize);

	// Step between BoundingBox min/max along all 3 axes and hash object into all cells it intersects
	for(int x = 0; x <= diffX; ++x)
	{
		for(int y = 0; y <= diffY; ++y)
		{
			for(int z = 0; z <= diffZ; ++z)
				indices.insert(Hash(bounds.minimum + Vector(x*cellSize, y*cellSize, z*cellSize)));
		}
	}

	return indices;
}

void World::ClearHash()
{
	for(int i = 0; i < numBuckets; ++i)
		delete hashTable[i];
}

void World::AddToHash(RigidBody* o)
{
	std::set<int> indices = GetHashIndices(o);

	for(auto iter = indices.begin(); iter != indices.end(); ++iter)
	{
		if(hashTable[*iter] == nullptr)
			hashTable[*iter] = new std::vector<RigidBody*>;
		hashTable[*iter]->push_back(o);
	}

	// Save the list of cells/indices in hash intersected by this Object
	o->SetHashIndices(indices);
}

void World::UpdateHashedObject(RigidBody* o)
{
	int hIndex;

	// list of hash indices after/before Object moved
	std::set<int> updatedIndices;
	std::set<int> oldIndices = o->GetHashIndices();

	// Iterators to track 'end' of currentIndices list
/*	auto oldEnd = oldIndices.end();
	std::vector<int>::iterator newEnd;

	// Calculate number of cells between BoundingBox min/max
	AABB bounds = o->GetBoundingBox();
	int diffX = std::floor((((((int)bounds.max.x / cellSize) * cellSize) + cellSize) - bounds.min.x) / cellSize);
	int diffY = std::floor((((((int)bounds.max.Y / cellSize) * cellSize) + cellSize) - bounds.min.Y) / cellSize);
	int diffZ = std::floor((((((int)bounds.max.Z / cellSize) * cellSize) + cellSize) - bounds.min.Z) / cellSize);

	// Step between BoundingBox min/max along all 3 axes and hash object into all cells it intersects
	for(int x = 0; x < diffX; ++x)
	{
		for(int y = 0; y < diffY; ++y)
		{
			for(int z = 0; z < diffZ; ++z)
			{
				// Attempt to remove hash index from list of old hased indices
				//	If index already exists in old list/set, it will be removed
				hIndex = Hash(bounds.min + Vector(x*cellSize, y*cellSize, z*cellSize));
				newEnd = std::remove(oldIndices.begin(), oldIndices.end(), hIndex);

				// New index not in old set, add Object to new hash index
				if(newEnd != oldEnd)
				{
					updatedIndices.push_back(hIndex);
					if(hashTable[hIndex] == nullptr)
						hashTable[hIndex] = new std::vector<Object*>;
					hashTable[hIndex]->push_back(o);
					hashIndices.insert(hIndex);
					oldEnd = newEnd;
				}
			}
		}
	}

	// OldIndices list now only contains hash indices of indices Object DOESNT hash to anymore
	// Remove Object from all those indices
	for(auto iter = oldIndices.begin(); iter != newEnd; ++iter)
	{
		std::remove(hashTable[*iter]->begin(), hashTable[*iter]->end(), o);
		hashIndices.erase(*iter);
	}
*/
	RemoveFromHash(o);
	AddToHash(o);

	// Set Object's new list of cell/indices
//	o->SetHashIndices(updatedIndices);
}

std::vector<RigidBody*>* World::QueryHash(Vector v)
{
	return hashTable[Hash(v)];
}

void World::RemoveFromHash(RigidBody* o)
{
	std::set<int> indices = o->GetHashIndices();
	std::vector<RigidBody*>* bucket;
	std::vector<RigidBody*>::iterator i;
	for(auto iter = indices.begin(); iter != indices.end(); ++iter)
	{
		bucket = hashTable[*iter];
		i = std::find(bucket->begin(), bucket->end(), o);
		bucket->erase(i, i+1);
	}
}

bool World::RayCast(Vector start, Vector dir, int mask, gFloat len/*=G_MAX*/)
{
	std::vector<RigidBody*>* bucket;
	Vector end = start + dir*len;
	Vector diff = end - start;

	int prevHash = -1, curHash = -2;
	gFloat stepLength = 1.0f;
	for(gFloat step = 0.0f; step < len; step+=stepLength)
	{
		curHash = Hash(start + dir*step);

		// Don't check Bucket we have already checked
		if(prevHash == curHash) continue;
		prevHash = curHash;

		// Don't check Bucket if there is nothing in it
	//	if(!hashIndices.count(curHash)) continue;

		// Loop through each object in Bucket
		bucket = hashTable[curHash];
		for(auto iter = bucket->begin(); iter != bucket->end(); ++iter)
		{
			// Raycast test against it
		}
	}

	return false;
}

#pragma endregion

void World::AddRigidBody(RigidBody* rb)
{
	rigidBodies.push_back(rb);
	AddToHash(rb);
}

/*
void World::AddForceGenerator(ForceGenerator* fg)
{
	forceGenerators[fg->GetId()] = fg;
}


void World::AddContactGenerator(ContactGenerator* cg)
{
	contactGenerators.push_back(cg);
}
*/

std::vector<RigidBody*>& World::GetRigidBodies()
{
	return rigidBodies;
}

/*
std::map<int, ForceGenerator*>& World::GetForceGenerators()
{
	return forceGenerators;
}

std::vector<ContactGenerator*>& World::GetContactGenerators()
{
	return contactGenerators;
}
*/