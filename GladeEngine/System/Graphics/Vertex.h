#ifndef GLADE_VERTEX_S
#define GLADE_VERTEX_S
#include <vector>
#include <D3dx10math.h>
#pragma comment(lib, "d3dx9.lib")
// Vertex struct for graphics drawing
// Simple definition Pos, Color (no texture)
struct SVertexC
{
	SVertexC() { }
	SVertexC(const D3DXVECTOR3& p, const D3DXVECTOR4& c=D3DXVECTOR4(1,1,1,1)) :
		position(p), color(c) { }
	SVertexC(
		float px, float py, float pz, 
		float r=1, float g=1, float b=1, float a=1)
		: position(px,py,pz), color(r,g,b,a) { }

	D3DXVECTOR3 position;
	D3DXVECTOR4 color;
};

// Vertex struct for graphics drawing
// Simple definition - Pos, UV, Color only
struct SVertex
{
	SVertex() { }
	SVertex(const D3DXVECTOR3& p, const D3DXVECTOR2& uv, const D3DXVECTOR4& c=D3DXVECTOR4(1,1,1,1)) :
		position(p), texCoord(uv), color(c) { }
	SVertex(
		float px, float py, float pz, 
		float u, float v,
		float r=1, float g=1, float b=1, float a=1)
		: position(px,py,pz), texCoord(u,v), color(r,g,b,a) { }

	D3DXVECTOR3 position;
	D3DXVECTOR2 texCoord;
	D3DXVECTOR4 color;
};

// Vertex Struct for graphics drawing
// Full definition - Pos, Normal, Tangent, Color (no texture)
struct VertexC
{
	VertexC() { }
	VertexC(const D3DXVECTOR3& p, const D3DXVECTOR3& n, const D3DXVECTOR3& t, const D3DXVECTOR4& c=D3DXVECTOR4(1,1,1,1)) :
		position(p), normal(n), tangent(t), color(c) { }
	VertexC(
		float px, float py, float pz, 
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float r=1, float g=1, float b=1, float a=1)
		: position(px,py,pz), normal(nx,ny,nz),
			tangent(tx, ty, tz),  color(r,g,b,a) { }

	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR4 color;
};


// Vertex struct for graphics drawing
// Full definition - Pos, Normal, Tangent, UV
struct Vertex
{
	Vertex() { }
	Vertex(const D3DXVECTOR3& p, const D3DXVECTOR3& n, const D3DXVECTOR3& t, const D3DXVECTOR2& uv, const D3DXVECTOR4& c=D3DXVECTOR4(1,1,1,1)) :
		position(p), normal(n), tangent(t), texCoord(uv), color(c) { }
	Vertex(
		float px, float py, float pz, 
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v,
		float r=1, float g=1, float b=1, float a=1)
		: position(px,py,pz), normal(nx,ny,nz),
			tangent(tx, ty, tz), texCoord(u,v), color(r,g,b,a) { }

	Vertex(const VertexC& vc) : 
		position(vc.position), normal(vc.normal), tangent(vc.tangent),
		texCoord(D3DXVECTOR2(-1,-1)), color(vc.color) { }
	Vertex(const SVertex& sv) :
		position(sv.position), normal(D3DXVECTOR3(-1,-1,-1)), tangent(D3DXVECTOR3(-1,-1,-1)),
		texCoord(sv.texCoord), color(sv.color) { }
	Vertex(const SVertexC& svc) :
		position(svc.position), normal(D3DXVECTOR3(-1,-1,-1)), tangent(D3DXVECTOR3(-1,-1,-1)),
		texCoord(D3DXVECTOR2(-1,-1)), color(svc.color) { }

	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR2 texCoord;
	D3DXVECTOR4 color;
};

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};
#endif	// GLADE_VERTEX_F