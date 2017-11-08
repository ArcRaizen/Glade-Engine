#include "World.h"

using namespace Glade;

World::World(int worldMin, int worldMax, int cellSize_, unsigned int maxContacts_, unsigned int iterations) : 
			contactResolver(iterations), maxContacts(maxContacts_), worldCoordinateMinimum(worldMin), worldCoordinateMaximum(worldMax), cellSize(cellSize_)
{
	contacts = new Contact[4];
	calculateIterations = (iterations == 0);

	AssertMsg((worldMax-worldMin) % cellSize_ == 0, "World cannot be evenly divided into cells with given World size and cell dimensions");
	worldHashWidth = (worldMax - worldMin) / cellSize;
	numBuckets = worldHashWidth * worldHashWidth * worldHashWidth;
	cellSizeConvFactor = gFloat(1.0f) / cellSize;
	hashTable = new SpatialHashCell*[numBuckets];
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
			auto bucket = hashTable[*j]->bucket;	// Get list of Objects/RigidBodies in that cell

			// Loop through each Object/RigidBody in cell, add to master list
			for(auto k = bucket.begin(); k != bucket.end(); ++k)
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

						// Query  Colliders' Collision Mask(s) to ensure these Colliders can collide(r)
						if(!aColliders[a]->QueryCollisionMask(bColliders[b]->GetCollisionType())) continue; 

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

void World::Render(Camera* cam)
{
#ifdef FRUSTUM_CULLING_BOXES
	for(unsigned int i = 0; i < numBuckets; ++i)
	{
		if(hashTable[i] == nullptr || hashTable[i]->bucket.size() == 0) continue;
		if(cam->IsBoxInFrustum(hashTable[i]->boundingBox) != Camera::OUTSIDE)
		{
			for(unsigned int j = 0; j < hashTable[i]->bucket.size(); ++j)
			{
#ifdef FRUSTUM_CULLING_RIGOROUS
				if(cam->IsBoxInFrustum(hashTable[i]->bucket[j]) != Camera::OUTSIDE)
#endif
				hashTable[i]->bucket[j]->Render();
			}
		}
	}
#else
	for(unsigned int i = 0; i < rigidBodies.size()l ++i)
	{
		if(cam->IsSphereInFrustum(rigidBodies[i]) != Camera::OUTSIDE)
			rigidBodies[i]->Render();
	}
#endif
}

#pragma region Spatial Hash
int World::Hash(Vector v)
{
	if(v.x < worldCoordinateMinimum || v.x > worldCoordinateMaximum) return -1;
	if(v.y < worldCoordinateMinimum || v.y > worldCoordinateMaximum) return -1;
	if(v.z < worldCoordinateMinimum || v.z > worldCoordinateMaximum) return -1;
	int x = (int)((int)(v.x-worldCoordinateMinimum) * cellSizeConvFactor);
	int y = (int)((int)(v.y-worldCoordinateMinimum) * cellSizeConvFactor) * worldHashWidth * worldHashWidth;
	int z = (int)((int)(v.z-worldCoordinateMinimum) * cellSizeConvFactor) * worldHashWidth;
	return x + y + z;

/*	int a = v.x / cellSize;
	int b = v.y / cellSize;
	int c = v.z / cellSize;
	int test = ((a * 73856093) ^ (b * 19349663) ^ (c * 83492791)) % numBuckets;

	unsigned int d = v.x / cellSize;
	unsigned int e = v.y / cellSize;
	unsigned int f = v.z / cellSize;
	int test2 = ((d * 73856093) ^ (e * 19349663) ^ (f * 83492791)) % numBuckets;

	unsigned __int64 x = (v.x + worldSize.x/2) / cellSize;
	unsigned __int64 y = (v.y + worldSize.y/2) / cellSize;
	unsigned __int64 z = (v.z + worldSize.z/2) / cellSize;

	int test3 =  ((x * 73856093) ^ (y * 19349663) ^ (z * 83492791)) % numBuckets;
	return test3;*/
}

// Return the AABB representing the Spatial Hash cell of the given index
AABB World::CalcHashCellBounds(int i)
{
	Vector min, max;
	min.y = i / (worldHashWidth * worldHashWidth);
	i -= min.y * (worldHashWidth * worldHashWidth);
	min.z = i / worldHashWidth;
	i -=min.z * worldHashWidth;
	min.x = i;
	min *= cellSize;
	min += Vector(worldCoordinateMinimum, worldCoordinateMinimum, worldCoordinateMinimum);

	max.x = min.x + cellSize;
	max.y = min.y + cellSize;
	max.z = min.z + cellSize;

	return AABB(min, max);
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
		{
			hashTable[*iter] = new SpatialHashCell;
			hashTable[*iter]->boundingBox = CalcHashCellBounds(*iter);
		}
		hashTable[*iter]->bucket.push_back(o);
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

SpatialHashCell* World::QueryHash(Vector v)
{
	return hashTable[Hash(v)];
}

void World::RemoveFromHash(RigidBody* o)
{
	std::set<int> indices = o->GetHashIndices();
	SpatialHashCell* cell;
	std::vector<RigidBody*>::iterator i;
	for(auto iter = indices.begin(); iter != indices.end(); ++iter)
	{
		cell = hashTable[*iter];
		i = std::find(cell->bucket.begin(), cell->bucket.end(), o);
		cell->bucket.erase(i, i+1);
	}
}

// Return the 1st Object in the world that collides with given Ray
// The distance along the ray is "returned" via the 't' parameter by reference
// Source: http://www.cse.yorku.ca/~amana/research/grid.pdf, http://stackoverflow.com/questions/12367071/how-do-i-initialize-the-t-variables-in-a-fast-voxel-traversal-algorithm-for-ray
Object* World::RayCast(Ray ray, gFloat& t, int mask)
{
	// Pre-define variables before using them
	SpatialHashCell* cell;
	std::vector<Collider*> colliders;
	std::set<unsigned int> objectIDs;
	unsigned int id, numC, index;
	gFloat dist = 0;

	// Get parameters for moving down the ray incrementally
	Vector start = ray.origin;
	Vector delta = ray.GetEndPoint() - start;
	Vector tDelta, tMax;
	CalcRaycastParams(ray, tDelta, tMax);

	// Move down ray until it enters a new cell, then check the new cell for collisions
	do
	{	
		// Check new cell for Objects
		index = Hash(start + delta*dist);
		if(index >= numBuckets) return nullptr;
		cell = hashTable[index];
		if(cell != nullptr)
		{
			// Check each Object in cell for collision with ray
			for(unsigned int i = 0; i < cell->bucket.size(); ++i)
			{
				// Make sure we haven't checked this Object already (Objects can be in multiple cells)
				id = cell->bucket[i]->GetID();
				if(objectIDs.count(id) == 0)
				{
					//objectIDs.insert(id);

					// Check each Collider for each Object
					numC = cell->bucket[i]->GetColliders(colliders);
					for(unsigned int j = 0; j < numC; ++j)
					{
						// Check that the Collider is enabled and matches the collision mask of the ray
						if(colliders[j]->IsEnabled() && colliders[j]->QueryCollisionMask(mask))
						{
							// Actually test collider against ray
							if(CollisionTests::RayAABBTest(ray, colliders[j]->GetBounds(), t))
							//if(CollisionTests::RaySphereTest(ray, colliders[j]->GetPosition(), colliders[j]->GetBounds().GetRadius(), t))
								return cell->bucket[i];	// if collision, return the object
						}
					}
				}
			}
		}

		// Choose which direction we move to reach a new cell
		if(tMax.x < tMax.y)
		{
			if(tMax.x < tMax.z)
			{
				//dist += tMax.x;
				tMax.x += tDelta.x;
				dist = tMax.x;
			}
			else
			{
				//dist += tMax.z;
				tMax.z += tDelta.z;
				dist = tMax.z;
			}
		}
		else
		{
			if(tMax.y < tMax.z)
			{
				//dist += tMax.y;
				tMax.y += tDelta.y;
				dist = tMax.y;
			}
			else
			{
				//dist += tMax.z;
				tMax.z += tDelta.z;
				dist = tMax.z;
			}
		}
	} while(dist < 1.0f && tMax.x < 1.0f && tMax.y < 1.0f && tMax.z < 1.0f);

	// No collision with ray
	return nullptr;
}

// Just like RayCast, but doesn't stop after the first collision and returns ALL Objects that collide with the Ray
// Return is a vector of pairs: an Object that collides with the ray, and the distance along the ray 't'
std::vector<std::pair<Object*, gFloat>> World::RayCastPenetrate(Ray ray, int mask)
{
	// Pre-define variables before using them
	std::vector<std::pair<Object*, gFloat>> objects;
	SpatialHashCell* cell;
	std::vector<Collider*> colliders;
	std::set<unsigned int> objectIDs;
	unsigned int id, numC, index;
	gFloat dist = 0, t;

	// Get parameters for moving down the ray incrementally
	Vector start = ray.origin;
	Vector delta = ray.GetEndPoint() - start;
	Vector tDelta, tMax;
	CalcRaycastParams(ray, tDelta, tMax);

	// Move down ray until it enters a new cell, then check the new cell for collisions
	do
	{	
		// Check new cell for Objects
		index = Hash(start + delta*dist);
		if(index >= numBuckets) continue;
		cell = hashTable[index];
		if(cell != nullptr)
		{
			// Check each Object in cell for collision with ray
			for(unsigned int i = 0; i < cell->bucket.size(); ++i)
			{
				// Make sure we haven't checked this Object already (Objects can be in multiple cells)
				id = cell->bucket[i]->GetID();
				if(objectIDs.count(id) == 0)
				{
					objectIDs.insert(id);

					// Check each Collider for each Object
					numC = cell->bucket[i]->GetColliders(colliders);
					for(unsigned int j = 0; j < numC; ++j)
					{
						// Check that the Collider is enabled and matches the collision mask of the ray
						if(colliders[j]->IsEnabled() && colliders[j]->QueryCollisionMask(mask))
						{
							// Actually test collider against ray
							if(CollisionTests::RayAABBTest(ray, colliders[j]->GetBounds(), t))
								objects.push_back(std::make_pair(cell->bucket[i], t));	// if collision, Save object to return
						}
					}
				}
			}
		}

		// Choose which direction we move to reach a new cell
		if(tMax.x < tMax.y)
		{
			if(tMax.x < tMax.z)
			{
				dist += tMax.x;
				tMax.x += tDelta.x;
			}
			else
			{
				dist += tMax.z;
				tMax.z += tDelta.z;
			}
		}
		else
		{
			if(tMax.y < tMax.z)
			{
				dist += tMax.y;
				tMax.y += tDelta.y;
			}
			else
			{
				dist += tMax.z;
				tMax.z += tDelta.z;
			}
		}
	} while(tMax.x < 1.0f && tMax.y < 1.0f && tMax.z < 1.0f);

	// Return complete list of all Objects penetrated by ray
	return objects;
}

// Calculate the parameters necessary to step along the Ray so that each step enters
// the next cell in the spatial hash
void World::CalcRaycastParams(Ray ray, Vector& tDelta, Vector& tMax)
{
	Vector start = ray.origin;
	Vector delta = ray.GetEndPoint() - start;
	gFloat size = gFloat(cellSize);

	
	// Fraction of ray that is needed to be traveled to cross an entire cell along each axis 
	tDelta.x = Abs(delta.x) > EPSILON ? size / Abs(delta.x) : G_MAX;
	tDelta.y = Abs(delta.y) > EPSILON ? size / Abs(delta.y) : G_MAX;
	tDelta.z = Abs(delta.z) > EPSILON ? size / Abs(delta.z) : G_MAX;

	// Fraction of ray that is needed to be traveled to reach the border of current cell along each axis
	// ............just trust me on this one
	tMax.x = tDelta.x * (gFloat(1.0f) - (fmodf(SameSign(start.x, delta.x) ? Abs(start.x) : Abs(start.x + size * Ceiling(Abs(start.x / size)) * (delta.x > 0 ? 1 : -1)), size) / size));
	tMax.y = tDelta.y * (gFloat(1.0f) - (fmodf(SameSign(start.y, delta.y) ? Abs(start.y) : Abs(start.y + size * Ceiling(Abs(start.y / size)) * (delta.y > 0 ? 1 : -1)), size) / size));
	tMax.z = tDelta.z * (gFloat(1.0f) - (fmodf(SameSign(start.z, delta.z) ? Abs(start.z) : Abs(start.z + size * Ceiling(Abs(start.z / size)) * (delta.z > 0 ? 1 : -1)), size) / size));
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