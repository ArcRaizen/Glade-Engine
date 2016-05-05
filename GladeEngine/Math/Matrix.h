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

	// Casting Operators (defined inline below)
	// USE WITH CAUTION - Returns raw pointer to memory.
	// Assume its an 16-length array, NEVER go past that.
	explicit operator float* ();
	explicit operator const float* () const;
	
	// Operator Overloads
	float& operator() (unsigned int i, unsigned int j);
	float  operator() (unsigned int i, unsigned int j) const;
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
	Matrix& Identity();
	bool	IsIdentity();
	Matrix& Zero();
	bool	IsZero();
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
	Matrix  GetMatrix3() const;
	Matrix	Times3(const Matrix& mat) const;
	Vector	Times3(const Vector& v) const;
	Matrix	Transpose3Times(const Matrix& mat) const;
	Vector	Transpose3Times(const Vector& v) const;
	Matrix	TimesTranspose3(const Matrix& mat) const;
	Matrix  Transpose3TimesTranspose3(const Matrix& mat) const;

	// Transformations
	void Translate(const Vector& v);
	void Translate(gFloat x, gFloat y, gFloat z);
	void Scale(gFloat scale);
	void Scale(gFloat sx, gFloat sy, gFloat sz);
	void Rotate(gFloat angle, const Vector& axis);
	void Rotate(gFloat angle, gFloat x, gFloat y, gFloat z);
	void Rotate(Matrix mat);
	void RotateX(gFloat angle);
	void RotateY(gFloat angle);
	void RotateZ(gFloat angle);
	void ShearXY(gFloat shx, gFloat shy);
	void ShearYZ(gFloat shy, gFloat shz);
	void ShearZX(gFloat shx, gFloat shz);

	// Static Functions
	static Matrix IdentityMatrix();
	static Matrix SkewSymmetricMatrix(const Vector& v);
	static Matrix MatrixFromTranslation(const Vector& trans);
	static Matrix MatrixFromXAxisRotation(gFloat angle);
	static Matrix MatrixFromYAxisRotation(gFloat angle);
	static Matrix MatrixFromZAxisRotation(gFloat angle);
	static Matrix MatrixFromAxisAngle(gFloat angle, const Vector& vec);
	static Matrix MatrixFromAxisAngle(gFloat angle, gFloat x, gFloat y, gFloat z);
	static Matrix LookAt(const Vector& eye, const Vector& target, const Vector& up);
	static Matrix FPSView(const Vector& eye, gFloat pitch, gFloat yaw, gFloat roll=gFloat(0.0f));
	static Matrix Perspective(gFloat fieldOfView, gFloat aspect, gFloat zNear, gFloat zFar);
	static Matrix Ortho(gFloat width, gFloat height, gFloat zNear, gFloat zFar);

	// Inverse Inertia Tensors
	static Matrix CuboidInertiaTensor(gFloat mass, Vector fullWidths);
	static Matrix SphereInertiaTensor(gFloat mass, gFloat radius);
	static Matrix HollowSphereInertiaTensor(gFloat mass, gFloat radius);
	static Matrix CylinderInertiaTensor(gFloat mass, gFloat height, gFloat radius);
	static Matrix CylindricalTubeInertiaTensor(gFloat mass, gFloat height, gFloat oRadius, gFloat iRadius);
	static Matrix ConeInertiaTensor(gFloat mass, gFloat height, gFloat radius);
	static Matrix CapsuleInertiaTensor(gFloat mass, gFloat height, gFloat radius);

private:
	// Data in Matrix MUST be stored as a float, not gFloat.
	// Do to interaction with DirectX functions, this data absolutely needs to be a float to work.
	// If you do not NEED to acess this data specficially as an array of floats, this can be changed to gFloat.
	// If someone can find how to access this gFloat data as an array of floats even with gFloats being double precision,
	// go right ahead and do that.
	union {
		struct {
			float	_00, _01, _02, _03;			// r00	r01	r02	0	r## contain the combination or
			float	_10, _11, _12, _13;			// r10	r11	r12	0	rotation, scaling, and any shear transformations
			float	_20, _21, _22, _23;			// r20	r21	r22	P	(P only used for perspective Matrices)
			float	_30, _31, _32, _33;			// tx	ty	tz	1	t is the position/translation 
		};
		float m[4][4];
	};

public:
	const static Matrix INFINITE_MASS_INERTIA_TENSOR;
};

__forceinline
Matrix::operator float* ()
{
	return (float*)&_00;
}

__forceinline
Matrix::operator const float* () const
{
	 return (const float *) &_00;
}
}	// namespace
#endif	// GLADE_MATRIX_H