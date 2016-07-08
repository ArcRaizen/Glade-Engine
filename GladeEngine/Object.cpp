#include "Object.h"

using namespace Glade;

Object::Object() : position(Vector()), centroid(Vector()), acceleration(Vector()), inverseMass(0), isAwake(true), canSleep(true), useGravity(true), gravity(Vector::GRAVITY), properties(0), ID(Identification::ID())
#ifdef TRACK_MASS
				, mass(0)
#endif
	{ }

Object::Object(Vector p, Vector a, gFloat lDamp, bool ug, Vector grav) : position(p), centroid(p), acceleration(a), inverseMass(0), linearDamping(lDamp), isAwake(false), canSleep(true),  useGravity(ug), gravity(grav), properties(0), ID(Identification::ID())
#ifdef TRACK_MASS
																				, mass(0)
#endif
	{ }

// NOTE! This Constructor should only be used by Particles
Object::Object(Vector p, Vector a, gFloat iMass, gFloat lDamp, bool ug, Vector grav) : position(p), acceleration(a), inverseMass(iMass), linearDamping(lDamp), isAwake(true), canSleep(true), useGravity(ug), gravity(grav), properties(0)
#ifdef TRACK_MASS
																				, mass(0)
#endif
	{ }

Object::~Object()
{
	delete shaderResource;
}

bool Object::operator== (const Object& other) { return ID == other.ID; }
bool Object::operator< (const Object& other) { return ID < other.ID; }
unsigned int Object::GetID() { return ID; }

void Object::ApplyForce(const Vector& f)
{
	// Do not apply force to infinitely-massed Object
	if(inverseMass == 0) return;
	force += f;
	isAwake = true;
}

// Sleep
bool Object::GetAwake() const { return isAwake; }
void Object::SetCanSleep(bool sleep)
{
	canSleep = sleep;
	if(!canSleep && !isAwake)
		SetAwake();
}
bool Object::GetCanSleep() const { return canSleep; }

// Gravity
void Object::TurnGravityOn(Vector grav) { useGravity = true; gravity = grav; }
void Object::TurnGravityOff() { useGravity = false; }

// Force Generators
void Object::RegisterForceGenerator(int id)
{
	for(unsigned int i = 0; i < generatorIDs.size(); ++i)
	{
		if(generatorIDs[i] == id) return;
	}

	generatorIDs.push_back(id);
}

void Object::UnregisterForceGenerator(int id)
{
	for(unsigned int i = 0; i < generatorIDs.size(); ++i)
	{
		if(generatorIDs[i] == id)
		{
			generatorIDs.erase(generatorIDs.begin() + i);
			return;
		}
	}
}
std::vector<int> Object::GetRegisteredForceGenerators() { return generatorIDs; }

void Object::AllowSetPosition() { properties |= OVERRIDE_POSITION; }
void Object::DisallowSetPosition() { properties &= ~OVERRIDE_POSITION; }
bool Object::CheckAllowSetPosition() { return properties & OVERRIDE_POSITION; }
void Object::AllowSetCentroid() { properties |= OVERRIDE_CENTROID; }
void Object::DisallowSetCentroid() { properties &= ~OVERRIDE_CENTROID; }
bool Object::CheckAllowSetCentroid() { return properties & OVERRIDE_CENTROID; }
void Object::AllowSetVelocity() { properties |= OVERRIDE_VELOCITY; }
void Object::DisallowSetVelocity() { properties &= ~OVERRIDE_VELOCITY; }
bool Object::CheckAllowSetVelocity() { return properties & OVERRIDE_VELOCITY; }
void Object::AllowSetAcceleration() { properties |= OVERRIDE_ACCELERATION; }
void Object::DisallowSetAcceleration() { properties &= ~OVERRIDE_ACCELERATION; }
bool Object::CheckAllowSetAcceleration() { return properties & OVERRIDE_ACCELERATION; }

Vector Object::GetPosition() const { return position; }
void Object::SetPosition(const Vector& p) { if(!(properties & OVERRIDE_POSITION)) return;  position = p; }
void Object::SetPosition(const gFloat x, const gFloat y, const gFloat z)
{
	if(!(properties & OVERRIDE_POSITION)) return;
	position.x = x;
	position.y = y;
	position.z = z;
}
Vector Object::GetCentroid() const { return centroid; }
void Object::SetCentroid(const Vector& c) { if(!(properties & OVERRIDE_CENTROID)) return; centroid = c; }
void Object::SetCentroid(const gFloat x, const gFloat y, const gFloat z)
{
	if(!(properties & OVERRIDE_CENTROID)) return;
	centroid.x = x;
	centroid.y = y;
	centroid.z = z;
}
Vector Object::GetAcceleration() const { return acceleration; }
void Object::SetAcceleration(const Vector& a) { if(!(properties & OVERRIDE_ACCELERATION)) return; acceleration = a; }
void Object::SetAcceleration(const gFloat x, const gFloat y, const gFloat z)
{
	if(!(properties & OVERRIDE_ACCELERATION)) return;
	acceleration.x = x;
	acceleration.y = y;
	acceleration.z = z;
}

gFloat Object::GetMass() const
{
#ifdef TRACK_MASS
	return mass;
#endif
	return inverseMass == 0 ? G_MAX : ((gFloat)1.0 / inverseMass);
}
gFloat Object::GetInverseMass() const { return inverseMass; }

Matrix Object::GetTransformMatrix() const { return transformationMatrix; }
void Object::GetTransformMatrix(Matrix* m) const { *m = transformationMatrix; }

AABB Object::GetBoundingBox() const { return boundingBox; }
AABB& Object::GetBoundingBox() { return boundingBox; }

gFloat Object::GetRadius() const { return radius; }

gFloat Object::GetRadiusSquared() const { return radius * radius; }

void Object::SetHightlightColor(D3DXVECTOR4 c) { highlightColor = c; }

std::set<int> Object::GetHashIndices() { return hashIndices; }
void Object::SetHashIndices(std::set<int> indices) { hashIndices = indices; }