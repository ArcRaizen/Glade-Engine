#include "..\stdafx.h"
#include "Matrix.h"

using namespace Glade;

const Matrix Matrix::INFINITE_MASS_INERTIA_TENSOR = Matrix(0.0f);

// Default Constructor - Creates Identity Matrix
Matrix::Matrix(bool _3x3) : _01(0.0), _02(0.0), _03(0.0), _10(0.0), _12(0.0), _13(0.0), _20(0.0), _21(0.0), _23(0.0), _30(0.0), _31(0.0), _32(0.0),
					_00(1.0), _11(1.0), _22(1.0), _33(1.0), is3x3(_3x3)
{ }

// Matrix from array
Matrix::Matrix(gFloat components[], bool _3x3) : _00(components[0]), _01(components[1]), _02(components[2]), _03(components[3]), 
									_10(components[4]), _11(components[5]), _12(components[6]), _13(components[7]), 
									_20(components[8]), _21(components[9]), _22(components[10]), _23(components[11]),
									_30(components[12]), _31(components[13]), _32(components[14]), _33(components[15])
{ }

// Matrix from 2d array
Matrix::Matrix(gFloat mat[][4], bool _3x3) : _00(mat[0][0]), _01(mat[0][1]), _02(mat[0][2]), _03(mat[0][3]), 
									_10(mat[1][0]), _11(mat[1][1]), _12(mat[1][2]), _13(mat[1][3]), 
									_20(mat[2][0]), _21(mat[2][1]), _22(mat[2][2]), _23(mat[2][3]),
									_30(mat[3][0]), _31(mat[3][1]), _32(mat[3][2]), _33(mat[3][3]), is3x3(_3x3)
{ }

// Matrix from Quaternion
Matrix::Matrix(const Quaternion& q, bool _3x3) : _03(0.0), _13(0.0), _23(0.0), _30(0.0), _31(0.0), _32(0.0), _33(10.), is3x3(_3x3)
{
#ifdef LEFT_HANDED_COORDS
	_00 = 1 - 2*q.y*q.y - 2*q.z*q.z;		_01 = 2*q.x*q.y - 2*q.z*q.w;			_02 = 2*q.x*q.z + 2*q.y*q.w;
	_10 = 2*q.x*q.y + 2*q.z*q.w;			_11 = 1 - 2*q.x*q.x - 2*q.z*q.z;		_12 = 2*q.y*q.z - 2*q.x*q.w;
	_20 = 2*q.x*q.z - 2*q.y*q.w;			_21 = 2*q.y*q.z + 2*q.x*q.w;			_22 = 1 - 2*q.x*q.x - 2*q.y*q.y;
#else
	_00 = 1 - 2*q.y*q.y - 2*q.z*q.z;		_01 = 2*q.x*q.y + 2*q.z*q.w;			_02 = 2*q.x*q.z - 2*q.y*q.w;
	_10 = 2*q.x*q.y - 2*q.z*q.w;			_11 = 1 - 2*q.x*q.x - 2*q.z*q.z;		_12 = 2*q.y*q.z + 2*q.x*q.w;
	_20 = 2*q.x*q.z + 2*q.y*q.w;			_21 = 2*q.y*q.z - 2*q.x*q.w;			_22 = 1 - 2*q.x*q.x - 2*q.y*q.y;
#endif
}

// Matrix from Axis-Angle
Matrix::Matrix(gFloat angle, const Vector& axis, bool _3x3) : _03(0.0), _13(0.0), _23(0.0), _30(0.0), _31(0.0), _32(0.0), _33(1.0), is3x3(_3x3)
{
	// Normalize vector [xyz] for safety
	gFloat x=axis.x, y=axis.y, z=axis.z;
	gFloat mag = Sqrt(x*x + y*y + z*z);
	x /= mag;	y /= mag;	z /= mag;

	// Calculate the rotation matrix
	gFloat cos_ = Cos(angle);
	gFloat sin_ = Sin(angle);
	gFloat _1MinCos = (gFloat)1.0f - cos_;

#ifdef LEFT_HANDED_COORDS
	_00 = cos_+(x*x*_1MinCos);		_01 = x*y*_1MinCos+(z*sin_);	_02 = x*z*_1MinCos-(y*sin_);
	_10 = y*x*_1MinCos-(z*sin_);	_11 = cos_+(y*y*_1MinCos);		_12 = y*z*_1MinCos+(x*sin_);
	_20 = z*x*_1MinCos+(y*sin_);	_21 = z*y*_1MinCos-(x*sin_);	_22 = cos_+(z*z*_1MinCos);
#else
	_00 = cos_+(x*x*_1MinCos);		_01 = x*y*_1MinCos-(z*sin_);	_02 = x*z*_1MinCos+(y*sin_);
	_10 = y*x*_1MinCos+(z*sin_);	_11 = cos_+(y*y*_1MinCos);		_12 = y*z*_1MinCos-(x*sin_);
	_20 = z*x*_1MinCos-(y*sin_);	_21 = z*y*_1MinCos+(x*sin_);	_22 = cos_+(z*z*_1MinCos);
#endif
}


// Matrix from Euler Angles
Matrix::Matrix(gFloat phi, gFloat theta, gFloat psi, bool _3x3) : _03(0.0), _13(0.0), _23(0.0), _30(0.0), _31(0.0), _32(0.0), _33(1.0), is3x3(_3x3)
{
	gFloat cosP_ = Cos(phi),	sinP_ = Sin(phi);
	gFloat cosT_ = Cos(theta),	sinT_ = Sin(theta);
	gFloat cosS_ = Cos(psi),	sinS_ = Sin(psi);

#ifdef LEFT_HANDED_COORDS
	_00 = cosT_*cosS_;		_01 = sinP_*sinT_*cosS_-cosP_*sinS_;		_02 = cosP_*sinT_*cosS_+sinP_*sinS_;
	_10 = cosT_*sinS_;		_11 = sinP_*sinT_*sinS_+cosP_*cosS_;		_12 = cosP_*sinT_*sinS_-sinP_*cosS_;
	_20 = -sinT_;			_21 = sinP_*cosT_;							_22 = cosP_*cosT_;
#else
	_00 = cosT_*cosS_;		_01 = sinP_*sinT_*cosS_+cosP_*sinS_;		_02 = sinP_*sinS_-cosP_*sinT_*cosS_;
	_10 = -cosT_*sinS_;		_11 = cosP_*cosS_-sinP_*sinT_*sinS_;		_12 = cosP_*sinT_*sinS_+sinP_*cosS_;
	_20 = sinT_;			_21 = -sinP_*cosT_;							_22 = cosP_*cosT_;
#endif
}

// Matrix from Affine Transformations
Matrix::Matrix(Vector* trans, Quaternion* q, Vector* scale, bool _3x3) : _01(0.0), _02(0.0), _03(0.0), _10(0.0), _12(0.0), _13(0.0), _20(0.0), _21(0.0), _23(0.0), _30(0.0), _31(0.0), _32(0.0),
																		_00(1.0), _11(1.0), _22(1.0), _33(1.0), is3x3(_3x3)
{
	if(q != nullptr)
	{
		gFloat scaleX=1.0f, scaleY=1.0f, scaleZ=1.0f;
		if(scale != nullptr)
		{
			scaleX = scale->x;
			scaleY = scale->y;
			scaleZ = scale->z;
		}

#ifdef LEFT_HANDED_COORDS
		_00 = (1.0f - (2.0f * (q->y*q->y + q->z*q->z))) * scaleX;	_01 = (2.0f * (q->x*q->y + q->z*q->w)) * scaleX;			_02 = (2.0f * (q->x*q->z - q->y*q->w)) * scaleX;
		_10 = (2.0f * (q->x*q->y - q->z*q->w)) * scaleY;			_11 = (1.0f - (2.0f * (q->x*q->x + q->z*q->z))) * scaleY;	_12 = (2.0f * (q->y*q->z + q->x*q->w)) * scaleY;
		_20 = (2.0f * (q->x*q->z + q->y*q->w)) * scaleZ;			_21 = (2.0f * (q->y*q->z - q->x*q->w)) * scaleZ;			_22 = (1.0f - (2.0f * (q->x*q->x + q->y*q->y))) * scaleZ;
#else
		_00 = (1.0f - (2.0f * (q->y*q->y - q->z*q->z))) * scaleX;	_01 = (2.0f * (q->x*q->y - q->z*q->w)) * scaleX;			_02 = (2.0f * (q->x*q->z + q->y*q->w)) * scaleX;
		_10 = (2.0f * (q->x*q->y + q->z*q->w)) * scaleY;			_11 = (1.0f - (2.0f * (q->x*q->x - q->z*q->z))) * scaleY;	_12 = (2.0f * (q->y*q->z - q->x*q->w)) * scaleY;
		_20 = (2.0f * (q->x*q->z - q->y*q->w)) * scaleZ;			_21 = (2.0f * (q->y*q->z + q->x*q->w)) * scaleZ;			_22 = (1.0f - (2.0f * (q->x*q->x - q->y*q->y))) * scaleZ;
#endif
	}
	else if(scale != nullptr)
	{
		_00 = scale->x;
		_11 = scale->y;
		_22 = scale->z;
	}
	else
		_00 = _11 = _22 = 1.0f;

	if(trans != nullptr)
	{
		_30 = trans->x;
		_31 = trans->y;
		_32 = trans->z;
	}
}

Matrix::Matrix(gFloat literallyUselss) : _00(0.0f), _01(0.0f), _02(0.0f), _03(0.0f), _10(0.0f), _11(0.0f), _12(0.0f), _13(0.0f),
										_20(0.0f), _21(0.0f), _22(0.0f), _23(0.0f), _30(0.0f), _31(0.0f), _32(0.0f), _33(1.0f)
{}

Matrix& Matrix::operator= (const Matrix& mat)
{
	_00 = mat._00;	_01 = mat._01;	_02 = mat._02;	_03 = mat._03;
	_10 = mat._10;	_11 = mat._11;	_12 = mat._12;	_13 = mat._13;
	_20 = mat._20;	_21 = mat._21;	_22 = mat._22;	_23 = mat._23;
	_30 = mat._30;	_31 = mat._31;	_32 = mat._32;	_33 = mat._33;
	if(is3x3) { _30 = _31 = _32 = 0.0f; _33 = 1.0f; }
	return *this;
}

#pragma region Operator Overloads
// Indexing
gFloat (&Matrix::operator[] (unsigned int index))[4]
{
	assert(index < 4);
	return m[index];
}

const gFloat* Matrix::operator[] (unsigned int index) const
{
	assert(index < 4);
	return m[index];
}

gFloat& Matrix::operator() (unsigned int i, unsigned int j)
{
	assert(i < 4 && j < 4);
	return m[i][j];
}
gFloat Matrix::operator() (unsigned int i, unsigned int j) const
{
	assert(i < 4 && j < 4);
	return m[i][j];
}

// Matrix Addition
Matrix Matrix::operator+ (const Matrix& mat) const
{
	gFloat newMat[16] = {_00+mat._00, _01+mat._01, _02+mat._02, _03+mat._03,
						 _10+mat._10, _11+mat._11, _12+mat._12, _13+mat._13,
						 _20+mat._20, _21+mat._21, _22+mat._22, _23+mat._23,
						 _30+mat._30, _31+mat._31, _32+mat._32, _33+mat._33
						};
	return Matrix(newMat);
}

Matrix& Matrix::operator+= (const Matrix& mat)
{
	_00 += mat._00;	 _01 += mat._01;	_02 += mat._02;		_03 += mat._03;
	_10 += mat._10;	 _11 += mat._11;	_12 += mat._12;		_13 += mat._13;
	_20 += mat._20;	 _21 += mat._21;	_22 += mat._22;		_23 += mat._23;
	_30 += mat._30;	 _31 += mat._31;	_32 += mat._32;		_33 += mat._33;
	if(is3x3) { _30 = _31 = _32 = 0.0f; _33 = 1.0f; }
	return *this;
}

// Matrix Subtraction
Matrix Matrix::operator- (const Matrix& mat) const
{
	gFloat newMat[16] = {_00-mat._00, _01-mat._01, _02-mat._02, _03-mat._03,
						 _10-mat._10, _11-mat._11, _12-mat._12, _13-mat._13,
						 _20-mat._20, _21-mat._21, _22-mat._22, _23-mat._23,
						 _30-mat._30, _31-mat._31, _32-mat._32, _33-mat._33
						};
	return Matrix(newMat);
}
Matrix& Matrix::operator-= (const Matrix& mat)
{
	_00 -= mat._00;	 _01 -= mat._01;	_02 -= mat._02;		_03 -= mat._03;
	_10 -= mat._10;	 _11 -= mat._11;	_12 -= mat._12;		_13 -= mat._13;
	_20 -= mat._20;	 _21 -= mat._21;	_22 -= mat._22;		_23 -= mat._23;
	_30 -= mat._30;	 _31 -= mat._31;	_32 -= mat._32;		_33 -= mat._33;
	if(is3x3) { _30 = _31 = _32 = 0.0f; _33 = 1.0f; }
	return *this;
}

// Matrix Scalar Multiplication
Matrix Matrix::operator* (gFloat scalar) const
{
	gFloat newMat[16] = {_00*scalar, _01*scalar, _02*scalar, _03*scalar,
						 _10*scalar, _11*scalar, _12*scalar, _13*scalar,
						 _20*scalar, _21*scalar, _22*scalar, _23*scalar,
						 _30*scalar, _31*scalar, _32*scalar, _33*scalar
						};
	return Matrix(newMat);
}
Matrix&	Matrix::operator*= (gFloat scalar)
{
	_00 *= scalar;	 _01 *= scalar;	_02 *= scalar;	_03 *= scalar;
	_10 *= scalar;	 _11 *= scalar;	_12 *= scalar;	_13 *= scalar;
	_20 *= scalar;	 _21 *= scalar;	_22 *= scalar;	_23 *= scalar;
	_30 *= scalar;	 _31 *= scalar;	_32 *= scalar;	_33 *= scalar;
	return *this;
}

// Matrix Multiplication
Matrix Matrix::operator* (const Matrix& mat) const
{
	gFloat temp[16] = {
		_00*mat._00 + _01*mat._10 + _02*mat._20 + _03*mat._30,
		_00*mat._01 + _01*mat._11 + _02*mat._21 + _03*mat._31,
		_00*mat._02 + _01*mat._12 + _02*mat._22 + _03*mat._32,
		_00*mat._03 + _01*mat._13 + _02*mat._23 + _03*mat._33,
		_10*mat._00 + _11*mat._10 + _13*mat._20 + _13*mat._30,
		_10*mat._01 + _11*mat._11 + _12*mat._21 + _13*mat._31,
		_10*mat._02 + _11*mat._12 + _12*mat._22 + _13*mat._32,
		_10*mat._03 + _11*mat._13 + _12*mat._23 + _13*mat._33,
		_20*mat._00 + _21*mat._10 + _22*mat._20 + _23*mat._30,
		_20*mat._01 + _21*mat._11 + _22*mat._21 + _23*mat._31,
		_20*mat._02 + _21*mat._12 + _22*mat._22 + _23*mat._32,
		_20*mat._03 + _21*mat._13 + _22*mat._23 + _23*mat._33,
		_30*mat._00 + _31*mat._10 + _32*mat._20 + _33*mat._30,
		_30*mat._01 + _31*mat._11 + _32*mat._21 + _33*mat._31,
		_30*mat._02 + _31*mat._12 + _32*mat._22 + _33*mat._32,
		_30*mat._03 + _33*mat._13 + _32*mat._23 + _33*mat._33,
	};
	return Matrix(temp);
}
Matrix&	Matrix::operator*= (const Matrix& mat)
{
	gFloat temp[16] = {
		_00*mat._00 + _01*mat._10 + _02*mat._20 + _03*mat._30,
		_00*mat._01 + _01*mat._11 + _02*mat._21 + _03*mat._31,
		_00*mat._02 + _01*mat._12 + _02*mat._22 + _03*mat._32,
		_00*mat._03 + _01*mat._13 + _02*mat._23 + _03*mat._33,
		_10*mat._00 + _11*mat._10 + _12*mat._20 + _13*mat._30,
		_10*mat._01 + _11*mat._11 + _12*mat._21 + _13*mat._31,
		_10*mat._02 + _11*mat._12 + _12*mat._22 + _13*mat._32,
		_10*mat._03 + _11*mat._13 + _12*mat._23 + _13*mat._33,
		_20*mat._00 + _21*mat._10 + _22*mat._20 + _23*mat._30,
		_20*mat._01 + _21*mat._11 + _22*mat._21 + _23*mat._31,
		_20*mat._02 + _21*mat._12 + _22*mat._22 + _23*mat._32,
		_20*mat._03 + _21*mat._13 + _22*mat._23 + _23*mat._33,
		_30*mat._00 + _31*mat._10 + _32*mat._20 + _33*mat._30,
		_30*mat._01 + _31*mat._11 + _32*mat._21 + _33*mat._31,
		_30*mat._02 + _31*mat._12 + _32*mat._22 + _33*mat._32,
		_30*mat._03 + _33*mat._13 + _32*mat._23 + _33*mat._33
	};

	_00 = temp[0];	_01 = temp[1];	_02 = temp[2];	_03 = temp[3];
	_10 = temp[4];	_11 = temp[5];	_12 = temp[6];	_13 = temp[7];
	_20 = temp[8];	_21 = temp[9];	_22 = temp[10];	_23 = temp[11];
	_30 = temp[12];	_31 = temp[13];	_32 = temp[14];	_33 = temp[15];
	if(is3x3) { _30 = _31 = _32 = 0.0f; _33 = 1.0f; }
	return *this;
}

// Matrix Inversion
Matrix& Matrix::operator- ()
{
	// If 4th row of matrix is [0,0,0,1], then Matrix has no translation
	if(_30 == 0 && _31 == 0 && _32 == 0 && _33 == 1)
		return this->Inverse3();
	else	// 4x4 Affine Transformation Matrix Inverse
		return this->Inverse4();

}

// Equality
bool Matrix::operator== (const Matrix& mat) const
{
	return  _00 == mat._00 && _01 == mat._01 && _02 == mat._02 && _03 == mat._03 &&
			_10 == mat._10 && _11 == mat._11 && _12 == mat._12 && _13 == mat._13 &&
			_20 == mat._20 && _21 == mat._21 && _22 == mat._22 && _23 == mat._23 &&
			_30 == mat._30 && _31 == mat._31 && _32 == mat._32 && _33 == mat._33;
}

bool Matrix::operator!= (const Matrix& mat) const
{
	return  _00 != mat._00 || _01 != mat._01 || _02 != mat._02 || _03 != mat._03 ||
			_10 != mat._10 || _11 != mat._11 || _12 != mat._12 || _13 != mat._13 ||
			_20 != mat._20 || _21 != mat._21 || _22 != mat._22 || _23 != mat._23 ||
			_30 != mat._30 || _31 != mat._31 || _32 != mat._32 || _33 != mat._33;
}

#pragma endregion

#pragma region Functions
// Return matrix as an array of 16 floats
gFloat (&Matrix::GetArray())[16]
{
	return mArray;
}

// Set to and return Identity Matrix
Matrix& Matrix::Identity()
{
	_01 = _02 = _03 = _10 = _12 = _13 = _20 = _21 = _23 = _30 = _31 = _32 = 0;
	_00 = _11 = _22 = _33 = 1;
	return* this;
}

bool Matrix::IsIdentity()
{
	return  _00 == 1 && _01 == 0 && _02 == 0 && _03 == 0 &&
			_10 == 0 && _11 == 1 && _12 == 0 && _13 == 0 &&
			_20 == 0 && _21 == 0 && _22 == 1 && _23 == 0 &&
			_30 == 0 && _31 == 0 && _32 == 0 && _33 == 1;
}

// Calculate and return the determinent of this Matrix
gFloat Matrix::Determinant() const
{
	return (_00 * (_11*_22-_12*_21)) - 
			(_01 * (_12*_20-_10*_22)) + 
			(_02 * (_11*_20-_10*_21));
}

// Return inverse of 3x3 Matrix representing Rotation and Scaling
// ASSUMING NO SHEARING/REFLECTION/ETC! ONLY ROTATION & SCALE
Matrix Matrix::Inverse3() const
{
	// Decompose/Remove Scale components (length squared of vector component)
	gFloat scaleX = (_00*_00)+(_01*_01)+(_02*_02);
	gFloat scaleY = (_10*_10)+(_11*_11)+(_12*_12);
	gFloat scaleZ = (_20*_20)+(_21*_21)+(_22*_22);

	// Separate the rotation matrix from the scale matrix (multiply by s^-1)
	gFloat rot[16] = {
		_00/scaleX,	_10/scaleY,	_20/scaleZ,	0,
		_01/scaleX,	_11/scaleY,	_21/scaleZ,	0,
		_02/scaleX,	_12/scaleY,	_22/scaleZ,	0,
		0,			0,			0,			1
	};
	
	return Matrix(rot);
}

/*	Return Inverse of this 4x4 Matrix: http://stackoverflow.com/questions/30536920/how-do-i-invert-an-affine-transformation-with-translation-rotation-and-scaling
	http://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform
	https://en.wikipedia.org/wiki/Affine_transformation#Properties

	For Matrix M that represents a Rotation R and Translation T, then
	.  .  .  0                   .  .  .  0
	.  R  .  0	 then M^-1 is  	 . R^t .  0  where V = T*R^t
	.  .  .  0                   .  .  .  0
	Tx Ty Tz 1                   Vx Vy Vz 0

	For a Matrix M composed of a Rotation Matrix 'R', Scale Matrix 'S' and Translation 'V'
	[ S*R  0 ]   then the inverse =  [     (S^-1*R)^T     0 ]
	[  V   1 ]                       [ (-(S^-1*R)^T * V)  1 ]
*/
// ASSUMING NO SHEARING/REFLECTION/ETC! ONLY ROTATION & SCALE & TRANSLATION
Matrix Matrix::Inverse4() const
{
	// Decompose/Remove Scale matrix (scale component is equal to length of vector component)
	gFloat scale[16] = {
		Sqrt((_00*_00)+(_01*_01)+(_02*_02)), 0, 0, 0,
		0, Sqrt((_10*_10)+(_11*_11)+(_12*_12)), 0, 0,
		0, 0, Sqrt((_20*_20)+(_21*_21)+(_22*_22)), 0,
		0, 0, 0, 1
	};
	Matrix s(scale);
	// To invert a scale matrix, invert its diagonal
	s._00 = (gFloat)1.0 / s._00;
	s._11 = (gFloat)1.0 / s._11;
	s._22 = (gFloat)1.0 / s._22;
	// s is now s^-1

	// Separate the rotation matrix from the scale matrix (multiply by s^-1)
	gFloat rot[16] = {
		_00*s._00,	_01*s._00,	_02*s._00,	0,
		_10*s._11,	_11*s._11,	_12*s._11,	0,
		_20*s._22,	_21*s._22,	_22*s._22,	0,
		0,			0,			0,			1
	};
	Matrix r(rot);

	// Invert translation vector V
	Matrix inverse = (s * r).Transpose();
	Vector v(_30, _31, _32);	// Get V from original matrix
	v *= inverse * (gFloat)-1.0;

	// Put inverted translation into inversed matrix
	inverse._30 = v.x;
	inverse._31 = v.y;
	inverse._32 = v.z;

	// Inversion complete. Return.
	return inverse;
}

void Matrix::InvertInPlace3()
{
	// Decompose/Remove Scale components (length squared of vector component)
	gFloat scaleX = (_00*_00)+(_01*_01)+(_02*_02);
	gFloat scaleY = (_10*_10)+(_11*_11)+(_12*_12);
	gFloat scaleZ = (_20*_20)+(_21*_21)+(_22*_22);

	// Separate the rotation matrix from the scale matrix (multiply by s^-1)
	gFloat rot[16] = {
		_00/scaleX,	_10/scaleY,	_20/scaleZ,	0,
		_01/scaleX,	_11/scaleY,	_21/scaleZ,	0,
		_02/scaleX,	_12/scaleY,	_22/scaleZ,	0,
		0,			0,			0,			1
	};
	
	*this = Matrix(rot);
}

void Matrix::InvertInPlace4()
{
	// Decompose/Remove Scale matrix (scale component is equal to length of vector component)
	gFloat scale[16] = {
		Sqrt((_00*_00)+(_01*_01)+(_02*_02)), 0, 0, 0,
		0, Sqrt((_10*_10)+(_11*_11)+(_12*_12)), 0, 0,
		0, 0, Sqrt((_20*_20)+(_21*_21)+(_22*_22)), 0,
		0, 0, 0, 1
	};
	Matrix s(scale);
	// To invert a scale matrix, invert its diagonal
	s._00 = (gFloat)1.0 / s._00;
	s._11 = (gFloat)1.0 / s._11;
	s._22 = (gFloat)1.0 / s._22;
	// s is now s^-1

	// Separate the rotation matrix from the scale matrix (multiply by s^-1)
	gFloat rot[16] = {
		_00*s._00,	_01*s._00,	_02*s._00,	0,
		_10*s._11,	_11*s._11,	_12*s._11,	0,
		_20*s._22,	_21*s._22,	_22*s._22,	0,
		0,			0,			0,			1
	};
	Matrix r(rot);

	// Invert translation vector V
	Matrix inverse = (s * r).Transpose();
	Vector v(_30, _31, _32);	// Get V from original matrix
	v *= inverse * (gFloat)-1.0;

	// Put inverted translation into inversed matrix
	inverse._30 = v.x;
	inverse._31 = v.y;
	inverse._32 = v.z;

	// Inversion complete. Set this Matrix to its inverse
	*this = inverse;
}

// Return the Transpose of this Matrix
Matrix Matrix::Transpose() const
{
	gFloat newMat[16] = {
		_00,	_10,	_20,	_30,
		_01,	_11,	_21,	_31,
		_02,	_12,	_22,	_32,
		_03,	_13,	_23,	_33
	};
	return Matrix(newMat);
}

// Set this Matrix to its Transpose
Matrix& Matrix::TransposeInPlace()
{
	gFloat temp = _01;
				_01 = _10; _10 = temp;
	temp = _02; _02 = _20; _20 = temp;
	temp = _03; _03 = _30; _30 = temp;
	temp = _12; _12 = _21; _21 = temp;
	temp = _13; _13 = _31; _31 = temp;
	temp = _23; _23 = _32; _32 = temp;
	return *this;
}

// Return an array of the Euler Angles represented by this Matrix
// There are 2 possible ways to represent any orientation with Euler Angles. Set last parameter to true to get the 2nd
void Matrix::EulerAngles(gFloat& phi, gFloat& theta, gFloat& psi, bool solution2/*=false*/) const
{
#ifdef LEFT_HANDED_COORDS
	// Limit at theta = +/- 90 (cos(theta)=0 and sin(theta)=1). Stop that here.
	if(_20 != 1 && _20 != -1)
	{
		// asin(sin(theta)) = theta
		theta = ASin(_20);
		if(!solution2)
		{
			gFloat cosTheta = Cos(theta);

			// atan2f(cos(theta)sin(psi), -(-cos(theta)cos(psi))) = atan2f(sin(psi), cos(psi)) = atan2f(tan(psi)) = psi
			// divide by cos(theta) to handle whether cos(theta) is positive or negative
			psi = ATan2(-_10/cosTheta, _00/cosTheta);

			// atan2f( -(-sin(phi)cos(theta)), cos(phi)cos(theta)) = atan2f( sin(phi), cos(phi)) = atan2f(tan(phi)) = phi
			// divide by cos(theta) to handle whether cos(theta) is positive or negative
			phi = ATan2(-_21/cosTheta, _22/cosTheta);
		}
		else
		{
			// 2nd solution, sin(theta) = sin(PI-theta)
			theta = PI - theta;
			gFloat cosTheta = Cos(theta);

			// atan2f(cos(theta)sin(psi), -(-cos(theta)cos(psi))) = atan2f(sin(psi), cos(psi)) = atan2f(tan(psi)) = psi
			// divide by cos(theta) to handle whether cos(theta) is positive or negative
			psi = ATan2(_10/cosTheta, _00/cosTheta);

			// atan2f( -(-sin(phi)cos(theta)), cos(phi)cos(theta)) = atan2f( sin(phi), cos(phi)) = atan2f(tan(phi)) = phi
			// divide by cos(theta) to handle whether cos(theta) is positive or negative
			phi = ATan2(_21/cosTheta, _22/cosTheta);
		}
	}
	else
	{
		// Degree of freedom has been lost due to Gimbal Lock -> Can be anything
		// Use 0 for simplicity
		psi = 0;

		if(_20 == 1)
		{
			// sin(PI/2) = 1 -> theta = PI/2
			theta = PI / (gFloat)2.0;

			// sin(phi)sin(PI/2)cos(psi)+cos(phi)sin(psi) -> sin(phi)cos(psi)+cos(phi)sin(psi)
			// -(sin(phi)sin(psi)-cos(phi)sin(PI/2)cos(psi)) -> -(sin(phi)sin(psi)-cos(phi)cos(psi)) -> (cos(phi)cos(psi)-sin(phi)sin(psi))
			// atan2f(sin(phi)cos(psi)+cos(phi)sin(psi), cos(phi)cos(psi)-sin(phi)sin(psi)) = atan2f(sin(phi+psi), cos(phi+psi))
			// psi is set to 0 -> atan2f(sin(phi), cos(phi)) = atan2f(tan(phi)) = phi
			phi = ATan2(_01, -_02);
		}
		else
		{
			// sin(-PI/2) = -1 -> theta = -PI/2
			theta = -(PI / (gFloat)2.0);

			// sin(phi)sin(-PI/2)cos(psi)+cos(phi)sin(psi) -> -(-sin(phi)cos(psi)+cos(phi)sin(psi)) -> sin(phi)cos(psi)-cos(phi)sin(psi)
			// sin(phi)sin(psi)-cos(phi)sin(-PI/2)cos(psi) -> sin(phi)sin(psi)+cos(phi)cos(psi)
			// atan2f(sin(phi)cos(psi)-cos(phi)sin(psi), sin(phi)sin(psi)+cos(phi)cos(psi)) = atan2f(sin(phi-psi), cos(phi-psi)) -> 
			// psi is set to 0 -> atan2f(sin(phi), cos(phi)) = atan2f(tan(phi)) = phi
			phi = ATan2(-_01, _02);
		}
	}
#else
	// Limit at theta = +/- 90 (cos(theta)=0 and sin(theta)=1). Stop that here.
	if(_20 != 1 && _20 != -1)
	{
		// -asin(-sin(theta)) = -(-theta) = theta
		theta = -ASin(_20);
		if(!solution2)
		{
			gFloat cosTheta = Cos(theta);

			// atan2f(cos(theta)sin(psi), cos(theta)cos(psi)) = atan2f(sin(psi), cos(psi)) = atan2f(tan(psi)) = psi
			// divide by cos(theta) to handle whether cos(theta) is positive or negative
			psi = ATan2(_10/cosTheta, _00/cosTheta);

			// atan2f( sin(phi)cos(theta), cos(phi)cos(theta)) = atan2f( sin(phi), cos(phi)) = atan2f(tan(phi)) = phi
			// divide by cos(theta) to handle whether cos(theta) is positive or negative
			phi = ATan2(_21/cosTheta, _22/cosTheta);
		}
		else
		{
			// 2nd solution, sin(theta) = sin(PI-theta)
			theta = PI - theta;
			gFloat cosTheta = Cos(theta);

			// atan2f(cos(theta)sin(psi), cos(theta)cos(psi)) = atan2f(sin(psi), cos(psi)) = atan2f(tan(psi)) = psi
			// divide by cos(theta) to handle whether cos(theta) is positive or negative
			psi = ATan2(_10/cosTheta, _00/cosTheta);

			// atan2f( sin(phi)cos(theta), cos(phi)cos(theta)) = atan2f( sin(phi), cos(phi)) = atan2f(tan(phi)) = phi
			// divide by cos(theta) to handle whether cos(theta) is positive or negative
			phi = ATan2(_21/cosTheta, _22/cosTheta);
		}
	}
	else
	{
		// Degree of freedom has been lost due to Gimbal Lock -> Can be anything
		// Use 0 for simplicity
		psi = 0;

		if(_20 == -1)
		{
			// -sin(PI/2) = -1 -> theta = PI/2
			theta = PI / (gFloat)2.0;

			// sin(phi)sin(PI/2)cos(psi)-cos(phi)sin(psi) -> sin(phi)cos(psi)-cos(phi)sin(psi)
			// cos(phi)sin(PI/2)cos(psi)+sin(phi)sin(psi) -> cos(phi)cos(psi)+sin(phi)sin(psi)
			// atan2f(sin(phi)cos(psi)-cos(phi)sin(psi), cos(phi)cos(psi)+sin(phi)sin(psi)) = atan2f(sin(phi-psi), cos(phi-psi))
			// psi is set to 0 -> atan2f(sin(phi), cos(phi)) = atan2f(tan(phi)) = phi
			phi = ATan2(_01, _02);
		}
		else
		{
			// -sin(-PI/2) = 1 -> theta = -PI/2
			theta = -(PI / (gFloat)2.0);

			// sin(phi)sin(-PI/2)cos(psi)-cos(phi)sin(psi) -> -sin(phi)cos(psi)-cos(phi)sin(psi)
			// cos(phi)sin(-PI/2)cos(psi)+sin(phi)sin(psi) -> -cos(phi)cos(psi)+sin(phi)sin(psi)
			// atan2f(-(-sin(phi)cos(psi)-cos(phi)sin(psi)), -(-cos(phi)cos(psi)+sin(phi)sin(psi))) = atan2f(sin(phi)cos(psi)+cos(phi)sin(psi), cos(phi)cos(psi)-sin(phi)sin(psi)) =
			// atan2f(sin(phi+psi), cos(phi+psi)) -> psi is set to 0 -> atan2f(sin(phi), cos(phi)) = atan2f(tan(phi)) = phi
			phi = ATan2(-_01, -_02);
		}
	}
#endif
}

// Separate the Scaling, Rotational, and Translational components of a Transformation Matrix
void Matrix::Decompose(Vector& scale, Quaternion& rot, Vector& trans)
{
	// Compute Scale component
	scale.x = Sqrt(_00*_00 + _01*_01 + _02*_02);
	scale.y = Sqrt(_10*_10 + _11*_11 + _12*_12);
	scale.z = Sqrt(_20*_20 + _21*_21 + _22*_22);

	// Compute Translation component
	if(!is3x3)
	{
		trans.x = _30;
		trans.y = _31;
		trans.z = _32;
	}

	// Compute Rotation component
	gFloat temp[16] = {
		_00/scale.x,	_01/scale.x,	_02/scale.x,	0,
		_10/scale.y,	_11/scale.y,	_12/scale.y,	0,
		_20/scale.z,	_21/scale.z,	_22/scale.z,	0,
		0,				0,				0,				1
	};
	Matrix m(temp);
	rot = m.ConvertToQuaternion();
}

// Create a Transformation Matrix from the 3 main Affine Transformations
void Matrix::ComposeTransformationMatrix(Vector* trans, Quaternion* q, Vector* scale)
{
	Identity();
	if(q != nullptr)
	{
		gFloat scaleX=1.0f, scaleY=1.0f, scaleZ=1.0f;
		if(scale != nullptr)
		{
			scaleX = scale->x;
			scaleY = scale->y;
			scaleZ = scale->z;
		}

#ifdef LEFT_HANDED_COORDS
		_00 = (1.0f - (2.0f * (q->y*q->y + q->z*q->z))) * scaleX;	_01 = (2.0f * (q->x*q->y + q->z*q->w)) * scaleX;			_02 = (2.0f * (q->x*q->z - q->y*q->w)) * scaleX;
		_10 = (2.0f * (q->x*q->y - q->z*q->w)) * scaleY;			_11 = (1.0f - (2.0f * (q->x*q->x + q->z*q->z))) * scaleY;	_12 = (2.0f * (q->y*q->z + q->x*q->w)) * scaleY;
		_20 = (2.0f * (q->x*q->z + q->y*q->w)) * scaleZ;			_21 = (2.0f * (q->y*q->z - q->x*q->w)) * scaleZ;			_22 = (1.0f - (2.0f * (q->x*q->x + q->y*q->y))) * scaleZ;
#else
		_00 = (1.0f - (2.0f * (q->y*q->y - q->z*q->z))) * scaleX;	_01 = (2.0f * (q->x*q->y - q->z*q->w)) * scaleX;			_02 = (2.0f * (q->x*q->z + q->y*q->w)) * scaleX;
		_10 = (2.0f * (q->x*q->y + q->z*q->w)) * scaleY;			_11 = (1.0f - (2.0f * (q->x*q->x - q->z*q->z))) * scaleY;	_12 = (2.0f * (q->y*q->z - q->x*q->w)) * scaleY;
		_20 = (2.0f * (q->x*q->z - q->y*q->w)) * scaleZ;			_21 = (2.0f * (q->y*q->z + q->x*q->w)) * scaleZ;			_22 = (1.0f - (2.0f * (q->x*q->x - q->y*q->y))) * scaleZ;
#endif
	}
	else if(scale != nullptr)
	{
		_00 = scale->x;
		_11 = scale->y;
		_22 = scale->z;
	}
	else
		_00 = _11 = _22 = 1.0f;

	if(trans != nullptr)
	{
		_30 = trans->x;
		_31 = trans->y;
		_32 = trans->z;
	}
	if(is3x3) { _30 = _31 = _32 = 0.0f; _33 = 1.0f; }
}

void Matrix::SetBasis(Vector x, Vector y, Vector z)
{
	_00 = x.x;	_01 = x.y;	_02 = x.z;	_03 = 0.0f;
	_10 = y.x;	_11 = y.y;	_12 = y.z;	_13 = 0.0f;
	_20 = z.x;	_21 = z.y;	_22 = z.z;	_23 = 0.0f;
	_30 = 0.0f;	_31 = 0.0f;	_32 = 0.0f;	_33 = 1.0f;
	is3x3 = true;
}

// Calculate and return Quaternion representation of this rotation Matrix
Quaternion Matrix::ConvertToQuaternion() const
{
	gFloat tr = _00 + _11 + _22;
	gFloat w, x, y, z;

	if(tr > 0)
	{
		gFloat s = Sqrt(tr+1.0f) * 2;
		w = 0.25f * s;
#ifdef LEFT_HANDED_COORDS
		x = (_12 - _21) / s;
		y = (_20 - _02) / s;
		z = (_01 - _10) / s;
#else
		x = (_21 - _12) / s;
		y = (_02 - _20) / s;
		z = (_10 - _01) / s;
#endif
	}
	else if((_00 >_11) && (_00 > _22))
	{
		gFloat s = Sqrt(1.0f + _00 - _11 - _22) * 2.0f;
		x = 0.25f * s;
#ifdef LEFT_HANDED_COORDS
		w = (_12 - _21) / s;
		y = (_10 + _01) / s;
		z = (_20 + _02) / s;
#else
		w = (_21 - _12) / s;
		y = (_01 + _10) / s;
		z = (_02 + _20) / s;
#endif
	}
	else if(_11 > _22)
	{
		gFloat s = Sqrt(1.0f + _11 - _00 - _22) * 2;
		y = 0.25f * s;
#ifdef LEFT_HANDED_COORDS
		w = (_20 - _02) / s;
		x = (_10 + _01) / s;
		z = (_21 + _12) / s;
#else
		w = (_02 - _20) / s;
		x = (_01 + _10) / s;
		z = (_12 + _21) / s;
#endif
	}
	else
	{
		gFloat s = Sqrt(1.0f + _22 - _00 - _11) * 2;
		z = 0.25f * s;
#ifdef LEFT_HANDED_COORDS
		w = (_01 - _10) / s;
		x = (_20 + _02) / s;
		y = (_21 + _12) / s;
#else
		w = (_10 - _01) / s;
		x = (_02 + _20) / s;
		y = (_12 + _21) / s;
#endif
	}

	return Quaternion(x, y, z, w);
}
Quaternion Matrix::ConvertToQuaternion2() const
{
	Quaternion q(MAX(0, 1+_00-_11-_22),
				MAX(0, 1-_00+_11-_22),
				MAX(0, 1-_00-_11+_22),
				MAX(0, 1+_00+_11+_22));

	q.x = copysign(q.x, _21-_12);
	q.y = copysign(q.y, _02-_20);
	q.z = copysign(q.z, _10-_01);
	return q;
}

Matrix Matrix::GetMatrix3() const
{
	gFloat mat[16] = {
		_00,	_01,	_02,	0.0,
		_10,	_11,	_12,	0.0,
		_20,	_21,	_22,	0.0,
		0.0,	0.0,	0.0,	1.0
	};
	return Matrix(mat);
}

// Return result of this * mat, but ignoring the 4th row/column
Matrix Matrix::Multiply3(const Matrix& mat) const
{
	gFloat temp[16] = {
		_00*mat._00 + _01*mat._10 + _02*mat._20,
		_00*mat._01 + _01*mat._11 + _02*mat._21,
		_00*mat._02 + _01*mat._12 + _02*mat._22,
		0.0f,		
		_10*mat._00 + _11*mat._10 + _12*mat._20,
		_10*mat._01 + _11*mat._11 + _12*mat._21,
		_10*mat._02 + _11*mat._12 + _12*mat._22,
		0.0f,
		_20*mat._00 + _21*mat._10 + _22*mat._20,
		_20*mat._01 + _21*mat._11 + _22*mat._21,
		_20*mat._02 + _21*mat._12 + _22*mat._22,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};
	return Matrix(temp);
}

Vector Matrix::Multiply3(const Vector& v) const
{
	gFloat x,y,z;
	x = _00*v.x + _10*v.y + _20*v.z;
	y = _01*v.x + _11*v.y + _21*v.z;
	z = _02*v.x + _12*v.y + _22*v.z;
	return Vector(x,y,z);
}

// Return result of this->Inverse3() * mat
Matrix Matrix::MultiplyInverse3(const Matrix& mat) const
{
	gFloat temp[16] = {
		_00*mat._00 + _10*mat._10 + _20*mat._20,
		_00*mat._01 + _10*mat._11 + _20*mat._21,
		_00*mat._02 + _10*mat._12 + _20*mat._22,
		0.0f,		
		_01*mat._00 + _11*mat._10 + _21*mat._20,
		_01*mat._01 + _11*mat._11 + _21*mat._21,
		_01*mat._02 + _11*mat._12 + _21*mat._22,
		0.0f,
		_02*mat._00 + _12*mat._10 + _22*mat._20,
		_02*mat._01 + _12*mat._11 + _22*mat._21,
		_02*mat._02 + _12*mat._12 + _22*mat._22,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};
	return Matrix(temp);
}

Vector Matrix::MultiplyInverse3(const Vector& v) const
{
	gFloat x,y,z;
	x = _00*v.x + _01*v.y + _02*v.z;
	y = _10*v.x + _11*v.y + _12*v.z;
	z = _20*v.x + _21*v.y + _22*v.z;
	return Vector(x,y,z);
}

void Matrix::SetIs3x3(bool _3x3)
{
	is3x3 = _3x3;
}
#pragma endregion

#pragma region Transformations

// Translate this Matrix by Vector v of components [x, y, z]
void Matrix::Translate(const Vector& v)
{
	_30 += v.x;
	_31 += v.y;
	_32 += v.z;
}
void Matrix::Translate(gFloat x, gFloat y, gFloat z)
{
	_30 += x;
	_31 += y;
	_32 += z;
}

// Equally scale this Matrix along all axes
void Matrix::Scale(gFloat scale)
{
	Scale(scale, scale, scale);
}

// Scale this Matrix along each axis proportional to sx, sy and sz
void Matrix::Scale(gFloat sx, gFloat sy, gFloat sz)
{
	// Create Scale Matrix
	Matrix scale;
	scale._00 = sx;
	scale._11 = sy;
	scale._22 = sz;

	// Mutliply onto this matrix
	*this = scale * *this;
}

// Rotate this matrix by 'angle' radians along 'axis' from origin
void Matrix::Rotate(gFloat angle, const Vector& axis)
{
	 Rotate(angle, axis.x, axis.y, axis.z);
}

// Rotate this matrix by 'angle' radians along vector [xyz] from origin
void Matrix::Rotate(gFloat angle, gFloat x, gFloat y, gFloat z)
{
	// Create matrix for rotation
	Matrix mR;

	// Normalize vector [xyz] for safety
	gFloat mag = Sqrt(x*x + y*y + z*z);
	x /= mag;	y /= mag;	z /= mag;

	// Calculate the rotation matrix
	gFloat cos_ = Cos(angle);
	gFloat sin_ = Sin(angle);
	gFloat _1MinCos = (gFloat)1.0f - cos_;

#ifdef LEFT_HANDED_COORDS
	mR._00 = cos_+(x*x*_1MinCos);	mR._01 = x*y*_1MinCos+(z*sin_);	mR._02 = x*z*_1MinCos-(y*sin_);
	mR._10 = y*x*_1MinCos-(z*sin_);	mR._11 = cos_+(y*y*_1MinCos);	mR._12 = y*z*_1MinCos+(x*sin_);
	mR._20 = z*x*_1MinCos+(y*sin_);	mR._21 = z*y*_1MinCos-(x*sin_);	mR._22 = cos_+(z*z*_1MinCos);
#else
	mR._00 = cos_+(x*x*_1MinCos);	mR._01 = x*y*_1MinCos-(z*sin_);	mR._02 = x*z*_1MinCos+(y*sin_);
	mR._10 = y*x*_1MinCos+(z*sin_);	mR._11 = cos_+(y*y*_1MinCos);	mR._12 = y*z*_1MinCos-(x*sin_);
	mR._20 = z*x*_1MinCos-(y*sin_);	mR._21 = z*y*_1MinCos+(x*sin_);	mR._22 = cos_+(z*z*_1MinCos);
#endif

	// Multiply onto this matrix
	*this = mR * *this;
}

// Rotate by 'angle' radians along X-Axis
void Matrix::RotateX(gFloat angle)
{
	// Create X-Axis rotation matrix
	Matrix mX;
#ifdef LEFT_HANDED_COORDS
	mX._11 = Cos(angle);
	mX._12 = Sin(angle);
	mX._21 = -Sin(angle);
	mX._22 = Cos(angle);
#else
	mX._11 = Cos(angle);
	mX._12 = -Sin(angle);
	mX._21 = Sin(angle);
	mX._22 = Cos(angle);
#endif

	// Multiply onto this matrix
	*this = mX * *this;
}

// Rotate by 'angle' radians along Y-Axis
void Matrix::RotateY(gFloat angle)
{
	// Create Y-Axis rotation matrix
	Matrix mY;
#ifdef LEFT_HANDED_COORDS
	mY._00 = Cos(angle);
	mY._02 = -Sin(angle);
	mY._20 = Sin(angle);
	mY._22 = Cos(angle);
#else
	mY._00 = Cos(angle);
	mY._02 = Sin(angle);
	mY._20 = -Sin(angle);
	mY._22 = Cos(angle);
#endif

	// Multiply onto this matrix
	*this = mY * *this;
}

// Rotate by 'angle' radians along Z-Axis
void Matrix::RotateZ(gFloat angle)
{
	// Create Z-Axis rotation matrix
	Matrix mZ;
#ifdef LEFT_HANDED_COORDS
	mZ._00 = Cos(angle);
	mZ._01 = Sin(angle);
	mZ._10 = -Sin(angle);
	mZ._11 = Cos(angle);
#else
	mZ._00 = Cos(angle);
	mZ._01 = -Sin(angle);
	mZ._10 = Sin(angle);
	mZ._11 = Cos(angle);
#endif

	// Multiply onto this matrix
	*this = mZ * *this;
}

void Matrix::ShearXY(gFloat shx, gFloat shy)
{
	Matrix m;
	m._02 = shx;
	m._12 = shy;
	*this = m * *this;
}

void Matrix::ShearYZ(gFloat shy, gFloat shz)
{
	Matrix m;
	m._10 = shy;
	m._20 = shz;
	*this = m * *this;
}

void Matrix::ShearZX(gFloat shx, gFloat shz)
{
	Matrix m;
	m._01 = shx;
	m._21 = shz;
	*this = m * *this;
}
#pragma endregion

Matrix Matrix::SkewSymmetricMatrix(const Vector& v)
{
	gFloat m[16] = {
		0.0f, -v.z, v.y, 0.0f, 
		v.z, 0.0f, -v.x, 0.0f, 
		-v.y, v.x, 0.0f, 0.0f, 
		0.0f, 0.0f, 0.0f, 0.0f
	};
	return Matrix(m);
}

// Return Matrix representation of 'angle' degrees rotation along X-Axis
Matrix Matrix::MatrixFromXAxisRotation(gFloat angle)
{
	// Create X-Axis rotation matrix
#ifdef LEFT_HANDED_COORDS
	gFloat mX[16] = {
		1.0,		0.0,		0.0,			0.0,
		0.0,		Cos(angle),	Sin(angle),		0.0,
		0.0,		-Sin(angle),Cos(angle),		0.0,
		0.0,		0.0,		0.0,			1.0
	};
#else
	gFloat mX[16] = {
		1.0,		0.0,		0.0,			0.0,
		0.0,		Cos(angle),	-Sin(angle),	0.0,
		0.0,		Sin(angle),	Cos(angle),		0.0,
		0.0,		0.0,		0.0,			1.0
	};
#endif
	return Matrix(mX);
}

// Return Matrix representation of 'angle' radians rotation along Y-Axis
Matrix Matrix::MatrixFromYAxisRotation(gFloat angle)
{
	// Create Y-Axis rotation matrix
#ifdef LEFT_HANDED_COORDS
	gFloat mY[16] = {
		Cos(angle),		0.0,		-Sin(angle),	0.0,
		0.0,			1.0,		0.0,			0.0,
		Sin(angle),		0.0,		Cos(angle),		0.0,
		0.0,			0.0,		0.0,			1.0
	};
#else
	gFloat mY[16] = {
		Cos(angle),		0.0,		Sin(angle),		0.0,
		0.0,			1.0,		0.0,			0.0,
		-Sin(angle),	0.0,		Cos(angle),		0.0,
		0.0,			0.0,		0.0,			1.0
	};
#endif
	return Matrix(mY);
}

// Return Matrix representation of 'angle' radians rotation along Z-Axis
Matrix Matrix::MatrixFromZAxisRotation(gFloat angle)
{
	// Create Z-Axis rotation matrix
#ifdef LEFT_HANDED_COORDS
	gFloat mZ[16] = {
		Cos(angle),	Sin(angle),		0.0,	0.0,
		-Sin(angle),Cos(angle),		0.0,	0.0,
		0.0,		0.0,			0.0,	0.0,
		0.0,		0.0,			0.0,	1.0
	};
#else
	gFloat mZ[16] = {
		Cos(angle),	-Sin(angle),	0.0,	0.0,
		Sin(angle),	Cos(angle),		0.0,	0.0,
		0.0,		0.0,			0.0,	0.0,
		0.0,		0.0,			0.0,	1.0
	};
#endif
	return Matrix(mZ);
}

// Return Matrix representation of 'angle' radians rotation along Axis [xyz]
Matrix Matrix::MatrixFromAxisAngle(gFloat angle, const Vector& vec)
{
	return MatrixFromAxisAngle(angle, vec.x, vec.y, vec.z);
}
Matrix Matrix::MatrixFromAxisAngle(gFloat angle, gFloat x, gFloat y, gFloat z)
{
	// Create matrix for rotation
	gFloat mR[4][4] = {0.0};
	mR[3][3] = 1.0f;
	
	// Normalize vector [xyz] for safety
	gFloat mag = Sqrt(x*x + y*y + z*z);
	x /= mag;	y /= mag;	z /= mag;

	// Calculate the rotation matrix
	gFloat cos_ = Cos(angle);
	gFloat sin_ = Sin(angle);
	gFloat _1MinCos = (gFloat)1.0f - cos_;

#ifdef LEFT_HANDED_COORDS
	mR[0][0] = cos_+(x*x*_1MinCos);		mR[0][1] = x*y*_1MinCos+(z*sin_);	mR[0][2] = x*z*_1MinCos-(y*sin_);
	mR[1][0] = y*x*_1MinCos-(z*sin_);	mR[1][1] = cos_+(y*y*_1MinCos);		mR[1][2] = y*z*_1MinCos+(x*sin_);
	mR[2][0] = z*x*_1MinCos+(y*sin_);	mR[2][1] = z*y*_1MinCos-(x*sin_);	mR[2][2] = cos_+(z*z*_1MinCos);
#else
	mR[0][0] = cos_+(x*x*_1MinCos);		mR[0][1] = x*y*_1MinCos-(z*sin_);	mR[0][2] = x*z*_1MinCos+(y*sin_);
	mR[1][0] = y*x*_1MinCos+(z*sin_);	mR[1][1] = cos_+(y*y*_1MinCos);		mR[1][2] = y*z*_1MinCos-(x*sin_);
	mR[2][0] = z*x*_1MinCos-(y*sin_);	mR[2][1] = z*y*_1MinCos+(x*sin_);	mR[2][2] = cos_+(z*z*_1MinCos);
#endif
	return Matrix(mR);
}

// Returns a View Matrix at position 'eye' rotated towards 'target' in space
Matrix Matrix::LookAt(const Vector& eye, const Vector& target, const Vector& up)
{
	Vector zAxis = (target - eye).Normalized();
	Vector xAxis = up.CrossProduct(zAxis).Normalized();
	Vector yAxis = zAxis.CrossProduct(xAxis);

#ifdef LEFT_HANDED_COORDS
	gFloat mat[16] = {
		xAxis.x,				yAxis.x,				zAxis.x,				0.0, 
		xAxis.y,				yAxis.y,				zAxis.y,				0.0, 
		xAxis.z,				yAxis.z,				zAxis.z,				0.0,
		-xAxis.DotProduct(eye), -yAxis.DotProduct(eye), -zAxis.DotProduct(eye), 1.0
	};
#else
	gFloat mat[16] = {
		-xAxis.x,				yAxis.x,				-zAxis.x,				0.0,
		-xAxis.y,				yAxis.y,				-zAxis.y,				0.0,
		-xAxis.z,				yAxis.z,				-zAxis.z,				0.0,
		xAxis.DotProduct(eye),	-yAxis.DotProduct(eye),	zAxis.DotProduct(eye),	1.0
	};
#endif
	return Matrix(mat);
}

// Returns a Perspective Projection Matrix
Matrix Matrix::Perspective(gFloat fieldOfView, gFloat aspect, gFloat zNear, gFloat zFar)
{
	gFloat tan_ = Tan(fieldOfView / 2.0);
#ifdef LEFT_HANDED_COORDS
	gFloat mat[16] = {
		1.0/(aspect*tan_),	0.0,		0.0,							0.0, 
		0.0,				1.0/tan_,	0.0,							0.0, 
		0.0,				0.0,		zFar/(zFar-zNear),				1.0, 
		0.0,				0.0,		(zFar*zNear)/(zNear-zFar),		0.0
	};
#else
	gFloat mat[16] = {
		1.0/(aspect*tan_),	0.0,		0.0,							0.0, 
		0.0,				1.0/tan_,	0.0,							0.0, 
		0.0,				0.0,		zFar/(zNear-zFar),				-1.0, 
		0.0,				0.0,		(zFar*zNear)/(zNear-zFar),		0.0
	};
#endif
	return Matrix(mat);
}

// Returns an Orthographic Projection Matrix
Matrix Matrix::Ortho(gFloat width, gFloat height, gFloat zNear, gFloat zFar)
{
#ifdef LEFT_HANDED_COORDS
	gFloat mat[16] = {
		2.0/width,		0.0,			0.0,					0.0, 
		0.0,			2.0/height,		0.0,					0.0, 
		0.0,			0.0,			1.0/(zFar-zNear),		0.0,
		0.0,			0.0,			zNear/(zNear-zFar),		1.0
	};
#else
	gFloat mat[16] = {
		2.0/width,		0.0,			0.0,					0.0,
		0.0,			2.0/height,		0.0,					0.0,
		0.0,			0.0,			1.0/(zNear-zFar),		0.0,
		0.0,			0.0,			zNear/(zNear-zFar),		1.0
	};
#endif
	return Matrix(mat);
}

#pragma region Inverse Inertia Tensors
Matrix Matrix::CuboidInverseInertiaTensor(gFloat mass, Vector halfWidths)
{
	gFloat x2 = halfWidths.x * halfWidths.x;
	gFloat y2 = halfWidths.y * halfWidths.y;
	gFloat z2 = halfWidths.z * halfWidths.z;
	gFloat inertia[16] = {
		(mass*(y2+z2))/12.0f,	0.0f,					0.0f,					0.0f,
		0.0f,					(mass*(x2+z2))/12.0f,	0.0f,					0.0f,
		0.0f,					0.0f,					(mass*(x2+y2))/12.0f,	0.0f,
		0.0f,					0.0f,					0.0f,					1.0f		
	};
	return Matrix(inertia).Inverse3();
}

Matrix Matrix::SphereInverseInertiaTensor(gFloat mass, gFloat radius)
{
	gFloat s = mass * radius * radius * (2.0f / 5.0f);
	gFloat inertia[16] = {
		s,		0.0f,	0.0f,	0.0f,
		0.0f,	s,		0.0f,	0.0f,
		0.0f,	0.0f,	s,		0.0f,
		0.0f,	0.0f,	0.0f,	1.0f
	};
	return Matrix(inertia).Inverse3();
}

Matrix Matrix::HollowSphereInverseInertiaTensor(gFloat mass, gFloat radius)
{
	gFloat s = mass * radius * radius * (2.0f / 3.0f);
	gFloat inertia[16] = {
		s,		0.0f,	0.0f,	0.0f,
		0.0f,	s,		0.0f,	0.0f,
		0.0f,	0.0f,	s,		0.0f,
		0.0f,	0.0f,	0.0f,	1.0f
	};
	return Matrix(inertia).Inverse3();
}

Matrix Matrix::CylinderInverseInertiaTensor(gFloat mass, gFloat height, gFloat radius)
{
	gFloat mh2 = mass * height * height;
	gFloat mr2 = mass * radius * radius;
	gFloat inertia[16] = {
		mh2/12.0f + mr2/4.0f,	0.0f,		0.0f,					0.0f,
		0.0f,					mr2/2.0f,	0.0f,					0.0f,
		0.0f,					0.0f,		mh2/12.0f + mr2/4.0f,	0.0f,
		0.0f,					0.0f,		0.0f,					1.0f
	};
	return Matrix(inertia).Inverse3();
}

Matrix Matrix::CylindricalTubeInverseInertiaTensor(gFloat mass, gFloat height, gFloat oRadius, gFloat iRadius)
{
	gFloat mh2 = mass * height * height;
	gFloat mr2 = mass * (oRadius*oRadius + iRadius*iRadius);
	gFloat inertia[16] = {
		mh2/12.0f + mr2/4.0f,	0.0f,		0.0f,					0.0f,
		0.0f,					mr2/2.0f,	0.0f,					0.0f,
		0.0f,					0.0f,		mh2/12.0f + mr2/4.0f,	0.0f,
		0.0f,					0.0f,		0.0f,					1.0f
	};
	return Matrix(inertia).Inverse3();
}

Matrix Matrix::ConeInverseInertiaTensor(gFloat mass, gFloat height, gFloat radius)
{
	gFloat mh2 = mass * height * height;
	gFloat mr2 = mass * radius * radius;
	gFloat inertia[16] = {
		mh2*0.0375f + mr2*0.15f,	0.0f,		0.0f,					0.0f,
		0.0f,						mr2*0.3f,	0.0f,					0.0f,
		0.0f,						0.0f,		mh2*0.6f + mr2*0.15f,	0.0f,
		0.0f,						0.0f,		0.0f,					1.0f
	};
	return Matrix(inertia).Inverse3();
}

Matrix Matrix::CapsuleInverseInertiaTensor(gFloat mass, gFloat height, gFloat radius)
{
	gFloat r2 = radius * radius;
	gFloat h2 = height * height;
	gFloat hr = height * radius;

	// Calculate mass of Cylinder portion and each Hemisphere portion of the Capsule
	//		(derived from total mass and volume of each piece assuming uniform density)
	gFloat mc = (3.0f * mass * height) / ((3.0f*height) + (4.0f*radius));	// mass cylinder
	gFloat mh = (2.0f * mass * radius) / ((3.0f*height) + (4.0f*radius));	// mass hemisphere
	
	// Pre-calculate common terms
	gFloat xx = (mc*(h2 + 3.0f*r2))/12.0f + mh*(r2*0.8f + h2 + height*radius*0.75f);
	gFloat yy = mc*r2*0.5f + mh*r2*0.8f;
//	gFloat zz = xx;

	gFloat inertia[16] = {
		xx,		0.0f,	0.0f,	0.0f,
		0.0f,	yy,		0.0f,	0.0f,
		0.0f,	0.0f,	xx,		0.0f,
		0.0f,	0.0f,	0.0f,	1.0f
	};
	return Matrix(inertia).Inverse3();
}
#pragma endregion