#include "Camera.h"

using namespace Glade;

Camera::Camera() : watchTarget(nullptr), heightDamp(2.0f), rotateDamp(3.0f), viewDirty(false)
{
	LookAt(Vector(0,0,0), Vector(0,0,1), Vector(0,1,0));
	SetPerspectiveFoVY(PI * 0.25f, 16.f/9.f, 1.0f, 1000.0f);
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

#ifdef FRUSTUM_CULLING_GEOMETRIC
		CalcPlanes();
#endif	
	}
}

void Camera::LookAt(const Vector& eye, const Vector& target, const Vector& up)
{
	// Calc new view matrix
	view = Matrix::LookAt(eye, target, up);

	// Update new properties
	position = eye;
	orientation = view.Inverse3().ConvertToQuaternion();

#ifdef FRUSTUM_CULLING_GEOMETRIC
	CalcPlanes();
#endif
}

void Camera::FPSView(const Vector& eye, gFloat pitch, gFloat yaw, gFloat roll/*=gFloat(0.0f)*/)
{
	// Calc new view matrix
	view = Matrix::FPSView(eye, pitch, yaw, roll);

	// Update new properites
	position = eye;
	orientation = view.Inverse3().ConvertToQuaternion();
}

void Camera::SetPerspectiveFoVY(gFloat fovy, gFloat aspect, gFloat zNear, gFloat zFar)
{
	// Save properties
	gFloat fov = fovy * gFloat(0.5f);
	this->fovy = fovy;
	this->fovx = (2 * ATan(aspect * Tan(fov)));
	this->aspect = aspect;
	this->zNear = zNear;
	this->zFar = zFar;
	nearHeight = gFloat(2.0f) * zNear * Tan(fov);
	farHeight = gFloat(2.0f) * zFar * Tan(fov);
	nearWidth = nearHeight * aspect;
	farWidth = farHeight * aspect;
#ifdef FRUSTUM_CULLING_RADAR
	secFoVY = gFloat(1.0f) / Cos(fov);
	secFoVX = gFloat(1.0f) / Cos(fovx * gFloat(0.5f));
#endif

	projection = Matrix::PerspectiveFoVY(fovy, aspect, zNear, zFar);

#ifdef FRUSTUM_CULLING_GEOMETRIC
	CalcPlanes();
#endif
}

void Camera::SetPerspectiveFoVX(gFloat fovx, gFloat aspect, gFloat zNear, gFloat zFar)
{
	this->fovx = fovx;
	this->fovy = (2 * ATan(Tan(fovx*gFloat(0.5f)) / aspect));
	gFloat fov = fovy * gFloat(0.5f);
	this->aspect = aspect;
	this->zNear = zNear;
	this->zFar = zFar;
	nearHeight = gFloat(2.0f) * zNear * Tan(fov);
	farHeight = gFloat(2.0f) * zFar * Tan(fov);
	nearWidth = nearHeight * aspect;
	farWidth = farHeight * aspect;
#ifdef FRUSTUM_CULLING_RADAR
	secFoVY = gFloat(1.0f) / Cos(fov);
	secFoVX = gFloat(1.0f) / Cos(fovx * gFloat(0.5f));
#endif

	projection = Matrix::PerspectiveFoVY(fovy, aspect, zNear, zFar);

#ifdef FRUSTUM_CULLING_GEOMETRIC
	CalcPlanes();
#endif
}

void Camera::SetOrtho(gFloat width, gFloat height, gFloat zNear, gFloat zFar)
{
	// Save properties
	fovy = fovx = gFloat(-1.0f);
	aspect = width / height;
	this->zNear = zNear;
	this->zFar = zFar;
	nearWidth = farWidth = width;
	nearHeight = farHeight = height;

	projection = Matrix::Ortho(width, height, zNear, zFar);
}

// Test if an Object's Bounding Sphere intersects with the Frustum of the Camera
// http://zach.in.tu-clausthal.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/
#ifdef FRUSTUM_CULLING_SPHERES
int Camera::IsSphereInFrustum(Object* o)
{
	Vector p = o->GetPosition();
	gFloat r = o->GetRadius();
#ifdef FRUSTUM_CULLING_GEOMETRIC
	int result = FrustumTest::INSIDE;	// default result to inside, test for outside
	gFloat dist;
	int plane = o->GetFrustumPlane();

	// Each Object that was culled in the previous frame will save the plane that culled it
	// Test that plane 1st because it is most likely the Object will be culled by that plane again
	//		providing an early out to the test
	// "Plane Coherancy Optimization"
	if(plane > 0)
	{
		// Test Object against each plane
		for(unsigned int i, i_ = 0; i_ < 6; ++i_)
		{
			i = (i_+plane) % 6;
			dist = planes[i].Distance(p);
			if(dist < -r) // Early out, it is outside a plane
			{ 
				o->SetFrustumPlane(i);
				return OUTSIDE;
			}
			if(dist < r) result = INTERSECT;
		}
		return result;
	}
	else	// No Frustum Plane set, or is set to 0
	{		// Either way, no need for extra mod instruction each iteration
		// Test Object against each plane
		for(unsigned int i = 0; i < 6; ++i)
		{
			dist = planes[i].Distance(p);
			if(dist < -r) // Early out, it is outside a plane
			{ 
				o->SetFrustumPlane(i);
				return OUTSIDE;
			}
			if(dist < r) result = INTERSECT;
		}
		return result;
	}
#else	// FRUSTUM_CULLING_RADAR
	// Calc position of Object relative to Camera
	Vector v = position - p;
	gFloat pRelZ = v.DotProduct(GetForward());

	// Check if sphere totally outside frustum
	// Test 'z' - If totally outside, early out
	if(pRelZ > zFar + r || pRelZ < zNear - r)
		return OUTSIDE;

	gFloat pRelY = v.DotProduct(GetUp());			// 'y' distance of object from Camera
	gFloat h = pRelZ * 2 * Tan(fovx * gFloat(0.5f));// height of frustum at current z-depth of Object
	gFloat dy = r * secFoVY;						// distance from sphere to frustum along 'x' axis
	if(pRelY < -h/2 - dy || pRelY > h/2 + dy)		// Test 'y' - If totally outside, early out
		return OUTSIDE;

	gFloat pRelX = v.DotProduct(GetRight());	// 'x' distance of object from camera
	gFloat w = h * aspect;						// width of frustum at current z-depth of Object
	gFloat dx = r * secFoVX;					// distance from sphere to frustum along 'y' axis
	if(pRelX < -w/2 - dx || pRelX > w/2 + dx)	// Test 'x' - If totally outside, early out
		return OUTSIDE;

	// No early out, either intersect or inside
	if(pRelZ > zFar - r || pRelZ < zNear + r)	// If not totally inside, then must be intersecting
		return INTERSECT;
	if(pRelY < -h/2 + dy || pRelY > h/2 - dy)	// ""
		return  INTERSECT;
	if(pRelX < -w/2 + dx || pRelX > w/2 - dx)	// Ditto
		return  INTERSECT;

	// Done testing, nothing found. Must be totally inside
	return INSIDE;
#endif
}
#endif

// Test if an Object's AABB intersects with the Frustum of the Camera
// http://zach.in.tu-clausthal.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/
#ifdef FRUSTUM_CULLING_BOXES
int Camera::IsBoxInFrustum(Object* o)
{
	return IsBoxInFrustum(o->GetBoundingBox());
}

int Camera::IsBoxInFrustum(AABB& aabb)
{
	int result = FrustumTest::INSIDE;	// default result to inside, test for outside
	int plane = aabb.GetFrustumPlane();

	// "Positive" and "Negative" vertices of bounding box
	// The vertices most and least in the direction of the plane normal
	// If 'p' is outside a plane, the whole box is outside the frustum,
	// If 'p' is inside a plane and 'n' is outside, the box intersects that plane
	Vector p, n;

	// Each Object that was culled previous frame will save the plane that culled it
	// Test that plane 1st because it is most likely the Object will be culled by that plane again
	//		providing an early out to the test
	// "Plane Coherancy Optimization"
	if(plane > 0)
	{
		// Test Object against each plane
		for(unsigned int i, i_ = 0; i_ < 6; ++i_)
		{
			i = (i_+plane) % 6;

			// Calculate 'p'
			p.x = (planes[i].normal >= 0) ? aabb.maximum.x : aabb.minimum.x;
			p.y = (planes[i].normal >= 0) ? aabb.maximum.y : aabb.minimum.y;
			p.z = (planes[i].normal >= 0) ? aabb.maximum.z : aabb.minimum.z;

			// Test 'p'
			if(planes[i].Distance(p) < 0)	// Early out, 'p' is outside a plane
			{
				aabb.SetFrustumPlane(i);
				return OUTSIDE;
			}

			// Calculate 'n'
			n.x = (planes[i].normal >= 0) ? aabb.minimum.x : aabb.maximum.x;
			n.y = (planes[i].normal >= 0) ? aabb.minimum.y : aabb.maximum.y;
			n.z = (planes[i].normal >= 0) ? aabb.minimum.z : aabb.maximum.z;

			// Test 'n'
			if(planes[i].Distance(n) < 0) result = INTERSECT;
		}
		return result;
	}
	else// No Frustum Plane set, or is set to 0
	{		// Either way, no need for extra mod instruction each iteration
		for(unsigned int i = 0; i < 6; ++i)
		{
			// Calculate 'p'
			p.x = (planes[i].normal.x >= 0) ? aabb.maximum.x : aabb.minimum.x;
			p.y = (planes[i].normal.y >= 0) ? aabb.maximum.y : aabb.minimum.y;
			p.z = (planes[i].normal.z >= 0) ? aabb.maximum.z : aabb.minimum.z;

			// Test 'p'
			if(planes[i].Distance(p) < 0)	// Early out, 'p' is outside a plane
			{
				aabb.SetFrustumPlane(i);
				return OUTSIDE;
			}

			// Calculate 'n'
			n.x = (planes[i].normal.x >= 0) ? aabb.minimum.x : aabb.maximum.x;
			n.y = (planes[i].normal.y >= 0) ? aabb.minimum.y : aabb.maximum.y;
			n.z = (planes[i].normal.z >= 0) ? aabb.minimum.z : aabb.maximum.z;

			// Test 'n'
			if(planes[i].Distance(n) < 0) result = INTERSECT;
		}
		return result;
	}
}
#endif

#ifdef FRUSTUM_CULLING_GEOMETRIC
void Camera::CalcPlanes()
{
	Vector d = GetForward(), up = GetUp(), right = GetRight();
	Vector farCenter = position + d * zFar;
	Vector nearCenter = position + d * zNear;

//	~~~~ FOR ORTHOGRAPHIC PROJECTION ~~~~
	if(fovy < 0)
	{
		gFloat half = gFloat(0.5f);

		Vector ntl = nearCenter + (up * (nearHeight*half)) - (right * (nearWidth*half));
		Vector ntr = nearCenter + (up * (nearHeight*half)) + (right * (nearWidth*half));
		Vector nbl = nearCenter - (up * (nearHeight*half)) - (right * (nearWidth*half));
		Vector nbr = nearCenter - (up * (nearHeight*half)) + (right * (nearWidth*half));
	//	Vector ftl = farCenter + (up * (farHeight*half)) - (right * (farWidth*half));
		Vector ftr = farCenter + (up * (farHeight*half)) + (right * (farWidth*half));
		Vector fbl = farCenter - (up * (farHeight*half)) - (right * (farWidth*half));
	//	Vector fbr = farCenter - (up * (farHeight*half)) + (right * (farWidth*half));

		planes[0].SetNormalPoint(d, nearCenter);	// Near Plane
		planes[1].SetNormalPoint(-d, farCenter);	// Far Plane
		planes[2].Set3Points(ftr, ntr, nbr);		// Right Plane
		planes[3].Set3Points(fbl, nbl, ntl);		// Left Plane
		planes[4].Set3Points(ntl, ntr, ftr);		// Top Plane
		planes[5].Set3Points(nbr, nbl, fbl);		// Bottom Plane
	}
	else	// ~~~~ FOR PERSPECTIVE PROJECTION ~~~~
	{
		planes[0].SetNormalPoint(d, nearCenter);	// Near Plane
		planes[1].SetNormalPoint(-d, farCenter);	// Far Plane

		// Calculate point on plane for each of right/left/top/bottom planes
		// Calculate unit vector from camera position to point on plane
		// Calculate normal of that plane via CrossProduct of unit vector and camera Up/Right vectors
		// Create plane with normal and point on plane
		// All crushed down into 2 lines of code per plane

		// Right Plane
		Vector point;
		gFloat nh = nearHeight * gFloat(0.5f), nw = nearWidth * gFloat(0.5f);
		point = nearCenter + right * nw;
#ifdef LEFT_HANDED_COORDS
		planes[2].SetNormalPoint((point - position).Normalized() * up, point);
#else
		planes[2].SetNormalPoint(up * (point - position).Normalized(), point);
#endif
		// Left Plane
		point = nearCenter - right * nw;
#ifdef LEFT_HANDED_COORDS
		planes[3].SetNormalPoint(up * (point - position).Normalized(), point);
#else
		planes[3].SetNormalPoint((point - position).Normalized() * up, point);
#endif

		// Top Plane
		point = nearCenter + up * nh;
#ifdef LEFT_HANDED_COORDS
		planes[4].SetNormalPoint(right * (point - position).Normalized(), point);
#else
		planes[4].SetNormalPoint((point - position).Normalized() * right, point);
#endif

		// Bottom Plane
		point = nearCenter - up * nh;
#ifdef LEFT_HANDED_COORDS
		planes[5].SetNormalPoint((point - position).Normalized() * right, point);
#else
		planes[5].SetNormalPoint(right * (point - position).Normalized(), point);
#endif
	}
}
#endif

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

gFloat Camera::GetFoVY() const { return fovy; }
gFloat Camera::GetFoVX() const { return fovx; }
gFloat Camera::GetAspect() const { return aspect; }
gFloat Camera::GetZNear() const { return zNear; }
gFloat Camera::GetZFar() const { return zFar; }
gFloat Camera::GetNearWidth() const { return nearWidth; }
gFloat Camera::GetNearHeight() const { return nearHeight; }
gFloat Camera::GetFarWidth() const { return farWidth; }
gFloat Camera::GetFarHeight() const { return farHeight; }