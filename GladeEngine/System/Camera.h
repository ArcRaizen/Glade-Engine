#pragma once
#ifndef GLADE_CAMERA_H
#define GLADE_CAMERA_H
#include "../RigidBody.h"
#include "../Math/Matrix.h"
#ifdef FRUSTUM_CULLING_GEOMETRIC
#include "../Math/Plane.h"
#endif

namespace Glade {
class Camera
{
public:
	Camera();
	~Camera();

	enum FrustumTest { OUTSIDE=0, INSIDE, INTERSECT };

	void Update(gFloat dt);
private:
	void UpdateView();

public:
	// Setup View and Projection Matrices
	void LookAt(const Vector& eye, const Vector& target, const Vector& up);
	void FPSView(const Vector& eye, gFloat pitch, gFloat yaw, gFloat roll=gFloat(0.0f));
	void SetPerspectiveFoVY(gFloat fovy, gFloat aspect, gFloat near, gFloat far);	// Set Perspective projection given vertical Field-of-View
	void SetPerspectiveFoVX(gFloat fovx, gFloat aspect, gFloat near, gFloat far);	// Set perspective projection given horizonal Field-of-View
	void SetOrtho(gFloat width, gFloat height, gFloat near, gFloat far);

	// Frustum Culling functions
#ifdef FRUSTUM_CULLING_SPHERES
	int IsSphereInFrustum(Object* o);
#else
	int IsBoxInFrustum(Object* o);
	int IsBoxInFrustum(AABB& aabb);
#endif
#ifdef FRUSTUM_CULLING_GEOMETRIC
	void CalcPlanes();
#endif

	// Move/Modify the camera
	void Strafe(gFloat d);			// move left/right
	void StrafeWorld(gFloat d);		// move left/right on world X-Axis
	void Walk(gFloat d);			// move forward/backward
	void WalkWorld(gFloat d);		// move foward/backward on world Z-Axis
	void Climb(gFloat d);			// move up/down
	void ClimbWorld(gFloat d);		// move up/down along world Y-Axis
	void Move(const Vector& dir, gFloat d);	// Move camera in specified direction
	void MoveWorld(const Vector& dir, gFloat d);


	void Pitch(gFloat angle);		// Rotate up/down along right vector
	void PitchWorld(gFloat angle);	// Rotate up/down along world right vector
	void Turn(gFloat angle);		// Rotate left/right along up vector
	void TurnWorld(gFloat angle);	// Rotate left/right along world up vector
	void Roll(gFloat angle);		// Rotate around forward vector
	void RollWorld(gFloat angle);	// Rotate around world forward vector
	void Rotate(Quaternion quat, bool local=true);

	void SetPosition(const Vector& pos);
	void SetOrientation(const Quaternion& rot);
	void SetEulerAngles(const Vector& euler);

	// Set Camera to watch a specific Object/position
	void SetWatchTarget(RigidBody* b, Vector off, const Vector& worldUp=Vector(0,1,0));
	void ClearWatchTargt();

	// Set Camera to follow a specific target
	void SetFollowTarget(RigidBody* b, Vector off, gFloat d, unsigned int flag, gFloat h=-1.0f);
	void ClearFollowTarget();

	void SetHeightDampening(gFloat damp);
	void SetRotationDampening(gFloat damp);

	// ~~~~ Accessor Functions ~~~~
	Matrix GetView();
	Matrix GetProj() const;
	Matrix GetViewProj();
	Vector GetPosition() const;
	Quaternion GetOrientation() const;
	Vector GetRight() const;
	Vector GetUp() const;
	Vector GetForward() const;
	gFloat GetFoVY() const;
	gFloat GetFoVX() const;
	gFloat GetAspect() const;
	gFloat GetZNear() const;
	gFloat GetZFar() const;
	gFloat GetNearWidth() const;
	gFloat GetNearHeight() const;
	gFloat GetFarWidth() const;
	gFloat GetFarHeight() const;
private:
	Matrix view;
	Matrix projection;

	// Camera Targets
	RigidBody* watchTarget;	// If set, Camera will constantly update to
							// LookAt this target position
	Vector		watchOffset;
	Vector		watchUp;
	RigidBody*	followTarget;	// If set, Camera will remain stay within specified
								// distance on X-Z plane of this target position
								// and maintain same height from target
	gFloat	followDistance;
	unsigned int followFlag;
	gFloat  followHeight;
	gFloat heightDamp, rotateDamp;

	// Properties
	Vector position;
	Quaternion orientation;
	gFloat aspect;					// Aspect Ratio (width/height)
	gFloat fovy, fovx;				// Vertical/Horizontal Field of View
										// Aspect = Tan(fovx/2) / Tan(fovy/2)
	gFloat zNear, zFar;
	gFloat nearWidth, nearHeight;	// width/height of near plane
	gFloat farWidth, farHeight;		// width/height of far plane
	bool viewDirty;		// Does the View matrix need updating after
							// change to camera's position or orientation
							// Follow/Watch behavior overrides this
#ifdef FRUSTUM_CULLING_RADAR
	gFloat secFoVY, secFoVX;		// Secant (1 / cos) of the field-of-view angles
									// Used during frustum culling, they don't change once the project matrix is set
									// so we can precompute and save them early
#endif

	// Frustum Culling properties
#ifdef FRUSTUM_CULLING_GEOMETRIC
	Plane planes[6];
#endif
};
}	// namespace
#endif	// GLADE_CAMERA_H

