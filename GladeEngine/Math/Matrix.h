#pragma once
#ifndef GLADE_MATRIX_H
#define GLADE_MATRIX_H

#ifndef GLADE_QUATERNION_H
#include "Quaternion.h"
#endif
#ifndef GLADE_VECTOR_H
#include "Vector.h"
#endif
#ifndef GLADE_PRECISION_H
#include "Precision.h"
#endif

#include <assert.h>

namespace Glade {
// Forward Declaration
class Quaternion;
class Vector;

class Matrix
{
public:

	// Constructors
	Matrix();
	Matrix(gFloat components[]);
	Matrix(gFloat mat[][4]);
	Matrix(const Quaternion& q);
	Matrix(gFloat angle, const Vector& axis);
	Matrix(gFloat phi, gFloat theta, gFloat psi);
	Matrix(Vector* trans, Quaternion* q, Vector* scale);
private:
	Matrix(gFloat literallyUseless);
public:
	Matrix& operator= (const Matrix& mat);
	
	// Operator Overloads
	gFloat (&operator[] (unsigned int index))[4];
	const gFloat* operator[] (unsigned int index) const;
	gFloat& operator() (unsigned int i, unsigned int j);
	gFloat  operator() (unsigned int i, unsigned int j) const;
	Matrix	operator+ (const Matrix& mat) const;
	Matrix& operator+= (const Matrix& mat);
	Matrix	operator- (const Matrix& mat) const;
	Matrix& operator-= (const Matrix& mat);
	Matrix	operator* (gFloat scalar) const;
	Matrix&	operator*= (gFloat scalar);
	Matrix	operator* (const Matrix& mat) const;
	Matrix&	operator*= (const Matrix& mat);
	Matrix& operator- ();
	bool	operator== (const Matrix& mat) const;
	bool	operator!= (const Matrix& mat) const;

	// Functions
	gFloat (&GetArray())[16];
	Matrix& Identity();
	bool	IsIdentity();
	gFloat	Determinant() const;
	Matrix  Inverse3() const;
	Matrix	Inverse4() const;
	void	InvertInPlace3();
	void	InvertInPlace4();
	Matrix	Transpose() const;
	Matrix	Transpose3() const;
	Matrix& TransposeInPlace();
	Matrix& Transpose3InPlace();
	void	EulerAngles(gFloat& phi, gFloat& theta, gFloat& psi, bool solution2=false) const;
	void	Decompose(Vector& scale, Quaternion& rot, Vector& trans);
	void	ComposeTransformationMatrix(Vector* trans, Quaternion* q, Vector* scale);
	void	SetBasis(Vector x, Vector y, Vector z);
	Quaternion ConvertToQuaternion() const;
	Quaternion ConvertToQuaternion2() const;
	Matrix  GetMatrix3() const;
	Matrix	Multiply3(const Matrix& mat) const;
	Vector	Multiply3(const Vector& v) const;
	Matrix	MultiplyInverse3(const Matrix& mat) const;
	Vector	MultiplyInverse3(const Vector& v) const;

	// Transformations
	void Translate(const Vector& v);
	void Translate(gFloat x, gFloat y, gFloat z);
	void Scale(gFloat scale);
	void Scale(gFloat sx, gFloat sy, gFloat sz);
	void Rotate(gFloat angle, const Vector& axis);
	void Rotate(gFloat angle, gFloat x, gFloat y, gFloat z);
	void RotateX(gFloat angle);
	void RotateY(gFloat angle);
	void RotateZ(gFloat angle);
	void ShearXY(gFloat shx, gFloat shy);
	void ShearYZ(gFloat shy, gFloat shz);
	void ShearZX(gFloat shx, gFloat shz);

	// Static Functions
	static Matrix SkewSymmetricMatrix(const Vector& V);
	static Matrix MatrixFromXAxisRotation(gFloat angle);
	static Matrix MatrixFromYAxisRotation(gFloat angle);
	static Matrix MatrixFromZAxisRotation(gFloat angle);
	static Matrix MatrixFromAxisAngle(gFloat angle, const Vector& vec);
	static Matrix MatrixFromAxisAngle(gFloat angle, gFloat x, gFloat y, gFloat z);
	static Matrix LookAt(const Vector& eye, const Vector& target, const Vector& up);
	static Matrix Perspective(gFloat fieldOfView, gFloat aspect, gFloat zNear, gFloat zFar);
	static Matrix Ortho(gFloat width, gFloat height, gFloat zNear, gFloat zFar);

	// Inverse Inertia Tensors
	static Matrix CuboidInverseInertiaTensor(gFloat mass, Vector fullWidths);
	static Matrix SphereInverseInertiaTensor(gFloat mass, gFloat radius);
	static Matrix HollowSphereInverseInertiaTensor(gFloat mass, gFloat radius);
	static Matrix CylinderInverseInertiaTensor(gFloat mass, gFloat height, gFloat radius);
	static Matrix CylindricalTubeInverseInertiaTensor(gFloat mass, gFloat height, gFloat oRadius, gFloat iRadius);
	static Matrix ConeInverseInertiaTensor(gFloat mass, gFloat height, gFloat radius);
	static Matrix CapsuleInverseInertiaTensor(gFloat mass, gFloat height, gFloat radius);

private:
	union {
		struct {
			gFloat	_00, _01, _02, _03;			// r00	r01	r02	0	r## contain the combination or
			gFloat	_10, _11, _12, _13;			// r10	r11	r12	0	rotation, scaling, and any shear transformations
			gFloat	_20, _21, _22, _23;			// r20	r21	r22	P	(P only used for perspective Matrices)
			gFloat	_30, _31, _32, _33;			// tx	ty	tz	1	t is the position/translation 
		};
		gFloat m[4][4];
		gFloat mArray[16];
	};

public:
	const static Matrix INFINITE_MASS_INERTIA_TENSOR;
};
}	// namespace
#endif	// GLADE_MATRIX_H