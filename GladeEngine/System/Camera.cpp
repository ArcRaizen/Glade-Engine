#include "Camera.h"

using namespace Glade;

Camera::Camera() : watchTarget(nullptr), heightDamp(2.0f), rotateDamp(3.0f), viewDirty(false)
{
	LookAt(Vector(0,0,0), Vector(0,0,1), Vector(0,1,0));
	SetPerspective(PI * 0.25f, 1.0f, 1.0f, 100.0f);
}

Camera::~Camera()
{
}

void Camera::Update(gFloat dt)
{
	// Follow assigned target
	if(followTarget != nullptr)
	{
		// Calc height
		gFloat targetHeight = followTarget->GetPosition().y + followHeight + watchOffset.y;
		gFloat curHeight = position.y;

		// Dampen height
		curHeight = Lerp<gFloat>(curHeight, targetHeight, 2.0f * dt);

		gFloat targetAngle, curAngle, temp;
		if(followFlag != 1 && followFlag != 2)
		{
			followTarget->GetOrientation().EulerAngles(temp, targetAngle, temp);
			view.Inverse3().EulerAngles(temp, curAngle, temp);
			
			// Dampen rotation
			curAngle = LerpWrap<gFloat>(curAngle, targetAngle, rotateDamp*dt, 0, TWO_PI);

			// Convert rotation to Quaternion,  set position on X-Z plane behind target
			position = (followTarget->GetPosition() + watchOffset) - (Vector(0,0,1) * Quaternion(0,curAngle,0) * followDistance);
		}
		else if(followFlag == 1)
		{
			Vector targetPos = followTarget->GetPosition() + watchOffset;
			Vector targetVel = followTarget->GetVelocity().Normalized();
			if(targetVel.SquaredMagnitude() != gFloat(0.0f))
			{
				targetPos.y = 0;
				targetVel.y = 0;
				position.y = 0;

				// Lerp towards 'distance' units behind target's current heading in X-Z plane
				position = Vector::Lerp(position, (targetPos - (targetVel*followDistance)), rotateDamp * dt);
			}
		}
		else
		{
			Vector targetPos = followTarget->GetPosition() + watchOffset;
			targetPos.y = 0;
			position.y = 0;

			// Lerp towards 'distance' units from target in X-Z plane
			position = Vector::Lerp(position, targetPos + (position-targetPos).Normalized() * followDistance, rotateDamp * dt);
		}

		// Set height
		position.y = curHeight;

		// Always look at target
		LookAt(position, followTarget->GetPosition() + watchOffset, Vector(0,1,0));
		viewDirty = false;
	}
	else if(watchTarget != nullptr)
	{
		LookAt(position, watchTarget->GetPosition() + watchOffset, watchUp);
		viewDirty = false;
	}
}

void Camera::UpdateView()
{
	if(viewDirty)
	{
		Matrix trans = Matrix::MatrixFromTranslation(-position);
		Matrix rot = orientation.ConvertToMatrix().Transpose3();
		view = trans * rot;
		viewDirty = false;
	}
}

void Camera::LookAt(const Vector& eye, const Vector& target, const Vector& up)
{
	// Calc new view matrix
	view = Matrix::LookAt(eye, target, up);

	// Update new properties
	position = eye;
	orientation = view.Inverse3().ConvertToQuaternion();
}

void Camera::FPSView(const Vector& eye, gFloat pitch, gFloat yaw, gFloat roll/*=gFloat(0.0f)*/)
{
	// Calc new view matrix
	view = Matrix::FPSView(eye, pitch, yaw, roll);

	// Update new properites
	position = eye;
	orientation = view.Inverse3().ConvertToQuaternion();
}

void Camera::SetPerspective(gFloat fov, gFloat aspect, gFloat zNear, gFloat zFar)
{
	// Save properties
	this->fov = fov;
	this->aspect = aspect;
	this->zNear = zNear;
	this->zFar = zFar;
	nearHeight = gFloat(2.0f) * zNear * Tan(fov * 0.5f);
	farHeight = gFloat(2.0f) * zFar * Tan(fov * 0.5f);
	nearWidth = nearHeight * aspect;
	farWidth = farHeight * aspect;

	projection = Matrix::Perspective(fov, aspect, zNear, zFar);
}

void Camera::SetOrtho(gFloat width, gFloat height, gFloat zNear, gFloat zFar)
{
	// Save properties
	fov = gFloat(-1.0f);
	aspect = gFloat(1.0f);
	this->zNear = zNear;
	this->zFar = zFar;
	nearWidth = farWidth = width;
	nearHeight = farHeight = height;

	projection = Matrix::Ortho(width, height, zNear, zFar);
}

void Camera::Strafe(gFloat d)
{ 
	if(d == gFloat(0.0f)) return;
	if(followTarget != nullptr) return;
	position += Vector(d,0,0) * orientation;
	viewDirty = true;
}
void Camera::StrafeWorld(gFloat d)
{
	if(d == gFloat(0.0f)) return;
	if(followTarget != nullptr) return;
	position += Vector(d,0,0);
	viewDirty = true;
}
void Camera::Walk(gFloat d)
{
	if(d == gFloat(0.0f)) return;
	if(followTarget != nullptr) return;
	position += Vector(0,0,d) * orientation;
	viewDirty = true;
}
void Camera::WalkWorld(gFloat d)
{
	if(d == gFloat(0.0f)) return;
	if(followTarget != nullptr) return;
	position += Vector(0,0,d);
	viewDirty = true;
}
void Camera::Climb(gFloat d) 
{
	if(d == gFloat(0.0f)) return;
	if(followTarget != nullptr) return;
	position += Vector(0,d,0) * orientation;
	viewDirty = true;
}
void Camera::ClimbWorld(gFloat d)
{
	if(d == gFloat(0.0f)) return;
	if(followTarget != nullptr) return;
	position += Vector(0,d,0);
	viewDirty = true;
}
void Camera::Move(const Vector& dir, gFloat d)
{
	if(d == gFloat(0.0f)) return;
	if(followTarget != nullptr) return;
	position += (dir.Normalized()*d) * orientation;
	viewDirty = true;
}
void Camera::MoveWorld(const Vector& dir, gFloat d)
{
	if(d == gFloat(0.0f)) return;
	if(followTarget != nullptr) return;
	position += dir.Normalized() * d;
	viewDirty = true;
}


void Camera::Pitch(gFloat angle)
{
	if(angle == gFloat(0.0f)) return;
	if(watchTarget != nullptr || followTarget != nullptr) return;
	orientation *= Quaternion(GetRight(), angle);
	viewDirty = true;
}
void Camera::PitchWorld(gFloat angle)
{
	if(angle == gFloat(0.0f)) return;
	if(watchTarget != nullptr || followTarget != nullptr) return;
	orientation *= Quaternion(Vector(1,0,0), angle);
	viewDirty = true;
}
void Camera::Turn(gFloat angle) 
{ 
	if(angle == gFloat(0.0f)) return;
	if(watchTarget != nullptr || followTarget != nullptr) return;
	orientation *= Quaternion(GetUp(), angle);
	viewDirty = true;
}
void Camera::TurnWorld(gFloat angle) 
{ 
	if(angle == gFloat(0.0f)) return;
	if(watchTarget != nullptr || followTarget != nullptr) return;
	orientation *= Quaternion(Vector(0,1,0), angle);
	viewDirty = true;
}
void Camera::Roll(gFloat angle)
{
	if(angle == gFloat(0.0f)) return;
	if(watchTarget != nullptr || followTarget != nullptr) return;
	orientation *= Quaternion(GetForward(), angle);
	viewDirty = true;
}
void Camera::RollWorld(gFloat angle)
{
	if(angle == gFloat(0.0f)) return;
	if(watchTarget != nullptr || followTarget != nullptr) return;
	orientation *= Quaternion(Vector(0,0,1), angle);
	viewDirty = true;
}
void Camera::Rotate(Quaternion quat, bool local/*=true*/)
{
//	if(watchTarget != nullptr || followTarget != nullptr) return;

	orientation *= quat;
	viewDirty = true;
}

void Camera::SetPosition(const Vector& pos)
{
	position = pos;
	viewDirty = true;
}
void Camera::SetOrientation(const Quaternion& rot)
{
	orientation = rot;
	viewDirty = true;
}
void Camera::SetEulerAngles(const Vector& euler)
{
	orientation = Quaternion(euler.x, euler.y, euler.z);
	viewDirty = true;
}


void Camera::SetWatchTarget(RigidBody* b, Vector off, const Vector& u/*=Vector(0,1,0)*/) { watchTarget = b; watchOffset = off; watchUp = u; }
void Camera::ClearWatchTargt() { watchTarget = nullptr; }

// Flag = 0 -> Follow behind based on target orientation
// Flag = 1 -> Follow behind based on current velocity direction
// Flag = 2 -> Follow within distance regardless of direction
// Flag = ? -> Defaults to 0
void Camera::SetFollowTarget(RigidBody* b, Vector off, gFloat d, unsigned int flag, gFloat h/*=-1.0f*/) 
{ 
	followTarget = b; 
	watchOffset = off;
	followDistance = d; 
	followFlag = flag;
	followHeight = (h == gFloat(-1.0f)) ? position.y - b->GetPosition().y : h; 

	// Clear Watch Target - Following assumes watching already
	watchTarget = nullptr;
}
void Camera::ClearFollowTarget() { followTarget = nullptr; }

void Camera::SetHeightDampening(gFloat damp) { heightDamp = damp; }
void Camera::SetRotationDampening(gFloat damp) { rotateDamp = damp; }

Matrix Camera::GetView() 
{ 
	if(followTarget == nullptr && watchTarget == nullptr)
		UpdateView();
	return view; 
}
Matrix Camera::GetProj() const { return projection; }
Matrix Camera::GetViewProj() 
{ 
	if(followTarget == nullptr && watchTarget == nullptr)
		UpdateView();
	return view * projection; 
}
Vector Camera::GetPosition() const { return position; }
Quaternion Camera::GetOrientation() const { return orientation; }
Vector Camera::GetRight() const
{
#ifdef LEFT_HANDED_COORDS
	return Vector(view(0,0), view(1,0), view(2,0));
#else
	return Vector(-view(0,0, -view(1,0), -view(2,0));
#endif
}
Vector Camera::GetUp() const { return Vector(view(0,1), view(1,1), view(2,1)); }
Vector Camera::GetForward() const
{
#ifdef LEFT_HANDED_COORDS
	return Vector(view(0,2), view(1,2), view(2,2));
#else
	return Vector(-view(0,2), -view(1,2), -view(2,2));
#endif
}

gFloat Camera::GetFoV() const { return fov; }
gFloat Camera::GetAspect() const { return aspect; }
gFloat Camera::GetZNear() const { return zNear; }
gFloat Camera::GetZFar() const { return zFar; }
gFloat Camera::GetNearWidth() const { return nearWidth; }
gFloat Camera::GetNearHeight() const { return nearHeight; }
gFloat Camera::GetFarWidth() const { return farWidth; }
gFloat Camera::GetFarHeight() const { return farHeight; }