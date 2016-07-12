#pragma once
#ifndef GLADE_GEOMETRY_GENERATOR_H
#define GLADE_GEOMETRY_GENERATOR_H
#include "Graphics\Vertex.h"
#include "Resource.h"
#include "../Math/Precision.h"

namespace Glade {
class MeshData : public Resource<MeshData>
{
protected:
	MeshData(std::string& name, std::vector<Vertex> verts, std::vector<unsigned int> inds, std::string& filename, D3DXVECTOR4 c);


public:
	static SmartPointer<MeshData> CreateBox(const std::string& name, bool saveSmart, gFloat width, gFloat height, gFloat depth, std::string texFilename, D3DXVECTOR4 color=D3DXVECTOR4(1,1,1,1));
	static SmartPointer<MeshData> CreateSphere(const std::string& name, bool saveSmart, gFloat radius, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color=D3DXVECTOR4(1,1,1,1));
	static SmartPointer<MeshData> CreateCylinder(const std::string& name, bool saveSmart, gFloat radius, gFloat height, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color=D3DXVECTOR4(1,1,1,1));
	static SmartPointer<MeshData> CreateCone(const std::string& name, bool saveSmart, gFloat radius, gFloat height, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color=D3DXVECTOR4(1,1,1,1));
	static SmartPointer<MeshData> CreateCapsule(const std::string& name, bool saveSmart, gFloat radius, gFloat height, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color=D3DXVECTOR4(1,1,1,1));

	static void Subdivide(SmartPointer<MeshData> meshData);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::string texFilename;
	D3DXVECTOR4 color;
};
}	// namespace
#endif	// GLADE_GEOMETRY_GENERATOR_H
