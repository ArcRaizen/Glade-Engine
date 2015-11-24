#include "..\stdafx.h"
#include "Vector.h"

using namespace Glade;

const Vector Vector::GRAVITY = Vector(0.0f, (gFloat)-500.2f, 0.0f);
const Vector Vector::UP = Vector(0.0f, 1.0f, 0.0f);
const Vector Vector::DOWN = Vector(0.0f, -1.0f, 0.0f);
const Vector Vector::RIGHT = Vector(1.0f, 0.0f, 0.0f);
const Vector Vector::X_AXIS = Vector(1.0f, 0.0f, 0.0f);
const Vector Vector::Y_AXIS = Vector(0.0f, 1.0f, 0.0f);
const Vector Vector::Z_AXIS = Vector(0.0f, 0.0f, 1.0f);


// Default constructor
Vector::Vector() : x(0.0f), y(0.0f), z(0.0f) { }

// Constructor from list of coordinates
Vector::Vector(const gFloat components[]) : x(components[0]), y(components[1]), z(components[2]) { }

// Constructor from each component
Vector::Vector(const gFloat _x, const gFloat _y, const gFloat _z): x(_x), y(_y), z(_z) { }

Vector::Vector(const Vector& start, const Vector& end)
{
	x = end.x - start.x;
	y = end.y - start.y;
	z = end.z - start.z;
}

// Copy-Constructor
Vector::Vector(const Vector& other)
{
	//cout << "copy-constructor called" << endl;
	*this = other;
}

//Operator= overload
Vector& Vector::operator= (const Vector& other)
{
	//cout << "operator= called" << endl;
	if(this != &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	return *this;
}

// Destructor
Vector::~Vector() {}

#pragma region Operator Overloads
gFloat& Vector::operator[] (unsigned int index)
{
	if(index == 0) return x;
	if(index == 1) return y;
	return z;
}

const gFloat& Vector::operator[] (unsigned int index) const
{
	if(index == 0) return x;
	if(index == 1) return y;
	return z;
}

Vector Vector::operator- () const
{
	return Vector(-x, -y, -z);
}

// Vector Scalar Multiplication
Vector Vector::operator* (gFloat scalar) const
{
	return Vector(x * scalar, y * scalar, z * scalar);
}
Vector& Vector::operator*= (gFloat scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

// Vector Matrix Multiplication 
Vector Vector::operator* (const Matrix& m) const
{
	Vector v(m(0,0)*x + m(1,0)*y + m(2,0)*z + m(3,0),
			  m(0,1)*x + m(1,1)*y + m(2,1)*z + m(3,1),
			  m(0,2)*x + m(1,2)*y + m(2,2)*z + m(3,2));

/*	gFloat w = m(0,3)*x + m(1,3)*y + m(2,3)*z + m(3,3);
	if(w != (gFloat)1.0 && w != (gFloat)1.0)
	{
		v.x /= w;
		v.y /= w;
		v.z /= w;
	}
*/
	return v;
}
Vector& Vector::operator*= (const Matrix& m)
{
	gFloat a=x, b=y, c=z;
	x = m(0,0)*a + m(1,0)*b + m(2,0)*c + m(3,0);
	y = m(0,1)*a + m(1,1)*b + m(2,1)*c + m(3,1);
	z = m(0,2)*a + m(1,2)*b + m(2,2)*c + m(3,2);

/*	w = m(0,3)*a + m(1,3)*b + m(2,3)*c + m(3,3);
	if(w != (gFloat)1.0 && w != (gFloat)1.0)
	{
		x /= w;
		y /= w;
		z /= w;
	}
*/
	return *this;
}

Vector Vector::operator* (const Quaternion& q) const
{
	Vector v = this->Normalized();
	Quaternion resQuat, vecQuat;
	vecQuat.x = v.x;
	vecQuat.y = v.y;
	vecQuat.z = v.z;
	vecQuat.w = 0.0f;

	resQuat = vecQuat * q.Conjugated();
	resQuat = q * resQuat;

	return Vector(resQuat.x, resQuat.y, resQuat.z);
}

Vector& Vector::operator*= (const Quaternion& q)
{
	Quaternion vectorQuat(x, y, z, 0.0f);		// Quaternion representation of this vector
	Quaternion result = (q.Normalized() * vectorQuat * q.Normalized().Conjugated());

	x = result.x;
	y = result.y;
	z = result.z;

	return *this;
}

// Vector Scalar Division
Vector Vector::operator/ (gFloat scalar) const
{
	return Vector(x / scalar, y / scalar, z / scalar);
}
Vector& Vector::operator/= (gFloat scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}
	 
// Vector Addition
Vector Vector::operator+ (const Vector& other) const
{
	return Vector(x+other.x, y+other.y, z+other.z);
}
Vector& Vector::operator+= (const Vector& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

// Vector Subtraction (this - other)
Vector Vector::operator- (const Vector& other) const
{
	return Vector(x-other.x, y-other.y, z-other.z);
}
Vector& Vector::operator-= (const Vector& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

// Vector Dot Product
gFloat Vector::operator% (const Vector& other) const
{
	return x*other.x + y*other.y + z*other.z;
}

// Vector Cross Product
Vector Vector::operator* (const Vector& other) const
{
	gFloat a = y*other.z - z*other.y;
	gFloat b = z*other.x - x*other.z;
	gFloat c = x*other.y - y*other.x;

	return Vector(a, b, c);
}

// Angle Between Vectors
gFloat Vector::operator/ (const Vector& other) const
{
/*	gFloat dot = *this % other;
	gFloat mag1 = VectorMagnitude(*this);
	gFloat mag2 = VectorMagnitude(other);
	gFloat cosTheta = dot / (mag1 * mag2);
	return ACos(cosTheta);
*/
	return ACos((*this % other) / (this->Magnitude() * other.Magnitude()));
}

// Vector Equality
bool Vector::operator== (const Vector& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

bool Vector::operator!= (const Vector& other) const
{
	return !(*this == other);
}

// Default comparatison operator for using Vector in STL containers
bool Vector::operator< (const Vector& other) const
{
	if(z < other.z) return true; else if(z > other.z) return false;
	if(y < other.y) return true; else if(y > other.y) return false;
	if(x < other.x) return true;
	return false;
}

// Vector Magnitude
Vector::operator gFloat() const
{
	return Sqrt(x*x + y*y + z*z);
}

#pragma endregion Operator Overloads

#pragma region Functions
// Return this Vector + other
Vector& Vector::Add(const Vector& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

// Return this Vector - other
Vector& Vector::Subtract(const Vector& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

// Return this Vector * scalar	(Vector-Scalar Multiplication)
Vector& Vector::Multiply(gFloat scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

// Return this Vector * m	(Vector-Matrix Multiplication)
Vector& Vector::Multiply(const Matrix& m)
{
	gFloat a=x, b=y, c=z, w;
	x = m(0,0)*a + m(1,0)*b + m(2,0)*c + m(3,0);
	y = m(0,1)*a + m(1,1)*b + m(2,1)*c + m(3,1);
	z = m(0,2)*a + m(1,2)*b + m(2,2)*c + m(3,2);
	
/*	w = m(0,3)*a + m(1,3)*b + m(2,3)*c + m(3,3);
	if(w != (gFloat)1.0 && w != (gFloat)1.0)
	{
		x /= w;
		y /= w;
		z /= w;
	}
*/
	return *this;
}

Vector& Vector::MultiplyProjection(const Matrix& m)
{
	gFloat a=x, b=y, c=z, w;
	x = m(0,0)*a + m(1,0)*b + m(2,0)*c + m(3,0);
	y = m(0,1)*a + m(1,1)*b + m(2,1)*c + m(3,1);
	z = m(0,2)*a + m(1,2)*b + m(2,2)*c + m(3,2);
	
	w = m(0,3)*a + m(1,3)*b + m(2,3)*c + m(3,3);
	if(w != (gFloat)1.0 && w != (gFloat)1.0)
	{
		x /= w;
		y /= w;
		z /= w;
	}

	return *this;
}

// Return this Vector * q (Vector-Quaternion Multiplication)
Vector& Vector::Multiply(const Quaternion& q)
{
	Quaternion vectorQuat(x, y, z, 0.0f);		// Quaternion representation of this vector
	Quaternion result = (q.Normalized() * vectorQuat * q.Normalized().Conjugated());

	x = result.x;
	y = result.y;
	z = result.z;
	return *this;
}

// Return this Vector * scalar
Vector& Vector::Divide(gFloat scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}

// Return negative version of this Vector
Vector Vector::Negated() const
{
	return Vector(-x, -y, -z);
}

// Negate and return this Vector
Vector& Vector::Negate()
{
	x = -x;
	y = -y;
	z = -z;
	return *this;
}

// Return Normalized form of this Vector
Vector Vector::Normalized() const
{
	if(x==0.0f && y==0.0f && z==0.0f)
		return Vector(0.0f,0.0f,0.0f);

	gFloat mag = Sqrt(x*x + y*y + z*z);
	return Vector(x/mag, y/mag, z/mag);
}

// Normalize and return this Vector
Vector& Vector::NormalizeInPlace()
{
	if(x==0.0f && y==0.0f && z==0.0f)
		return *this;

	gFloat mag = Sqrt(x*x + y*y + z*z);
	x /= mag;
	y /= mag;
	z /= mag;
	return *this;
}

// Return Dot Product of this Vector and other
gFloat Vector::DotProduct(const Vector& other) const
{
	return (x*other.x + y*other.y + z*other.z);
}

// Return Cross Product of this Vector and other	(this cross other)
Vector Vector::CrossProduct(const Vector& other) const
{
	return Vector(y*other.z - z*other.y, 
				z*other.x - x*other.z, 
				x*other.y - y*other.x);
}

// Return the Magnitude of this Vector
gFloat Vector::Magnitude() const
{
	return Sqrt(x*x + y*y + z*z);
}

// Return the Squared Magnitude of thei Vector
gFloat Vector::SquaredMagnitude() const
{
	return x*x + y*y + z*z;
}

// Set this vector to all 0's
void Vector::Zero()
{
	x = y = z = 0.0f;
}

// Is this Vector a Zero vector?
bool Vector::IsZero() const
{
	return Abs(x) < EPSILON && Abs(y) < EPSILON && Abs(z) < EPSILON;
}

// Eliminate any parameter that may be due to round-off error.
Vector& Vector::Cleanse(gFloat epsilon/*=1.0e-5*/)
{
	if(Abs(x) < epsilon)	x = 0.0f;
	if(Abs(y) < epsilon)	y = 0.0f;
	if(Abs(z) < epsilon)	z = 0.0f;
	return (*this);
}
#pragma endregion Functions



// Global Static Vector Functions
Vector Vector::VectorAddition(const Vector& v1, const Vector& v2)
{
	return Vector(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector Vector::VectorSubtraction(const Vector& v1, const Vector& v2)
{
	return Vector(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vector Vector::VectorMatrixMultiplication(const Vector& v, const Matrix& m)
{
	Vector v2(m(0,0)*v.x + m(1,0)*v.y + m(2,0)*v.z + m(3,0),
				m(0,1)*v.x + m(1,1)*v.y + m(2,1)*v.z + m(3,1),
				m(0,2)*v.z + m(1,2)*v.y + m(2,2)*v.z + m(3,2));

/*	gFloat w = m(0,3)*x + m(1,3)*y + m(2,3)*z + m(3,3);
	if(w != (gFloat)1.0 && w != (gFloat)1.0)
	{
		v2.x /= w;
		v2.y /= w;
		v2.z /= w;
	}
*/
	return v2;
}

gFloat Vector::VectorDotProduct(const Vector& v1, const Vector& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vector Vector::VectorCrossProduct(const Vector& v1, const Vector& v2)
{
	return Vector(v1.y*v2.z - v1.z*v2.y, 
				v1.z*v2.x - v1.x*v2.z, 
				v1.x*v2.y - v1.y*v2.x);
}

gFloat Vector::VectorMagnitude(const Vector& vec)
{
	return Sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

gFloat Vector::VectorSquaredMagnitude(const Vector& vec)
{
	return vec.x*vec.x + vec.y*vec.y + vec.z*vec.z;
}

Vector Vector::VectorNormalized(const Vector& vec)
{
	if(vec.x==0.0f && vec.y==0.0f && vec.z==0.0)
		return Vector(0.0f, 0.0f, 0.0f);
	gFloat mag = Sqrt(vec.x*vec.x + vec.y*vec.y + vec.z+vec.z);
	return Vector(vec.x / mag, vec.y / mag, vec.z / mag);
}

gFloat Vector::AngleBetweenVectors(const Vector& v1, const Vector& v2)
{
	// cos(theta) = Dot(v1,v2) / ( |v1| * |v2| )
	return ACos((VectorDotProduct(v1,v2)) / (VectorMagnitude(v1) * VectorMagnitude(v2)));
}

// Projection of v1(b) onto v2(a)
Vector Vector::VectorProjection(const Vector& v1, const Vector& v2)
{
	gFloat theta = AngleBetweenVectors(v2, v1);	// Angle between v2 and v1
	gFloat mag1 = VectorMagnitude(v1);			// Magnitude of v1
	gFloat mag2 = VectorMagnitude(v2);			// Magnitude of v2
	gFloat componentB = mag1 * Cos(theta);		// Component of v1(b) in the direction of v1(a)
	
	Vector proj(v2);
	proj /= mag2;
	proj *= componentB;

	return proj;
}

Vector Vector::ZeroVector()
{
	return Vector(0.0f, 0.0f, 0.0f);
}

// Component-wise Min/Max
Vector Vector::VectorMin(const Vector& v1, const Vector& v2)
{
	return Vector(
		v1.x < v2.x ? v1.x : v2.x,
		v1.y < v2.y ? v1.y : v2.y,
		v1.z < v2.z ? v1.z : v2.z);
}
Vector Vector::VectorMax(const Vector& v1, const Vector& v2)
{
	return Vector(
		v1.x > v2.x ? v1.x : v2.x,
		v1.y > v2.y ? v1.y : v2.y,
		v1.z > v2.z ? v1.z : v2.z);
}