#include "GeometryGenerator.h"

using namespace Glade;

MeshData* GeometryGenerator::CreateBox(gFloat width, gFloat height, gFloat depth, D3DXVECTOR4 c)
{
	MeshData* meshData = new MeshData;
	Vertex v[24];
	gFloat w2 = width * gFloat(0.5f);
	gFloat h2 = height * gFloat(0.5f);
	gFloat d2 = depth * gFloat(0.5f);

	// Front face
	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, c.x, c.y, c.z, c.w);
	v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c.x, c.y, c.z, c.w);
	v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, c.x, c.y, c.z, c.w);
	v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, c.x, c.y, c.z, c.w);
	// Back face
	v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, c.x, c.y, c.z, c.w);
	v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, c.x, c.y, c.z, c.w);
	v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c.x, c.y, c.z, c.w);
	v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, c.x, c.y, c.z, c.w);
	// Top face
	v[8]  = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, c.x, c.y, c.z, c.w);
	v[9]  = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c.x, c.y, c.z, c.w);
	v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, c.x, c.y, c.z, c.w);
	v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, c.x, c.y, c.z, c.w);
	// Bottom face
	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, c.x, c.y, c.z, c.w);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, c.x, c.y, c.z, c.w);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c.x, c.y, c.z, c.w);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, c.x, c.y, c.z, c.w);
	// Left face
	v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, c.x, c.y, c.z, c.w);
	v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, c.x, c.y, c.z, c.w);
	v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, c.x, c.y, c.z, c.w);
	v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, c.x, c.y, c.z, c.w);
	// Right face
	v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, c.x, c.y, c.z, c.w);
	v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, c.x, c.y, c.z, c.w);
	v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, c.x, c.y, c.z, c.w);
	v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, c.x, c.y, c.z, c.w);

	meshData->vertices.assign(&v[0], &v[24]);

	unsigned int i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData->indices.assign(&i[0], &i[36]);
	return meshData;
}

MeshData* GeometryGenerator::CreateSphere(gFloat radius, unsigned int sliceCount, unsigned int stackCount, D3DXVECTOR4 c)
{
	MeshData* meshData = new MeshData;
	Vertex top(0.0f, radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c.x, c.y, c.z, c.w);
	Vertex bot(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, c.x, c.y, c.z, c.w);
	meshData->vertices.push_back(top);

	gFloat phiStep = PI / stackCount;
	gFloat thetaStep = 2.0f * PI / sliceCount;
	gFloat phi, theta;
	Vertex v;

	for(unsigned int i = 1; i <= stackCount-1; ++i)
	{
		phi = i*phiStep;
		for(unsigned int j = 0; j <= sliceCount; ++j)
		{
			theta = j*thetaStep;

			v.position.x = radius * Sin(phi) * Cos(theta);
			v.position.y = radius * Cos(phi);
			v.position.z = radius * Sin(phi) * Sin(theta);

			v.tangent.x = -radius * Sin(phi) * Sin(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = radius * Sin(phi) * Cos(theta);

			D3DXVec3Normalize(&v.tangent, &v.tangent);
			D3DXVec3Normalize(&v.normal, &v.position);

			v.texCoord.x = theta / TWO_PI;
			v.texCoord.y = phi / PI;

			v.color = c;

			meshData->vertices.push_back(v);
		}
	}
	meshData->vertices.push_back(bot);

	// Indices for top stack
	for(unsigned int i = 1; i < sliceCount; ++i)
	{
		meshData->indices.push_back(0);
		meshData->indices.push_back(i+1);
		meshData->indices.push_back(i);
	}

	// Indices for inner stacks
	unsigned int baseIndex = 1;
	unsigned int ringVertexCount = sliceCount+1;
	for(unsigned int i = 0; i <stackCount-2; ++i)
	{
		for(unsigned int j = 0; j < sliceCount; ++j)
		{
			meshData->indices.push_back(baseIndex + i*ringVertexCount + j);
			meshData->indices.push_back(baseIndex + i*ringVertexCount + j+1);
			meshData->indices.push_back(baseIndex + (i+1)*ringVertexCount + j);
			meshData->indices.push_back(baseIndex + (i+1)*ringVertexCount + j);
			meshData->indices.push_back(baseIndex + i*ringVertexCount + j+1);
			meshData->indices.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
		}
	}

	// Indices for bottom stakc
	unsigned int southPoleIndex = (unsigned int)meshData->vertices.size()-1;
	baseIndex = southPoleIndex - ringVertexCount;
	for(unsigned int i = 0; i < sliceCount; ++i)
	{
		meshData->indices.push_back(southPoleIndex);
		meshData->indices.push_back(baseIndex+i);
		meshData->indices.push_back(baseIndex+i+1);
	}

	return meshData;
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
void GeometryGenerator::Subdivide(MeshData* meshData)
{
	MeshData* temp = meshData;
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
			m0.color = (v0.color + v1.color) * 0.5f;
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
			m1.color = (v1.color + v2.color) * 0.5f;
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
			m2.color = (v2.color + v0.color) * 0.5f;
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