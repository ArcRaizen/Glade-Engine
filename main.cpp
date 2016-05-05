class Matrix
{
public:
	union {
		struct {
			float _00, _01, _02, _03;
			float _10, _11, _12, _13;
			float _20, _21, _22, _23;
			float _30, _31, _32, _33;
		};
		float m[4][4];
	};

	Matrix() { };
	__forceinline
	operator float* ( ) { return (float*) &_00; }

	__forceinline
	operator const float* () const { return (const float*) &_00; }
};

typedef struct _D3DMATRIX {
    union {
        struct {
            float    _11, _12, _13, _14;
            float    _21, _22, _23, _24;
            float    _31, _32, _33, _34;
            float    _41, _42, _43, _44;
        };
        float m[4][4];
    };
} D3DMATRIX;

typedef struct D3DXMATRIX : public _D3DMATRIX
{
	__forceinline
	operator float* ( ) { return (float*) &_11; }

	__forceinline
	operator const float* () const { return (const float*) &_11; }
} D3DXMATRIX, *LPD3DMATRIX;

void Test(const float *a, const float *b)
{
	float aa[16], bb[16];

	for(unsigned int i = 0; i < 16; ++i)
	{
		aa[i] = a[i];
		bb[i] = b[i];
	}
	return;
}

int main()
{
	Matrix a;
	D3DXMATRIX b;

/*	a._00 = b._11 = 1;
	a._01 = b._12 = 2;
	a._02 = b._13 = 3;
	a._03 = b._14 = 4;
	a._10 = b._21 = 5;
	a._11 = b._22 = 6;
	a._12 = b._23 = 7;
	a._13 = b._24 = 8;
	a._20 = b._31 = 9;
	a._21 = b._32 = 10;
	a._22 = b._33 = 11;
	a._23 = b._34 = 12;
	a._30 = b._41 = 13;
	a._31 = b._41 = 14;
	a._32 = b._43 = 15;
	a._33 = b._44 = 16;

	Test(a, b);
	Test((float*)a, (float*)b);
	Test((float*)&a, (float*)&b);
*/
	return 0;
}