#include "GeometryGenerator.h"

using namespace Glade;

MeshData::MeshData(std::string& name, std::vector<Vertex> verts, std::vector<unsigned int> inds, std::string& filename, D3DXVECTOR4 c) : 
			Resource(name), vertices(verts), indices(inds), texFilename(filename), color(c)
{ }

SmartPointer<MeshData> MeshData::CreateBox(const std::string& name, bool saveSmart, gFloat width, gFloat height, gFloat depth, std::string texFilename, D3DXVECTOR4 c)
{
	std::vector<Vertex> v;
	gFloat w2 = width * gFloat(0.5f);
	gFloat h2 = height * gFloat(0.5f);
	gFloat d2 = depth * gFloat(0.5f);

	// Front face
	v.emplace_back(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v.emplace_back(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v.emplace_back(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v.emplace_back(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	// Back face
	v.emplace_back(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v.emplace_back(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v.emplace_back(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v.emplace_back(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// Top face
	v.emplace_back(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v.emplace_back(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v.emplace_back(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v.emplace_back(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	// Bottom face
	v.emplace_back(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v.emplace_back(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v.emplace_back(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v.emplace_back(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// Left face
	v.emplace_back(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v.emplace_back(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v.emplace_back(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v.emplace_back(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	// Right face
	v.emplace_back(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v.emplace_back(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v.emplace_back(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v.emplace_back(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	std::vector<unsigned int> i;

	// Fill in the front face index data
	i.push_back(0); i.push_back(1); i.push_back(2);
	i.push_back(0); i.push_back(2); i.push_back(3);

	// Fill in the back face index data
	i.push_back(4); i.push_back(5); i.push_back(6);
	i.push_back(4); i.push_back(6); i.push_back(7);

	// Fill in the top face index data
	i.push_back(8); i.push_back(9); i.push_back(10);
	i.push_back(8); i.push_back(10); i.push_back(11);

	// Fill in the bottom face index data
	i.push_back(12); i.push_back(13); i.push_back(14);
	i.push_back(12); i.push_back(14); i.push_back(15);

	// Fill in the left face index data
	i.push_back(16); i.push_back(17); i.push_back(18);
	i.push_back(16); i.push_back(18); i.push_back(19);

	// Fill in the right face index data
	i.push_back(20); i.push_back(21); i.push_back(22);
	i.push_back(20); i.push_back(22); i.push_back(23);

	auto newMesh = SmartPointer<MeshData>(new MeshData(name.length()>0? name : GenerateName("Unnamed MeshData %i"), v, i, "../GladeEngine/Textures/" + texFilename, c));
	if(newMesh) 
	{
		if(saveSmart)
			RegisterSmartResource(newMesh);
		else
			RegisterWeakResource(newMesh);
	}
	return newMesh;
}

SmartPointer<MeshData> MeshData::CreateSphere(const std::string& name, bool saveSmart, gFloat radius, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color)
{
	Vertex top(0.0f, radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bot(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	std::vector<Vertex> verts;
	verts.push_back(top);

	gFloat phiStep = PI / stackCount;
	gFloat thetaStep = TWO_PI / sliceCount;
	gFloat phi, theta, cPhi, sPhi, cTheta, sTheta;
	Vertex v;

	for(unsigned int i = 1; i < stackCount; ++i)
	{
		phi = i * phiStep;
		sPhi = Sin(phi);	
		cPhi = Cos(phi);
		for(unsigned int j = 0; j <= sliceCount; ++j)
		{
			theta = j*thetaStep;
			sTheta = Sin(theta);
			cTheta = Cos(theta);

			v.position.x = radius * sPhi * cTheta;
			v.position.y = radius * cPhi;
			v.position.z = radius * sPhi * sTheta;

			v.tangent.x = -radius * sPhi * sTheta;
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sPhi * cTheta;

			D3DXVec3Normalize(&v.tangent, &v.tangent);
			D3DXVec3Normalize(&v.normal, &v.position);

			v.texCoord.x = theta / TWO_PI;
			v.texCoord.y = phi / PI;

			verts.push_back(v);
		}
	}
	verts.push_back(bot);
	std::vector<unsigned int> inds;

	// Indices for top stack
	for(unsigned int i = 1; i <= sliceCount; ++i)
	{
		inds.push_back(0);
		inds.push_back(i+1);
		inds.push_back(i);
	}

	// Indices for inner stacks
	unsigned int baseIndex = 1;
	unsigned int ringVertexCount = sliceCount+1;
	for(unsigned int i = 0; i < stackCount-2; ++i)
	{
		for(unsigned int j = 0; j < sliceCount; ++j)
		{
			inds.push_back(baseIndex + i*ringVertexCount + j);
			inds.push_back(baseIndex + i*ringVertexCount + j+1);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j);
			inds.push_back(baseIndex + i*ringVertexCount + j+1);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
		}
	}

	// Indices for bottom stack
	unsigned int southPoleIndex = (unsigned int)verts.size()-1;
	baseIndex = southPoleIndex - ringVertexCount;
	for(unsigned int i = 0; i < sliceCount; ++i)
	{
		inds.push_back(southPoleIndex);
		inds.push_back(baseIndex+i);
		inds.push_back(baseIndex+i+1);
	}

	auto newMesh = SmartPointer<MeshData>(new MeshData(name.length()>0? name : GenerateName("Unnamed MeshData %i"), verts, inds, "../GladeEngine/Textures/" + texFilename, color));
	if(newMesh) 
	{
		if(saveSmart)
			RegisterSmartResource(newMesh);
		else
			RegisterWeakResource(newMesh);
	}
	return newMesh;
}

SmartPointer<MeshData> MeshData::CreateCylinder(const std::string& name, bool saveSmart, gFloat radius, gFloat height, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color)
{
	std::vector<Vertex> verts;
	gFloat halfHeight = height * gFloat(0.5f);
	Vertex top(0.0f, halfHeight, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f);
	Vertex bottom(0.0f, -halfHeight, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f);
	verts.push_back(bottom);

	gFloat heightStep = height / stackCount;
	gFloat thetaStep = TWO_PI / sliceCount;
	gFloat h, theta, c, s;
	Vertex v;

	// Create bottom 
	v.normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	v.tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	for(unsigned int j = 0; j <= sliceCount; ++j)
	{
		theta = j * thetaStep;
		c = Cos(theta);
		s = Sin(theta);

		v.position.x = radius * c;
		v.position.y = -halfHeight;
		v.position.z = radius * s;

//		v.texCoord.x = v.position.x / height + gFloat(0.5f); // Scale down by the height to try and make top cap texture coord area proportional to base
//		v.texCoord.y = v.position.z / height + gFloat(0.5f);
		v.texCoord.x = gFloat(0.5f) * c + gFloat(0.5f);
		v.texCoord.y = gFloat(0.5f) * s + gFloat(0.5f);

		verts.push_back(v);
	}

	// Create middle of cylinder
	for(unsigned int i = 0; i <= stackCount; ++i)
	{
		h = -halfHeight + i*heightStep;
		for(unsigned int j = 0; j <= sliceCount; ++j)
		{
			theta = j * thetaStep;
			c = Cos(theta);
			s = Sin(theta);

			v.position.x = radius * c;
			v.position.y = h;
			v.position.z = radius * s;

			v.texCoord.x = gFloat(j)/sliceCount;
			v.texCoord.y = gFloat(1.0f) - gFloat(i)/stackCount;

			v.tangent.x = -s;
			v.tangent.y = 0.0f;
			v.tangent.z = c;

			D3DXVECTOR3 bitangent(0, -height, 0);
			D3DXVec3Cross(&v.tangent, &bitangent, &v.normal);
			D3DXVec3Normalize(&v.normal, &v.normal);

			verts.push_back(v);
		}
	}

	// Create top
	v.normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	v.tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	for(unsigned int j = 0; j <= sliceCount; ++j)
	{
		theta = j * thetaStep;
		c = Cos(theta);
		s = Sin(theta);

		v.position.x = radius * c;
		v.position.y = halfHeight;
		v.position.z = radius * s;

//		v.texCoord.x = v.position.x / height + gFloat(0.5f); // Scale down by the height to try and make top cap texture coord area proportional to base
//		v.texCoord.y = v.position.z / height + gFloat(0.5f);
		v.texCoord.x = gFloat(0.5f) * c + gFloat(0.5f);
		v.texCoord.y = gFloat(0.5f) * s + gFloat(0.5f);

		verts.push_back(v);
	}
	verts.push_back(top);
	std::vector<unsigned int> inds;

	// Indicies for bottom
	unsigned int ringVertexCount = sliceCount+1;	// we duplicate 1st/last vertex per ring because they haver different UVs
	for(unsigned int i = 0; i < ringVertexCount; ++i)
	{
		inds.push_back(0);
		inds.push_back(i);
		inds.push_back(i+1);
	}

	// Indices for middle
	unsigned int baseIndex = sliceCount+2;
	for(unsigned int i = 0; i < stackCount; ++i)
	{
		for(unsigned int j = 0; j < sliceCount; ++j)
		{
			inds.push_back(baseIndex + i*ringVertexCount + j);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j+1);

			inds.push_back(baseIndex + i*ringVertexCount + j);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
			inds.push_back(baseIndex + (i)*ringVertexCount + j+1);
		}
	}

	// Indices for top
	unsigned int topIndex = verts.size()-1;
	baseIndex = topIndex - ringVertexCount;
	for(unsigned int i = 0; i < sliceCount; ++i)
	{
		inds.push_back(topIndex);
		inds.push_back(baseIndex+i+1);
		inds.push_back(baseIndex+i);
	}

	auto newMesh = SmartPointer<MeshData>(new MeshData(name.length()>0? name : GenerateName("Unnamed MeshData %i"), verts, inds, "../GladeEngine/Textures/" + texFilename, color));
	if(newMesh) 
	{
		if(saveSmart)
			RegisterSmartResource(newMesh);
		else
			RegisterWeakResource(newMesh);
	}
	return newMesh;
}

SmartPointer<MeshData> MeshData::CreateCone(const std::string& name, bool saveSmart, gFloat radius, gFloat height, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color)
{
	std::vector<Vertex> verts;
	gFloat halfHeight = height * gFloat(0.5f);
	Vertex top(0.0f, halfHeight, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f);
	Vertex bottom(0.0f, -halfHeight, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f);
	verts.push_back(bottom);

	gFloat heightStep = height / stackCount;
	gFloat thetaStep = TWO_PI / sliceCount;
	gFloat radiusStep = -radius / stackCount;
	gFloat h, r, theta, c, s;
	Vertex v;

	// Create bottom
	v.normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	v.tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	for(unsigned int i = 0; i <= sliceCount; ++i)
	{
		theta = i * thetaStep;
		c = Cos(theta);
		s = Sin(theta);

		v.position.x = radius * c;
		v.position.y = -halfHeight;
		v.position.z = radius * s;

		v.texCoord.x = gFloat(0.5f) * c + gFloat(0.5f);
		v.texCoord.y - gFloat(0.5f) * s + gFloat(0.5f);

		verts.push_back(v);
	}

	// Create middle of cone
	for(unsigned int i = 0; i < stackCount; ++i)
	{
		h = -halfHeight + i*heightStep;
		r = radius + i*radiusStep;
		for(unsigned int j = 0; j <= sliceCount; ++j)
		{
			theta = j * thetaStep;
			c = Cos(theta);
			s = Sin(theta);

			v.position.x = r * c;
			v.position.y = h;
			v.position.z = r * s;

			v.texCoord.x = gFloat(j) / sliceCount;
			v.texCoord.y = gFloat(1.0f) - gFloat(i)/stackCount;

			v.tangent.x = -s;
			v.tangent.y = 0.0f;
			v.tangent.z = c;

			D3DXVECTOR3 bitangent(radius*c, -height, radius*s);
			D3DXVec3Cross(&v.tangent, &bitangent, &v.normal);
			D3DXVec3Normalize(&v.normal, &v.normal);

			verts.push_back(v);
		}
	}
	verts.push_back(top);
	std::vector<unsigned int> inds;

	// Indices for bottom
	unsigned int ringVertexCount = sliceCount + 1;
	for(unsigned int i = 0; i < ringVertexCount; ++i)
	{
		inds.push_back(0);
		inds.push_back(i);
		inds.push_back(i+1);
	}

	// Indices for middle
	unsigned int baseIndex = sliceCount+2;
	for(unsigned int i = 0; i < stackCount-1; ++i)
	{
		for(unsigned int j = 0; j < sliceCount; ++j)
		{
			inds.push_back(baseIndex + i*ringVertexCount + j);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j+1);

			inds.push_back(baseIndex + i*ringVertexCount + j);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
			inds.push_back(baseIndex + (i)*ringVertexCount + j+1);
		}
	}

	// Indices for tip
	unsigned int topIndex = verts.size() - 1;
	baseIndex = topIndex - ringVertexCount;
	for(unsigned int i = 0; i < sliceCount; ++i)
	{
		inds.push_back(topIndex);
		inds.push_back(baseIndex+i+1);
		inds.push_back(baseIndex+i);
	}

	auto newMesh = SmartPointer<MeshData>(new MeshData(name.length()>0? name : GenerateName("Unnamed MeshData %i"), verts, inds, "../GladeEngine/Textures/" + texFilename, color));
	if(newMesh) 
	{
		if(saveSmart)
			RegisterSmartResource(newMesh);
		else
			RegisterWeakResource(newMesh);
	}
	return newMesh;
}

SmartPointer<MeshData> MeshData::CreateCapsule(const std::string& name, bool saveSmart, gFloat radius, gFloat height, unsigned int sliceCount, unsigned int stackCount, std::string texFilename, D3DXVECTOR4 color)
{
	std::vector<Vertex> verts;
	gFloat halfHeight = height * gFloat(0.5f);
	Vertex top(0.0f, radius+halfHeight, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bot(0.0f, -radius-halfHeight, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	verts.push_back(bot);

	// StackCount tells how many stacks total between both end caps, but they need to be symmetrical
	// and the middle rings of each cap need to be at the max radius of the capsule, meaning we need
	// an even number of stacks.
	// Just override to the next highest even number if an odd one is passed in
	if(stackCount % 2 == 1) stackCount += 1;

	gFloat phiStep = PI / stackCount;
	gFloat thetaStep = TWO_PI / sliceCount;
	gFloat phi, theta, h, cPhi, sPhi, cTheta, sTheta;
	unsigned int i, j;
	Vertex v;

	// Create bottom cap
	for(i = 1; i <= stackCount/2; ++i)
	{
		phi = i * phiStep;
		sPhi = Sin(phi);
		cPhi = Cos(phi);
		for(j = 0; j <= sliceCount; ++j)
		{
			theta = j * thetaStep;
			sTheta = Sin(theta);
			cTheta = Cos(theta);

			v.position.x = radius * sPhi * cTheta;
			v.position.y = -radius * cPhi;
			v.position.z = radius * sPhi * sTheta;

			v.tangent.x = -radius * sPhi * sTheta;
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sPhi * cTheta;

			D3DXVec3Normalize(&v.tangent, &v.tangent);
			D3DXVec3Normalize(&v.normal, &v.position);
			v.position.y -= halfHeight;

			v.texCoord.x = theta = TWO_PI;
			v.texCoord.y = phi / PI;

			verts.push_back(v);
		}
	}

	// Create middle cylinder
/*	for(i = 0; i < 2; ++i)
	{
		h = -halfHeight + i*height;
		for(j = 0; j <= sliceCount; ++j)
		{
			theta = j * thetaStep;
			sTheta = Sin(theta);
			cTheta = Cos(theta);

			v.position.x = radius * cTheta;
			v.position.y = h;
			v.position.z = radius * sTheta;

			v.texCoord.x = gFloat(j)/sliceCount;
			v.texCoord.y = gFloat(1.0f) - gFloat(i)/stackCount;

			v.tangent.x = -sTheta;
			v.tangent.y = 0.0f;
			v.tangent.z = cTheta;

			D3DXVECTOR3 bitangent(0, -height, 0);
			D3DXVec3Cross(&v.tangent, &bitangent, &v.normal);
			D3DXVec3Normalize(&v.normal, &v.normal);

			meshData->vertices.push_back(v);
		}
	}
*/
	// Create top cap
	for(i = stackCount/2; i > 0; --i)
	{
		phi = i * phiStep;
		sPhi = Sin(phi);
		cPhi = Cos(phi);
		for(j = 0; j <= sliceCount; ++j)
		{
			theta = j * thetaStep;
			sTheta = Sin(theta);
			cTheta = Cos(theta);

			v.position.x = radius * sPhi * cTheta;
			v.position.y = radius * cPhi;
			v.position.z = radius * sPhi * sTheta;

			v.tangent.x = -radius * sPhi * sTheta;
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sPhi * cTheta;

			D3DXVec3Normalize(&v.tangent, &v.tangent);
			D3DXVec3Normalize(&v.normal, &v.position);
			v.position.y += halfHeight;

			v.texCoord.x = theta = TWO_PI;
			v.texCoord.y = phi / PI;

			verts.push_back(v);
		}
	}
	verts.push_back(top);
	std::vector<unsigned int> inds;

	// Indices for bottom stack
	for(i = 1; i <= sliceCount; ++i)
	{
		inds.push_back(0);
		inds.push_back(i);
		inds.push_back(i+1);
	}

	// Indices for middle
	unsigned int baseIndex = 1;
	unsigned int ringVertexCount = sliceCount+1;
	for(i = 0; i < stackCount-1; ++i)
	{
		for(j = 0; j < sliceCount; ++j)
		{
			inds.push_back(baseIndex + i*ringVertexCount + j);
			inds.push_back(baseIndex + i*ringVertexCount + j+1);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j);
			inds.push_back(baseIndex + i*ringVertexCount + j+1);
			inds.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
		}
	}

	// Indices for top stack
	unsigned int topIndex = verts.size() - 1;
	baseIndex = topIndex - ringVertexCount;
	for(i = 0; i < sliceCount; ++i)
	{
		inds.push_back(topIndex);
		inds.push_back(baseIndex+i+1);
		inds.push_back(baseIndex+i);
	}


	auto newMesh = SmartPointer<MeshData>(new MeshData(name.length()>0? name : GenerateName("Unnamed MeshData %i"), verts, inds, "../GladeEngine/Textures/" + texFilename, color));
	if(newMesh) 
	{
		if(saveSmart)
			RegisterSmartResource(newMesh);
		else
			RegisterWeakResource(newMesh);
	}
	return newMesh;
}


// Taken a mesh of vertices and divide every triangle into 4 triangles
//       
//        * v1							        * v1
//       / \							       / \
//      /   \							      /   \
//     /	 \		From -------> To	 m0  *-----*	m1	
//	  /       \								/ \   / \
//	 /         \						   /   \ /   \
//	*-----------* v2					  *-----*-----* v2
// v0     								v0     m2
//       
void MeshData::Subdivide(SmartPointer<MeshData> meshData)
{
	SmartPointer<MeshData> temp = meshData;
	meshData->vertices.resize(0);
	meshData->indices.resize(0);

	unsigned int numTris = temp->indices.size() / 3;
	Vertex v0, v1, v2, m0, m1, m2;
	for(unsigned int i = 0; i < numTris; ++i)
	{
		// Find original vertices of each triangle
		v0 = temp->vertices[temp->indices[i*3]];
		v1 = temp->vertices[temp->indices[i*3+1]];
		v2 = temp->vertices[temp->indices[i*3+2]];

		// Calculate midpoints of each triangle's segments
		// Flat surface - straight average
		if(v0.normal == v1.normal)
		{
			m0.position = (v0.position + v1.position) * 0.5f;
			m0.normal = v0.normal;
			m0.tangent = v0.tangent;
			m0.texCoord = (v0.texCoord + v1.texCoord) * 0.5f;
		}
		else	// Spherical/curved surface - TRIG GO
		{
			D3DXVec3Normalize(&m0.normal, &((v0.position + v1.position) * 0.5f));
			gFloat radius = (D3DXVec3Length(&v0.position) + D3DXVec3Length(&v1.position)) * 0.5f;
			m0.position = m0.normal * radius;

			// Tex Coords from spherical coords
			gFloat theta, phi;
			if(m0.position.x >= 0.0f)
			{
				theta = ATan(m0.position.z / m0.position.x);
				if(theta < 0.0f)
					theta += TWO_PI;
			}
			else
				theta = ATan(m0.position.z / m0.position.x) + PI;
			
			phi = ACos(m0.position.y / radius);

			m0.texCoord.x = theta / TWO_PI;
			m0.texCoord.y = phi / PI;

			// Partial derivative of P with respect to theta
			m0.tangent.x = -radius * Sin(phi) * Sin(theta);
			m0.tangent.y = 0.0f;
			m0.tangent.z = radius * Sin(phi) * Cos(theta);
			D3DXVec3Normalize(&m0.tangent, &m0.tangent);
		}

		if(v1.normal == v2.normal)
		{
			m1.position = (v1.position + v2.position) * 0.5f;
			m1.normal = v1.normal;
			m1.tangent = v1.tangent;
			m1.texCoord = (v1.texCoord + v2.texCoord) * 0.5f;
		}
		else
		{
			D3DXVec3Normalize(&m1.normal, &((v1.position + v2.position) * 0.5f));
			gFloat radius = (D3DXVec3Length(&v1.position) + D3DXVec3Length(&v2.position)) * 0.5f;
			m1.position = m1.normal * radius;

			// Tex Coords from spherical coords
			gFloat theta, phi;
			if(m1.position.x >= 0.0f)
			{
				theta = ATan(m1.position.z / m1.position.x);
				if(theta < 0.0f)
					theta += TWO_PI;
			}
			else
				theta = ATan(m1.position.z / m1.position.x) + PI;
			
			phi = ACos(m1.position.y / radius);

			m1.texCoord.x = theta / TWO_PI;
			m1.texCoord.y = phi / PI;

			// Partial derivative of P with respect to theta
			m1.tangent.x = -radius * Sin(phi) * Sin(theta);
			m1.tangent.y = 0.0f;
			m1.tangent.z = radius * Sin(phi) * Cos(theta);
			D3DXVec3Normalize(&m1.tangent, &m1.tangent);
		}

		if(v2.normal == v0.normal)
		{
			m2.position = (v2.position + v0.position) * 0.5f;
			m2.normal = v2.normal;
			m2.tangent = v2.tangent;
			m2.texCoord = (v2.texCoord + v0.texCoord) * 0.5f;
		}
		else
		{
			D3DXVec3Normalize(&m2.normal, &((v2.position + v0.position) * 0.5f));
			gFloat radius = (D3DXVec3Length(&v2.position) + D3DXVec3Length(&v0.position)) * 0.5f;
			m2.position = m2.normal * radius;

			// Tex Coords from spherical coords
			gFloat theta, phi;
			if(m2.position.x >= 0.0f)
			{
				theta = ATan(m2.position.z / m2.position.x);
				if(theta < 0.0f)
					theta += TWO_PI;
			}
			else
				theta = ATan(m2.position.z / m2.position.x) + PI;
			
			phi = ACos(m2.position.y / radius);

			m2.texCoord.x = theta / TWO_PI;
			m2.texCoord.y = phi / PI;

			// Partial derivative of P with respect to theta
			m2.tangent.x = -radius * Sin(phi) * Sin(theta);
			m2.tangent.y = 0.0f;
			m2.tangent.z = radius * Sin(phi) * Cos(theta);
			D3DXVec3Normalize(&m2.tangent, &m2.tangent);
		}

		// Add vertices and indices to mesh data
		meshData->vertices.push_back(v0);
		meshData->vertices.push_back(v1);
		meshData->vertices.push_back(v2);
		meshData->vertices.push_back(m0);
		meshData->vertices.push_back(m1);
		meshData->vertices.push_back(m2);

		meshData->indices.push_back(i*6+0);	// bottom left
		meshData->indices.push_back(i*6+3);
		meshData->indices.push_back(i*6+5);
		meshData->indices.push_back(i*6+3);	// middle
		meshData->indices.push_back(i*6+4);
		meshData->indices.push_back(i*6+5);
		meshData->indices.push_back(i*6+5);	// bottom right
		meshData->indices.push_back(i*6+4);
		meshData->indices.push_back(i*6+2);
		meshData->indices.push_back(i*6+3);	// top
		meshData->indices.push_back(i*6+1);
		meshData->indices.push_back(i*6+4);
	}
}