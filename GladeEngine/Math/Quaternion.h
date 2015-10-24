#pragma once
#ifndef GLADE_QUATERNION_H
#define GLADE_QUATERNION_H

#ifndef GLADE_MATRIX_H
#include "Matrix.h"
#endif
#ifndef GLADE_VECTOR_H
#include "Vector.h"
#endif
#ifndef GLADE_PRECISION_H
#include "Precision.h"
#endif

namespace Glade {
// Forward Declaration
class Matrix;
class Vector;

class Quaternion
{
public:
	gFloat x, y, z, w;	//x, y and z are the imaginary components and w is the real component of the Quaternion

	// Constructors
	Quaternion();
	Quaternion(gFloat components[]);
	Quaternion(gFloat _x, gFloat _y, gFloat _z, gFloat _w);
	Quaternion(gFloat phi, gFloat theta, gFloat psi);
	Quaternion(const Vector& startVec, const Vector& endVec);
	Quaternion(const Vector& v, gFloat angle);
	Quaternion(const Quaternion& quat);
	Quaternion& operator=(const Quaternion& other);

	// Destructor
	~Quaternion();

	// Operator Overloads
	gFloat& operator[] (int index);
	const gFloat& operator[] (int index) const;
	Quaternion operator+ (const Quaternion& other) const;
	Quaternion& operator+= (const Quaternion& other);
	Quaternion operator- (const Quaternion& other) const;
	Quaternion& operator-= (const Quaternion& other);
	Quaternion operator* (const Quaternion& other) const;
	Quaternion& operator*= (const Quaternion& other);
	Quaternion operator* (gFloat scalar) const;
	Quaternion& operator*= (gFloat scalar);
	Quaternion& operator~();
	Quaternion& operator-();
	gFloat operator% (const Quaternion& other) const;
	bool operator== (const Quaternion& other) const;
	bool operator!= (const Quaternion& other) const;
	operator gFloat() const;

	// Functions
	Quaternion& Add(const Quaternion& other);
	Quaternion& Subtract(const Quaternion& other);
	Quaternion& Multiply(const Quaternion& other);
	Quaternion& Scale(gFloat scalar);
	Quaternion	Conjugated() const;
	Quaternion&	ConjugateInPlace();
	Quaternion	Inverse() const;
	Quaternion& InvertInPlace();
	Quaternion	Negated() const;
	Quaternion& NegateInPlace();
	Quaternion	Normalized() const;
	Quaternion& NormalizeInPlace();
	gFloat		DotProduct(const Quaternion& other) const;
	gFloat		Magnitude() const;
	Matrix		ConvertToMatrix() const;	
	void		EulerAngles(gFloat& phi, gFloat& theta, gFloat& psi) const;
	void		AxisAngle(Vector& v, gFloat& angle);
	Quaternion& Identity();
	bool		IsIdentity();

	// Static Functions
	static Quaternion QuaternionAddition(const Quaternion& q1, const Quaternion& q2);
	static Quaternion QuaternionSubtraction(const Quaternion& q1, const Quaternion& q2);
	static Quaternion QuaternionMultiplication(const Quaternion& q1, const Quaternion& q2);
	static Quaternion QuaternionScale(const Quaternion& q, gFloat scalar);
	static Quaternion QuaternionNegate(const Quaternion& q);
	static Quaternion QuaternionConjugate(const Quaternion& q);
	static Matrix QuaternionToMatrix(const Quaternion& q);
	static gFloat* QuaternionEulerAngles(Quaternion& q);
	static Quaternion QuaternionFromTwoVectors(const Vector& startVec, const Vector& endVec);
	static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, gFloat t);
};
}	// namespace
#endif	// GLADE_QUATERNION_H

// Quaternion Math Notes:
//
// Integrating Quaternions representing orientations:
//		q(t) - orientation quaternion | w - angular velocity
//		
//		dq/dt = (1/2) * w * q(t)
//		q(t+Δt) = q(t) + (dq/dt * Δt) = q(t) + ((1/2) * Δt * w * q(t))
//		https://fgiesen.wordpress.com/2012/08/24/quaternion-differentiation/#comment-3913
//
// Rotation between 2 quaternions:
//		For some quaternion q' such that q1 * q' = q2:
//		
//		q' = q1^-1 * q2  ->  q2 = q1 * q'
//		q' = q2 * q1^-1  ->  q2 = q' * q1
//