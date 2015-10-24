#include "Arcball.h"

Vector startVec;
Vector endVec;
float adjustWidth = 1.0f;
float adjustHeight = 1.0f;


Arcball::Arcball(float width, float height)
{
	adjustWidth = 1.0f / (width * 0.5f);
	adjustHeight = 1.0f / (height * 0.5f);
}


Arcball::~Arcball()
{
}

// Take coordinates of mouse on screen and convert to imaginary 3D Sphere with radius 1 at center of screen
Vector Arcball::MapToSphere(int x, int y)
{
	float a, b, c;

	a = (x * adjustWidth) - 1.0f;
	b = 1.0f - (y * adjustHeight);

	// Compute square length of the vector from center of screen to given point
	float length = (a * a) + (b * b);

	// If point is mapped outside of sphere
	if(length > 1.0f)
	{
		float norm = 1.0f / sqrtf(length);
		a *= norm;
		b *= norm;
		c = 0.0f;
	}
	else	// point is inside the sphere
		c = sqrtf(1.0f - length);

	return Vector(a, b, c).Normalized();
}

// Mouse Down - Save starting location of imminent drag
void Arcball::Click(int x, int y)
{
	startVec = MapToSphere(x, y);
}

// Mouse Moved - Compute rotation between start and end points of mouse drag
Quaternion Arcball::Drag(int x, int y)
{
	endVec = MapToSphere(x, y);

	Quaternion q(startVec, endVec);

	// Set current mouse coordinates to start of next drag
	//startVec = endVec;
	glutWarpPointer(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);

	return q;
}

void Arcball::Resize(float w, float h)
{
	adjustWidth = 1.0f / (w * 0.5f);
	adjustHeight = 1.0f / (h * 0.5f);
}