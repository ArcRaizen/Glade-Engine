#pragma once
#ifndef GLADE_VECTOR_H
#define GLADE_VECTOR_H

#ifndef GLADE_MATRIX_H
#include "Matrix.h"
#endif
#ifndef GLADE_QUATERNION_H
#include "Quaternion.h"
#endif
#ifndef GLADE_PRECISION_H
#include "Precision.h"
#endif

namespace Glade {
class Matrix;
class Quaternion;

class Vector
{
public:
	gFloat x,y,z;	// x, y and z components of the Vector
private:
	gFloat pad;		// ensure four word alignment

public:
	// Constructors
	Vector();
	Vector(const gFloat components[]);
	Vector(const gFloat _x, const gFloat _y, const gFloat _z);
	Vector(const Vector& start, const Vector& end);
	Vector(const Vector& other);
	Vector& operator= (const Vector& other);

	// Destructor
	~Vector();

	// Operator Overloads
	gFloat& operator[](unsigned int index);
	const gFloat& operator[] (unsigned int index) const;
	Vector operator- () const;
	Vector operator* (gFloat scalar) const;
	Vector& operator*= (gFloat scalar);
	Vector operator* (const Matrix& m) const;
	Vector& operator*= (const Matrix& m);
	Vector operator* (const Quaternion& q) const;
	Vector& operator*= (const Quaternion& q);
	Vector operator/ (gFloat scalar) const;
	Vector& operator/= (gFloat scalar);
	Vector operator+ (const Vector& other) const;
	Vector& operator+= (const Vector& other);
	Vector operator- (const Vector& other) const;
	Vector& operator-= (const Vector& other);
	gFloat operator% (const Vector& other) const;
	Vector operator* (const Vector& other) const;
	gFloat operator/ (const Vector& other) const;
	bool operator== (const Vector& other) const;
	bool operator!= (const Vector& other) const;
	bool operator<  (const Vector& other) const;
	operator gFloat() const;

	// Functions
	Vector& Add(const Vector& other);
	Vector& Subtract(const Vector& other);
	Vector& Multiply(gFloat scalar);
	Vector& Multiply(const Matrix& m);
	Vector& MultiplyProjection(const Matrix& m);
	Vector& Multiply(const Quaternion& q);
	Vector& Divide(gFloat scalar);
	Vector	Negated() const;
	Vector& Negate();
	Vector	Normalized() const;
	Vector& NormalizeInPlace();
	gFloat	DotProduct(const Vector& other) const;
	Vector	CrossProduct(const Vector& other) const;
	gFloat	Magnitude() const;
	gFloat  SquaredMagnitude() const;
	void	Zero();
	bool	IsZero() const;
	Vector& Cleanse();

	// Static Functions
	static Vector VectorAddition(const Vector& v1, const Vector& v2);
	static Vector VectorSubtraction(const Vector& v1, const Vector& v2);
	static Vector VectorMatrixMultiplication(const Vector& v, const Matrix& m);
	static gFloat VectorDotProduct(const Vector& v1, const Vector& v2);
	static Vector VectorCrossProduct(const Vector& v1, const Vector& v2);
	static gFloat VectorMagnitude(const Vector& vec);
	static gFloat VectorSquaredMagnitude(const Vector& vec);
	static Vector VectorNormalized(const Vector& vec);
	static gFloat AngleBetweenVectors(const Vector& v1, const Vector& v2);
	static Vector VectorProjection(const Vector& v1, const Vector& v2);
	static Vector ZeroVector();
	static Vector VectorMin(const Vector& v1, const Vector& v2);
	static Vector VectorMax(const Vector& v1, const Vector& v2);

	// Static Vectors
	const static Vector GRAVITY;
	const static Vector UP;
	const static Vector DOWN;
	const static Vector RIGHT;
	const static Vector X_AXIS;
	const static Vector Y_AXIS;
	const static Vector Z_AXIS;
};
}
#endif	// GLADE_VECTOR_H