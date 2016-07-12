#ifndef GLADE_VERTEX_S
#define GLADE_VERTEX_S
#include <vector>
#include <D3dx10math.h>
#pragma comment(lib, "d3dx9.lib")

// Vertex struct for graphics drawing
// Simple definition Pos (no texture)
struct SVertexC
{
	SVertexC() { }
	SVertexC(const D3DXVECTOR3& p) :
		position(p) { }
	SVertexC(
		float px, float py, float pz)
		: position(px,py,pz) { }

	D3DXVECTOR3 position;
};

// Vertex struct for graphics drawing
// Simple definition - Pos, UV only
struct SVertex
{
	SVertex() { }
	SVertex(const D3DXVECTOR3& p, const D3DXVECTOR2& uv) :
		position(p), texCoord(uv) { }
	SVertex(
		float px, float py, float pz, 
		float u, float v)
		: position(px,py,pz), texCoord(u,v) { }

	D3DXVECTOR3 position;
	D3DXVECTOR2 texCoord;
};

// Vertex Struct for graphics drawing
// Full definition - Pos, Normal, Tangent (no texture)
struct VertexC
{
	VertexC() { }
	VertexC(const D3DXVECTOR3& p, const D3DXVECTOR3& n, const D3DXVECTOR3& t) :
		position(p), normal(n), tangent(t) { }
	VertexC(
		float px, float py, float pz, 
		float nx, float ny, float nz,
		float tx, float ty, float tz)
		: position(px,py,pz), normal(nx,ny,nz),
			tangent(tx, ty, tz) { }

	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
};


// Vertex struct for graphics drawing
// Full definition - Pos, Normal, Tangent, UV
struct Vertex
{
	Vertex() { }
	Vertex(const D3DXVECTOR3& p, const D3DXVECTOR3& n, const D3DXVECTOR3& t, const D3DXVECTOR2& uv) :
		position(p), normal(n), tangent(t), texCoord(uv) { }
	Vertex(
		float px, float py, float pz, 
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: position(px,py,pz), normal(nx,ny,nz),
			tangent(tx, ty, tz), texCoord(u,v) { }

	Vertex(const VertexC& vc) : 
		position(vc.position), normal(vc.normal), tangent(vc.tangent),
		texCoord(D3DXVECTOR2(-1,-1)) { }
	Vertex(const SVertex& sv) :
		position(sv.position), normal(D3DXVECTOR3(-1,-1,-1)), tangent(D3DXVECTOR3(-1,-1,-1)),
		texCoord(sv.texCoord) { }
	Vertex(const SVertexC& svc) :
		position(svc.position), normal(D3DXVECTOR3(-1,-1,-1)), tangent(D3DXVECTOR3(-1,-1,-1)),
		texCoord(D3DXVECTOR2(-1,-1)) { }

	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR2 texCoord;
};
#endif	// GLADE_VERTEX_F