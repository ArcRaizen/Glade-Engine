#include "World.h"

using namespace Glade;

World::World(unsigned int maxContacts_, unsigned int iterations) : contactResolver(iterations), maxContacts(maxContacts_), timeAccumulator(0.0f)
{
	contacts = new Contact[maxContacts];
	calculateIterations = (iterations == 0);

	numBuckets = 1000;
	cellSize = 5;
	hashTable = new std::vector<Object*>*[numBuckets];
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
	Contact* nextContact = contacts;

// ~~~~ GENERATE CONTACTS VIA COLLISION DETECTION ~~~~
	std::vector<Object*>* bucket;
	std::vector<Collider*> aColliders, bColliders;
	unsigned int aSize, bSize;
	for(auto iter = hashIndices.begin(); iter != hashIndices.end(); ++iter)
	{
		// Get each bucket that has at least 1 object hashed in it
		bucket = hashTable[*iter];

		// Loop through each pair of Objects in a bucket
		for(unsigned int i = 0; i < bucket->size(); ++i)
		{
			for(unsigned int j = i+1; j < bucket->size(); ++j)
			{
				// If the AABB intersect, do more rigorous testing (actual collider tests)
				if(CollisionTests::AABBTest((*bucket)[i]->GetBoundingBox(), (*bucket)[j]->GetBoundingBox()))
				{
					// Get Collider(s) of each Object
					aSize = (*bucket)[i]->GetColliders(aColliders);
					bSize = (*bucket)[j]->GetColliders(bColliders);

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
							used = CollisionTests::TestCollision(aColliders[a], bColliders[b], nextContact);							
							limit -= used;
							nextContact += used;

							if(used)
							{
								return used;
							}
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
			contactResolver.ResolveContacts(contacts, usedContacts);
		}

		// Now we have spent one frame of time
		timeAccumulator -= PHYSICS_TIMESTEP;
	}
}

#pragma region Spatial Hash
int World::Hash(Vector v)
{
	int x = (int)((v.x<0 && v.x>-5) ? v.x-5.0f : v.x) / cellSize;
	int y = (int)((v.y<0 && v.y>-5) ? v.y-5.0f : v.y) / cellSize;
	int z = (int)((v.z<0 && v.z>-5) ? v.z-5.0f : v.z) / cellSize;
	return ((x * 73856093) ^ (y * 19349663) ^ (z * 83492791)) % numBuckets;

	/*
		PROBLEM!!!!!
		-4.99 <-> 4.99 map to 0
		5 <-> 9.99 map to 1
		-9.99 <-> 5 map to -1
		NEED TO USE POSTIVE POSITIONS ONLY
		NOTHING IN THE WORLD CAN BE NEGATIVE
	*/
}

void World::ClearHash()
{
	for(int i = 0; i < numBuckets; ++i)
		delete hashTable[i];
	hashIndices.clear();
}

void World::AddToHash(Object* o)
{
	int hIndex;
	std::vector<int> indices;

	// Calculate number of cells between BoundingBox min/max
	AABB bounds = o->GetBoundingBox();
	int diffX = std::floor((((((int)bounds.max.x / cellSize) * cellSize) + cellSize) - bounds.min.x) / cellSize);
	int diffY = std::floor((((((int)bounds.max.y / cellSize) * cellSize) + cellSize) - bounds.min.y) / cellSize);
	int diffZ = std::floor((((((int)bounds.max.z / cellSize) * cellSize) + cellSize) - bounds.min.z) / cellSize);

	// Step between BoundingBox min/max along all 3 axes and hash object into all cells it intersects
	for(int x = 0; x <= diffX; ++x)
	{
		for(int y = 0; y <= diffY; ++y)
		{
			for(int z = 0; z <= diffZ; ++z)
			{
				hIndex = Hash(bounds.min + Vector(x*cellSize, y*cellSize, z*cellSize));
				indices.push_back(hIndex);
				if(hashTable[hIndex] == nullptr)
					hashTable[hIndex] = new std::vector<Object*>;
				hashTable[hIndex]->push_back(o);
				hashIndices.insert(hIndex);
			}
		}
	}

	// Save the list of cells/indices in hash intersected by this Object
	o->SetHashIndices(indices);
}

void World::UpdateHashedObject(Object* o)
{
	int hIndex;

	// list of hash indices after/before Object moved
	std::vector<int> updatedIndices;
	std::vector<int> oldIndices = o->GetHashIndices();

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

std::vector<Object*>* World::QueryHash(Vector v)
{
	return hashTable[Hash(v)];
}

void World::RemoveFromHash(Object* o)
{
	std::vector<int> indices = o->GetHashIndices();
	std::vector<Object*>* bucket;
	std::vector<Object*>::iterator i;
	for(auto iter = indices.begin(); iter != indices.end(); ++iter)
	{
		bucket = hashTable[*iter];
		i = std::find(bucket->begin(), bucket->end(), o);
		bucket->erase(i, i+1);
	}
}

bool World::RayCast(Vector start, Vector dir, int mask, gFloat len/*=G_MAX*/)
{
	std::vector<Object*>* bucket;
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
		if(!hashIndices.count(curHash)) continue;

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