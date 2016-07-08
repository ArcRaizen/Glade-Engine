#include "DebugDraw.h"
using namespace Glade;

bool DebugDraw::InitDebugDraw()
{
	maintainBetweenDraw = true;
	resetQueue = true;

	g = GraphicsLocator::GetGraphics();
	dev = g->gpDevice;
	devCon = g->gpDeviceContext;

	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* shaderBuffer = 0;
	DWORD shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	if(FAILED(D3DX11CompileFromFile("../GladeEngine/Shaders/color.fx", 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &shaderBuffer, &errorMessage, 0)))
		return false;
	if(FAILED(D3DX11CreateEffectFromMemory(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), 0, dev, &gpDebugEffect)))
		return false;
	shaderBuffer->Release();
	shaderBuffer = 0;

	sDebugViewMatrix = gpDebugEffect->GetVariableByName("viewMatrix")->AsMatrix();
	sDebugProjMatrix = gpDebugEffect->GetVariableByName("projectionMatrix")->AsMatrix();
	sColor = gpDebugEffect->GetVariableByName("color")->AsVector();
	sDebugTechnique = gpDebugEffect->GetTechniqueByName("ColorTechnique");

	// Setup layout of data that goes into shader
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
	{	
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	unsigned int numElements = sizeof(vertexDesc) / sizeof(vertexDesc[0]);

	D3DX11_PASS_DESC passDesc;
	sDebugTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	if(FAILED(dev->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &gpDebugLayout)))
		return false;

	vertexBufferMaxSize = 100;
	vertices = new SVertexC[vertexBufferMaxSize];

	// Buffer Descriptipn
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(SVertexC) * vertexBufferMaxSize;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Subresource Data
	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create buffer
	dev->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	return true;
}

void DebugDraw::Shutdown()
{
	gpDebugLayout->Release();
	gpDebugLayout = 0;
	gpDebugEffect->Release();
	gpDebugEffect = 0;
	vertexBuffer->Release();
	vertexBuffer = 0;
}

void DebugDraw::Render(Matrix view, Matrix proj)
{
	// Set settings for DebugDraw
	devCon->RSSetState(g->gpRasterStateSolid);
	devCon->IASetInputLayout(gpDebugLayout);
	sDebugViewMatrix->SetMatrix((float*)view);
	sDebugProjMatrix->SetMatrix((float*)proj);

	// Update data in vertex buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	devCon->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &vertices[0], sizeof(SVertexC) * vertexBufferSize);
	devCon->Unmap(vertexBuffer, 0);

	// Bind Vertex Buffer to pipeline
	UINT stride = sizeof(SVertexC);
	UINT offset = 0;
	devCon->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	sDebugTechnique->GetPassByIndex(0)->Apply(0, devCon);	

	vertexBufferCounter = 0;
	std::queue<DQ> q;
	while(!drawQueue.empty())
	{
		if(maintainBetweenDraw)
			q.push(drawQueue.front());

		switch(drawQueue.front().i)
		{
			case 1:	DrawLine(); break;
			case 2: DrawTriangle(); break;
			case 3: DrawSquare(); break;
			case 4: DrawBox(); break;
			default:
				AssertMsg(false, "Attempted to draw invalid primitive shape");
				break;
		}
		
		drawQueue.pop();
	}

//	SVertexC v2[20];
//	for(unsigned int i = 0; i < vertexBufferCounter; ++i)
//		v2[i] = vertices[i];

	if(maintainBetweenDraw)
	{
		while(!q.empty())
		{
			drawQueue.push(q.front());
			q.pop();
		}
		resetQueue = true;
	}

	// clear vertex buffer
	vertexBufferSize = 0;

	// Reset default settings
	devCon->IASetInputLayout(g->gpLayout);
	devCon->RSSetState(g->gpRasterStateSolid);
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DebugDraw::PushLine(Vector p1, Vector p2, Color c) 
{
	if(maintainBetweenDraw && resetQueue)
	{
		resetQueue = false;
		while(!drawQueue.empty())
			drawQueue.pop();
	}

	if(vertexBufferSize + 2 > vertexBufferMaxSize)
		IncreaseVertexBufferSize();

	// add new vertices onto buffer (using vertexBufferSize as index)
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p1.x, p1.y, p1.z);
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p2.x, p2.y, p2.z);
	drawQueue.push(DQ(1,c));
}
void DebugDraw::PushTriangle(Vector p1, Vector p2, Vector p3, Color c)
{
	if(maintainBetweenDraw && resetQueue)
	{
		resetQueue = false;
		while(!drawQueue.empty())
			drawQueue.pop();
	}

	if(vertexBufferSize + 3 > vertexBufferMaxSize)
		IncreaseVertexBufferSize();

	// add new vertices onto buffer (using vertexBufferSize as index)
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p1.x, p1.y, p1.z);
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p2.x, p2.y, p2.z);
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p3.x, p3.y, p3.z);
	drawQueue.push(DQ(2,c));
}
void DebugDraw::PushSquare(Vector p1, Vector p2, Vector p3, Vector p4, Color c)
{
	if(maintainBetweenDraw && resetQueue)
	{
		resetQueue = false;
		while(!drawQueue.empty())
			drawQueue.pop();
	}

	if(vertexBufferSize + 6 > vertexBufferMaxSize)
		IncreaseVertexBufferSize();

	// add new vertices onto buffer (using vertexBufferSize as index)
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p1.x, p1.y, p1.z);
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p2.x, p2.y, p2.z);
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p3.x, p3.y, p3.z);
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p2.x, p2.y, p2.z);
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p4.x, p4.y, p4.z);
	vertices[vertexBufferSize++].position =	D3DXVECTOR3(p3.x, p3.y, p3.z);

	drawQueue.push(DQ(3,c));
}

void DebugDraw::PushBox(Vector center, Vector extents, Color c)
{
	if(maintainBetweenDraw && resetQueue)
	{
		resetQueue = false;
		while(!drawQueue.empty())
			drawQueue.pop();
	}

	if(vertexBufferSize + 36 > vertexBufferMaxSize)
		IncreaseVertexBufferSize();

	D3DXVECTOR3 cen(center.x, center.y, center.z);
	gFloat w = extents.x, h = extents.y, d = extents.z;

	// Horrible Code - DO NOT LOOK
#pragma region BOX VERTICES
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, -h, -d);		// Front 1
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, +h, -d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, +h, -d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, -h, -d);		// Front 2
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, +h, -d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, -h, -d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, -h, +d);		// Back 1
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, -h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, +h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, -h, +d);		// Back 2
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, +h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, +h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, +h, -d);		// Top 1
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, +h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, +h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, +h, -d);		// Top 2
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, +h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, +h, -d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, -h, -d);		// Bot 1
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, -h, -d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, -h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, -h, -d);		// Bot 2
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, -h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, -h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, -h, +d);		// Left 1
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, +h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, +h, -d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, -h, +d);		// Left 2
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, +h, -d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(-w, -h, -d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, -h, -d);		// Right 1
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, +h, -d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, +h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, -h, -d);		// Right 2
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, +h, +d);
	vertices[vertexBufferSize++].position =	cen + D3DXVECTOR3(+w, -h, +d);
	// I just don't know what I expected FUCK
#pragma endregion
	drawQueue.push(DQ(4,c));
}

void DebugDraw::DrawLine() 
{ 
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	sColor->SetFloatVector((float*)drawQueue.front().c);
	devCon->Draw(2, vertexBufferCounter);
	vertexBufferCounter += 2;
}
void DebugDraw::DrawTriangle()
{
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devCon->Draw(3, vertexBufferCounter);
	sColor->SetFloatVector((float*)drawQueue.front().c);
	vertexBufferCounter += 3;
}
void DebugDraw::DrawSquare()
{ 
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	devCon->IASetInputLayout(gpDebugLayout);
	sColor->SetFloatVector((float*)drawQueue.front().c);
	sDebugTechnique->GetPassByIndex(0)->Apply(0, devCon);
	devCon->Draw(6, vertexBufferCounter);
	vertexBufferCounter += 6;
}
void DebugDraw::DrawBox()
{
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	devCon->IASetInputLayout(gpDebugLayout);
	sColor->SetFloatVector((float*)drawQueue.front().c);
	sDebugTechnique->GetPassByIndex(0)->Apply(0, devCon);
	devCon->Draw(36, vertexBufferCounter);
	vertexBufferCounter += 36;
}

inline
void DebugDraw::IncreaseVertexBufferSize()
{
	int newVertexBufferSize = vertexBufferMaxSize + vertexBufferMaxSize / 2;

	// create new vertex buffer
	SVertexC* v2 = new SVertexC[newVertexBufferSize];

	// copy original buffer over to new buffer
	memcpy(v2, vertices, sizeof(SVertexC) * vertexBufferMaxSize);

	// Delete old list, move to new one
	delete [] vertices;
	vertices = v2;

	// Recreate vertex buffer to use new list
	vertexBufferDesc.ByteWidth = sizeof(SVertexC) * newVertexBufferSize;
	vertexData.pSysMem = &vertices[0];
	vertexBuffer->Release();
	vertexBuffer = 0;
	dev->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	vertexBufferMaxSize = newVertexBufferSize;
}