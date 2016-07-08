#pragma once
#ifndef GLADE_GEOMETRY_GENERATOR_H
#define GLADE_GEOMETRY_GENERATOR_H
#include "Graphics\Vertex.h"
#include "../Math/Precision.h"

namespace Glade {
class GeometryGenerator
{
public:
	static MeshData* CreateBox(gFloat width, gFloat height, gFloat depth, std::string texFilename, D3DXVECTOR4 color=D3DXVECTOR4(1,1,1,1));
	static MeshData* CreateSphere(gFloat radius, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color=D3DXVECTOR4(1,1,1,1));
	static MeshData* CreateCylinder(gFloat radius, gFloat height, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color=D3DXVECTOR4(1,1,1,1));
	static MeshData* CreateCone(gFloat radius, gFloat height, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color=D3DXVECTOR4(1,1,1,1));
	static MeshData* CreateCapsule(gFloat radius, gFloat height, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color=D3DXVECTOR4(1,1,1,1));

	static void Subdivide(MeshData* meshData);
};
}	// namespace
#endif	// GLADE_GEOMETRY_GENERATOR_H
