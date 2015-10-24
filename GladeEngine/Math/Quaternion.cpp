#include "..\stdafx.h"
#include "Quaternion.h"

using namespace Glade;

// Default Constructor
Quaternion::Quaternion() : x(0.0), y(0.0), z(0.0), w(1.0) { }

// Destructor
Quaternion::~Quaternion(){}

// Constructor from List of components
Quaternion::Quaternion(gFloat components[])
{
	x = components[0];
	y = components[1];
	z = components[2];
	w = components[3];
}

// Constructor from each component
Quaternion::Quaternion(gFloat _x, gFloat _y, gFloat _z, gFloat _w): x(_x), y(_y), z(_z), w(_w) {}

// Constructor from Euler Angles
Quaternion::Quaternion(gFloat phi, gFloat theta, gFloat psi)
{	
	gFloat cosPhi = Cos(phi/2.0),		sinPhi = Sin(phi/2.0);
	gFloat cosTheta = Cos(theta/2.0),	sinTheta = Sin(theta/2.0);
	gFloat cosPsi = Cos(psi/2.0),		sinPsi = Sin(psi/2.0);

	x = sinPhi*cosTheta*sinPsi + cosPhi*sinTheta*cosPsi;
	y = sinPhi*cosTheta*cosPsi - cosPhi*sinTheta*sinPsi;
	z = cosPhi*cosTheta*sinPsi - sinPhi*sinTheta*cosPsi;
	w = cosPhi*cosTheta*cosPsi + sinPhi*sinTheta*sinPsi;
}

// Constructor from Rotation about an Axis
Quaternion::Quaternion(const Vector& startVec, const Vector& endVec)
{
	Vector perp = startVec * endVec;
	perp.NormalizeInPlace();

	if(gFloat(perp) > 1.0e-5)	// if it's non-zero
	{
		gFloat theta = startVec / endVec;	// angle between startVec and endVec
		w = Cos(theta/2);
		// Potentially need to negate some of these based on which direction is positive for each axis
		x = perp.x * Sin(theta/2);
		y = perp.y * Sin(theta/2);
		z = perp.z * Sin(theta/2);
	}
	else
	{
		w = 1.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
}

// Quaternion from Axis Angle
Quaternion::Quaternion(const Vector& v, gFloat angle)
{
	Vector v2 = v.Normalized();
	w = Cos(angle/2);
	x = v2.x * Sin(angle/2);
	y = v2.y * Sin(angle/2);
	z = v2.z * Sin(angle/2);
}

// Copy-Constructor
Quaternion::Quaternion(const Quaternion& quat)
{
	*this = quat;
}

// Operator= overload
Quaternion& Quaternion::operator=(const Quaternion& other)
{
	if(this != &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
	}

	return *this;
}

#pragma region Operator Overloads
// Indexing
gFloat& Quaternion::operator[] (int index)
{
	if(index == 0)	return x;
	if(index == 1)	return y;
	if(index == 2)	return z;
	return w;
}

const gFloat& Quaternion::operator[] (int index) const
{
	if(index == 0)	return x;
	if(index == 1)	return y;
	if(index == 2)	return z;
	return w;
}

// Quaternion Addition
Quaternion Quaternion::operator+ (const Quaternion& other) const
{
	gFloat a = x + other.x;
	gFloat b = y + other.y;
	gFloat c = z + other.z;
	gFloat d = w + other.w;

	return Quaternion(a, b, c, d);
}
Quaternion& Quaternion::operator+= (const Quaternion& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;

	return *this;
}

// Quaternion Subtraction
Quaternion Quaternion::operator- (const Quaternion& other) const
{
	gFloat a = x - other.x;
	gFloat b = y - other.y;
	gFloat c = z - other.z;
	gFloat d = w - other.w;

	return Quaternion(a, b, c, d);
}
Quaternion& Quaternion::operator-= (const Quaternion& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

// Quaternion Multiplication
Quaternion Quaternion::operator* (const Quaternion& other) const
{
	gFloat a = w*other.x + x*other.w + y*other.z - z*other.y;
	gFloat b = w*other.y - x*other.z + y*other.w + z*other.x;
	gFloat c = w*other.z + x*other.y - y*other.x + z*other.w;
	gFloat d = w*other.w - x*other.x - y*other.y - z*other.z;

	return Quaternion(a, b, c ,d);
}
Quaternion& Quaternion::operator*= (const Quaternion& other)
{	// a b c d			e f g h
	gFloat a = w*other.x + x*other.w + y*other.z - z*other.y;
	gFloat b = w*other.y - x*other.z + y*other.w + z*other.x;
	gFloat c = w*other.z + x*other.y - y*other.x + z*other.w;
	gFloat d = w*other.w - x*other.x - y*other.y - z*other.z;

	x = a;
	y = b;
	z = c;
	w = d;

	return *this;
}

// Quaternion Scalar Multiplication
Quaternion Quaternion::operator* (gFloat scalar) const
{
	gFloat a = x * scalar;
	gFloat b = y * scalar;
	gFloat c = z * scalar;
	gFloat d = w * scalar;

	return Quaternion(a, b, c, d);
}
Quaternion& Quaternion::operator*= (gFloat scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;

	return *this;
}

// Quaternion Conjugate
Quaternion& Quaternion::operator~()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

// Quaternion Negation
Quaternion& Quaternion::operator-()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;

	return *this;
}

// Quaternion Dot Product
gFloat Quaternion::operator% (const Quaternion& other) const
{
	return x*other.x + y*other.y + z*other.z;
}

// Quaternion Equality
bool Quaternion::operator== (const Quaternion& other) const
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Quaternion::operator!= (const Quaternion& other) const
{
	return x != other.x || y != other.y || z != other.z || w != other.w;
}

// Quaternion Magnitude
Quaternion::operator gFloat() const
{
	return Sqrt(x*x + y*y + z*z + w*w);
}

#pragma endregion Operator Overloads

#pragma region Functions
// Return this Quaternion + other
Quaternion& Quaternion::Add(const Quaternion& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;

	return *this;
}

// Return this Quaternion - other
Quaternion& Quaternion::Subtract(const Quaternion& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

// Return this Quaternion * other		(Quaternion * Quaternion)
Quaternion& Quaternion::Multiply(const Quaternion& other)
{
	gFloat a = x*other.w + y*other.z - z*other.y + w*other.x;
	gFloat b = -x*other.z + y*other.w + z*other.z + w*other.y;
	gFloat c = x*other.y - y*other.x + z*other.w + w*other.z;
	gFloat d = -x*other.x - y*other.y - z*other.z + w*other.w;

	x = a;
	y = b;
	z = c;
	w = d;

	return *this;
}

// Return this Quaternion * scalar		(Quaternion * scalar)
Quaternion& Quaternion::Scale(gFloat scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;

	return *this;
}

// Returns the Conjugate of this Quaternion, but does not alter it
Quaternion Quaternion::Conjugated() const
{
	return Quaternion(-x, -y, -z, w);
}

// Conjugate this quaternion
Quaternion& Quaternion::ConjugateInPlace()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

Quaternion Quaternion::Inverse() const
{
	Quaternion q(x, y, z, w);

	return q.Conjugated() * (1/Pow(gFloat(q),2));
}

Quaternion& Quaternion::InvertInPlace()
{
	(*this).ConjugateInPlace();
	(*this) *= (1 / (gFloat(*this) * gFloat(*this)));

	return (*this);
}

// Returns the Negated version of this Quaternion, but does not alter it
Quaternion Quaternion::Negated() const
{
	gFloat a = -x;
	gFloat b = -y;
	gFloat c = -z;

	return Quaternion(a, b, c, w);
}

// Negates this Quaternion
Quaternion& Quaternion::NegateInPlace()
{
	return -(*this);
}

// Returns to Normalized version of this Quaternion, but does not alter it.
Quaternion Quaternion::Normalized() const
{
	gFloat mag = gFloat(*this);

	gFloat a = x / mag;
	gFloat b = y / mag;
	gFloat c = z / mag;
	gFloat d = w / mag;

	if(mag == 0)
		return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	return Quaternion(a, b, c, d);
}

// Normalizes this Quaternion
Quaternion& Quaternion::NormalizeInPlace()
{
	gFloat mag = gFloat(*this);
	x /= mag;
	y /= mag;
	z /= mag;
	w /= mag;

	// Exception for quaternions with no magnitude
	if(mag == 0)
	{
		x = y = z = 0.0f;
		w = 1.0f;
	}

	return *this;
}

// Quaternion Dot Product
gFloat Quaternion::DotProduct(const Quaternion& other) const
{
	return x*other.x + y*other.y + z*other.z + w*other.w;
}

// Return the Magnitude of this Quaternion
gFloat Quaternion::Magnitude() const
{
	return gFloat(*this);
}

// Return the corresponding Rotation Matrix of this Quaternion
Matrix Quaternion::ConvertToMatrix() const
{
	gFloat matrix[16] = {
		1-2*y*y - 2*z*z,	2*x*y - 2*z*w,		2*x*z + 2*y*w,		0.0f,
		2*x*y + 2*z*w,		1-2*x*x - 2*z*z,	2*y*z - 2*x*w,		0.0f,
		2*x*z - 2*y*w,		2*y*z + 2*x*w,		1-2*x*x - 2*y*y,	0.0f,
		0.0f,				0.0f,				0.0f,				1.0f
	};
	return Matrix(matrix);
}
	
// Return an array of the Euler Angles represented by this Quaternion
// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
void Quaternion::EulerAngles(gFloat& phi, gFloat& theta, gFloat& psi) const
{
	gFloat sqw = w*w;
    gFloat sqx = x*x;
    gFloat sqy = y*y;
    gFloat sqz = z*z;
	gFloat unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	gFloat test = x*y + z*w;

	if (test > 0.499*unit) // singularity at north pole
	{
		theta = 2 * ATan2(x,w);
		phi = PI/2;
		psi = 0;
		return;
	}

	if (test < -0.499*unit) // singularity at south pole
	{
		theta = -2 * ATan2(x,w);
		phi = -PI/2;
		psi = 0;
		return;
	}

    theta = ATan2(2*y*w-2*x*z , sqx - sqy - sqz + sqw);
	phi = ASin(2*test/unit);
	psi = ATan2(2*x*w-2*y*z , -sqx + sqy - sqz + sqw);
}

// Convert this Quaternion to corresponding Axis Angle representation (returned via passed-by-reference variables)
void Quaternion::AxisAngle(Vector& v, gFloat& angle)
{
	if(w > 1)
		NormalizeInPlace();

	angle = 2 * ACos(w);
	gFloat s = Sqrt(1-(w*w));

	if(s < 0.001f)
	{
		v.x = x;
		v.y = y;
		v.z = z;
		return;
	}

	v.x = x / s;
	v.y = y / s;
	v.z = z / s;
}

Quaternion& Quaternion::Identity()
{
	 w = 1.0f;
	 x = y = z = 0.0f;
	 return(*this);
}

bool Quaternion::IsIdentity()
{
	return x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f;
}

// Return q1 + q2
Quaternion Quaternion::QuaternionAddition(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
}

// Return q1 - q2
Quaternion Quaternion::QuaternionSubtraction(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
}

// Return q1 * q2
Quaternion Quaternion::QuaternionMultiplication(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(q1.x*q2.w + q1.y*q2.z - q1.z*q2.y + q1.w*q2.x,
						-q1.x*q2.z + q1.y*q2.w + q1.z*q2.z + q1.w*q2.y, 
						q1.x*q2.y - q1.y*q2.x + q1.z*q2.w + q1.w*q2.z,
						-q1.x*q2.x - q1.y*q2.y - q1.z*q2.z + q1.w*q2.w);
}

// Return q * scalar
Quaternion Quaternion::QuaternionScale(const Quaternion& q, gFloat scalar)
{
	return Quaternion(q.x * scalar, q.y * scalar, q.z * scalar, q.w * scalar);
}

// Return negation of q
Quaternion Quaternion::QuaternionNegate(const Quaternion& q)
{
	return Quaternion(-q.x, -q.y, -q.z, -q.w);
}

// Return conjugate of q
Quaternion Quaternion::QuaternionConjugate(const Quaternion& q)
{
	return Quaternion(-q.x, -q.y, -q.z, q.w);
}

// Return Matrix equivalent of q
Matrix Quaternion::QuaternionToMatrix(const Quaternion& q)
{
	gFloat matrix[4][4] = {0};

	matrix[0][0] = 1-2*q.y*q.y - 2*q.z*q.z;	matrix[0][1] = 2*q.x*q.y - 2*q.z*q.w;	matrix[0][2] = 2*q.x*q.z + 2*q.y*q.w;
	matrix[1][0] = 2*q.x*q.y + 2*q.z*q.w;	matrix[1][1] = 1-2*q.x*q.x - 2*q.z*q.z;	matrix[1][2] = 2*q.y*q.z - 2*q.x*q.w;
	matrix[2][0] = 2*q.x*q.z - 2*q.y*q.w;	matrix[2][1] = 2*q.y*q.z + 2*q.x*q.w;	matrix[2][2] = 1-2*q.x*q.x - 2*q.y*q.y;

	return Matrix(matrix);
}

// Return Euler Angles of q
gFloat* Quaternion::QuaternionEulerAngles(Quaternion& q)
{
	Matrix matrix = q.ConvertToMatrix();
	gFloat* eulerAngles = new gFloat[3];

	gFloat phi=0, theta=0, psi=0;
	gFloat xyDist = Sqrt(matrix[0][0]*matrix[0][0] + matrix[1][0]*matrix[1][0]);

	// Limit at theta = +/- 90. Stop that here.
	if(xyDist > 0.0001f)
	{
		// atan2f( sin(theta), abs(cos(theta))) = atan2f(tan(theta)) = theta
		theta = ATan2(-matrix[2][0], xyDist);

		// atan2f( cos(theta)sin(psi), cos(theta)cos(psi)) = atan2f(sin(psi), cos(psi)) = atan2f(tan(psi)) = psi
		psi = ATan2(matrix[1][0], matrix[0][0]);

		// atan2f( sin(phi)cos(theta), cos(phi)cos(theta)) = atan2f( sin(phi), cos(phi)) = atan2f(tan(phi)) = phi
		phi = ATan2(matrix[2][1], matrix[2][2]);
	}
	else
	{
		// atan2f( sin(theta), abs(cos(theta))) = atan2f( tan(theta)) = theta
		theta = ATan2(-matrix[2][0], xyDist);

		// atan2f( -(cos(phi)sin(psi)+sin(phi)sin(theta)cos(psi)), cos(phi)cos(psi)+sin(phi)sin(theta)sin(psi))
		//		in this clause, we assume thetha = 90deg or PI/4rad, at which sin(theta) = 0 - This results in
		// atan2f( cos(phi)sin(psi)-0, cos(phi)cos(psi)+0)) = atan2f(cos(phi)sin(psi), cos(phi)cos(psi)) = atan2f(sin(psi), cos(psi)) = atan2f(tan(psi)) = psi
		psi = ATan2(-matrix[0][1], matrix[1][1]);

		// Degree of freedom has been lost due to Gimbal Lock
		phi = 0;
	}

	eulerAngles[0] = phi;
	eulerAngles[1] = theta;
	eulerAngles[2] = psi;

	return eulerAngles;
}

// Return Quaternion representation of a rotation between 2 Vectors
Quaternion Quaternion::QuaternionFromTwoVectors(const Vector& startVec, const Vector& endVec)
{
	Vector perp = startVec * endVec;
	perp.NormalizeInPlace();

	if(gFloat(perp) > EPSILON)
	{
		gFloat angle = ACos((startVec % endVec) / (gFloat(startVec) * gFloat(endVec)));
		return Quaternion(perp.x * Sin(angle/2), perp.y * Sin(angle/2), perp.z * Sin(angle/2), Cos(angle/2));
	}
	else	// Start and End Vectors coincide, return identity quaternion
		return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, gFloat t)
{
	Quaternion newQ;
	gFloat dot = q1.DotProduct(q2);
	gFloat temp = 1.0 - t;

	// q1 == q2 or q1 == -q2 -> angle between = 0
	if(dot >= 1.0)
		return q1;

	// Interpolate the opposite way?
	if(dot < 0.0)
	{
		t = -t;
		dot = -dot;
	}

	// Only spherically interpolate if quats aren't extremely close
	// If they are too close, then they will linearly interpolate
	if(dot < EPSILON)
	{
		gFloat theta = ACos(dot);
		temp = Sin(theta * temp) / Sin(theta);
		t = Sin(theta * t) / Sin(theta);
	}

	// Actually do interpolation
	newQ.w = q1.w*temp + q2.x*t;
	newQ.x = q1.x*temp + q2.x*t;
	newQ.y = q1.y*temp + q2.y*t;
	newQ.z = q1.z*temp + q2.z*t;
	return newQ;
}