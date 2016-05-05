#include "Arcball.h"

using namespace Glade;

Arcball::Arcball(gFloat width, gFloat height, ArcballType t) : type(t)
{
	adjustWidth = 1.0f / (width * 0.5f);
	adjustHeight = 1.0f / (height * 0.5f);
	startVec = endVec = Vector(0.0f, 0.0f, 1.0f);
}

// Take coordinates of mouse on screen and convert to imaginary 3D Sphere with radius 1 at center of screen
Vector Arcball::MapToSphere(int x, int y)
{
	gFloat a, b, c;

	a = (x * adjustWidth) - 1.0f;
	b = 1.0f - (y * adjustHeight);

	// Compute square length of the vector from center of screen to given point
	gFloat length = (a * a) + (b * b);

	// If point is mapped outside of sphere
	if(length > 1.0f)
	{
		float norm = 1.0f / Sqrt(length);
		a *= norm;
		b *= norm;
		c = 0.0f;
	}
	else	// point is inside the sphere
		c = Sqrt(1.0f - length);

	return Vector(a, b, c).Normalized();
}

// Mouse Down - Save starting location of imminent drag
void Arcball::Click(int x, int y)
{
	if(type == ArcballType::SPHERICAL)
		startVec = MapToSphere(x, y);
	else
		startVec = Vector(x,y,0);
}

// Mouse Moved - Compute rotation between start and end points of mouse drag
Quaternion Arcball::Drag(int x, int y)
{
	if(type == ArcballType::SPHERICAL)
	{
		endVec = MapToSphere(x, y);
		Quaternion q(startVec, endVec);

		// Set current mouse coordinates to start of next drag
		//startVec = endVec;

		return q;
	}
	static gFloat pitch = 0.0f;
	static gFloat yaw = 0.0f;
	
	endVec = Vector(x,y,0);
	Vector delta = endVec - startVec;
	startVec = endVec;

/*	pitch += delta.y * 0.5f;
	yaw += delta.x * 0.5f;
	Clamp<gFloat>(pitch, -80.0f, 80.0f);
	yaw = FMod(yaw, 360.0f);
	return Matrix(yaw, pitch, 0).ConvertToQuaternion();
*/
	return Matrix(delta.y*DEG2RAD, delta.x*DEG2RAD, 0).ConvertToQuaternion();

	Quaternion rotX(Vector(1,0,0), (delta.y)*DEG2RAD * gFloat(0.5f));
	Quaternion rotY(Vector(0,1,0), (delta.x)*DEG2RAD * gFloat(0.5f));
	return rotY * rotX;
}

void Arcball::Resize(gFloat w, gFloat h)
{
	adjustWidth = 1.0f / (w * 0.5f);
	adjustHeight = 1.0f / (h * 0.5f);
	startVec = endVec = Vector(0.0f, 0.0f, 1.0f);
}

void Arcball::SetType(ArcballType t) { type = t; }