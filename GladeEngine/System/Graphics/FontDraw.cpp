#include "FontDraw.h"
using namespace Glade;

bool FontDraw::InitFontDraw(HWND hWnd, int screenWidth, int screenHeight)
{
	width = screenWidth;
	height = screenHeight;
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
	if(FAILED(D3DX11CompileFromFile("../GladeEngine/Shaders/font.fx", 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &shaderBuffer, &errorMessage, 0)))
	{
		if(errorMessage != 0)
		{
			MessageBox(hWnd, (char*)errorMessage->GetBufferPointer(), 0, 0);
			errorMessage->Release();
			errorMessage = 0;
		}
		return false;
	}
	if(FAILED(D3DX11CreateEffectFromMemory(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), 0, dev, &gpFontEffect)))
		return false;
	shaderBuffer->Release();
	shaderBuffer = 0;

	sFontViewMatrix = gpFontEffect->GetVariableByName("viewMatrix")->AsMatrix();
	sFontProjMatrix = gpFontEffect->GetVariableByName("projectionMatrix")->AsMatrix();
	sTexture = gpFontEffect->GetVariableByName("shaderTexture")->AsShaderResource();
	sColor = gpFontEffect->GetVariableByName("color")->AsVector();
	sFontTechnique = gpFontEffect->GetTechniqueByName("FontTechnique");

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	unsigned int numElements = sizeof(vertexDesc) / sizeof(vertexDesc[0]);
	
	D3DX11_PASS_DESC passDesc;
	sFontTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	if(FAILED(dev->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize, &gpFontLayout)))
			return false;

	LoadFontData("../GladeEngine/Font/Default.txt", "../GladeEngine/Font/Default.dds");
	sTexture->SetResource(fontTexture);
	return true;
}

void FontDraw::Shutdown()
{
	ReleaseFontData();
	for(auto iter = sentences.begin(); iter != sentences.end(); ++iter)
	{
		iter->second->vertexBuffer->Release();
		iter->second->vertexBuffer = 0;
		iter->second->indexBuffer->Release();
		iter->second->indexBuffer = 0;
		delete iter->second;
	}
}

void FontDraw::Render(Matrix view, Matrix proj)
{
	devCon->IASetInputLayout(gpFontLayout);
	sFontViewMatrix->SetMatrix((float*)view);
	sFontProjMatrix->SetMatrix((float*)proj);
	
	UINT stride = sizeof(SVertex);
	UINT offset = 0;
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for(auto iter = sentences.begin(); iter != sentences.end(); ++iter)
	{
		devCon->IASetVertexBuffers(0, 1, &iter->second->vertexBuffer, &stride, &offset);
		devCon->IASetIndexBuffer(iter->second->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		sColor->SetFloatVector((float*)D3DXVECTOR4(iter->second->red, iter->second->green, iter->second->blue, 1.0f));
		sFontTechnique->GetPassByIndex(0)->Apply(0, devCon);

		devCon->DrawIndexed(iter->second->indexCount, 0, 0);
	}
}

void FontDraw::PushSentence(std::string name, char* text, int posX, int posY, float r, float g, float b)
{
	int numLetters = (int)strlen(text);
	if(sentences.find(name) == sentences.end())
		InitializeSentence(name, numLetters+5);

	if(sentences[name]->maxLength < numLetters)
	{
		sentences.erase(name);
		InitializeSentence(name, numLetters+5);
	}
	
	// Create and initialize Vertices to 0 for now
	SVertex* vertices = new SVertex[sentences[name]->vertexCount];
	memset(vertices, 0, sizeof(SVertex) * sentences[name]->vertexCount);

	// Build Vertex Array for Sentence
	BuildVertexArray((void*)vertices, text, (float)(((width / 2) * -1) + posX), (float)((height / 2) - posY));
	 
	// Copy data into vertex buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	devCon->Map(sentences[name]->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, (void*)vertices, sizeof(SVertex) * sentences[name]->vertexCount);
	devCon->Unmap(sentences[name]->vertexBuffer, 0);

	// Set Sentence color
	sentences[name]->red = r;
	sentences[name]->green = g;
	sentences[name]->blue = b;

	delete [] vertices;
	vertices = 0;
}

void FontDraw::PopSentence(std::string name)
{
	auto iter = sentences.find(name);
	if(iter == sentences.end()) return;

	iter->second->vertexBuffer->Release();
	iter->second->vertexBuffer = 0;
	iter->second->indexBuffer->Release();
	iter->second->indexBuffer = 0;
	delete iter->second;
}

bool FontDraw::InitializeSentence(std::string name, int maxLength)
{
	SVertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	sentences[name] = new SentenceType;
	sentences[name]->vertexBuffer = sentences[name]->indexBuffer = 0;
	sentences[name]->maxLength = maxLength;
	sentences[name]->vertexCount = sentences[name]->indexCount = 6 * maxLength;

	vertices = new SVertex[sentences[name]->vertexCount];
	indices = new unsigned long[sentences[name]->indexCount];
	memset(vertices, 0, sizeof(SVertex) * sentences[name]->vertexCount);
	for(unsigned int i = 0; i < sentences[name]->indexCount; ++i)
		indices[i] = i;

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(SVertex) * sentences[name]->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	
	if(FAILED(dev->CreateBuffer(&vertexBufferDesc, &vertexData, &sentences[name]->vertexBuffer)))
		return false;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * sentences[name]->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if(FAILED(dev->CreateBuffer(&indexBufferDesc, &indexData, &sentences[name]->indexBuffer)))
		return false;

	delete [] vertices;
	vertices = 0;
	delete [] indices;
	indices = 0;
	return true;
}

void FontDraw::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	SVertex* vertexPtr;
	unsigned int numLetters, index, letter;

	vertexPtr = (SVertex*)vertices;
	numLetters = (int)strlen(sentence);
	index = 0;

	for(unsigned int i = 0; i < numLetters; ++i)
	{
		letter = ((unsigned int)sentence[i] - 32);
		if(letter == 0)
			drawX = drawX + 3.0f;
		else
		{
			// First triangle
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			vertexPtr[index++].texCoord = D3DXVECTOR2(font[letter].left, 0.0f);
			vertexPtr[index].position = D3DXVECTOR3(drawX + font[letter].size, drawY-16, 0.0f);
			vertexPtr[index++].texCoord = D3DXVECTOR2(font[letter].right, 1.0f);
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY-16, 0.0f);
			vertexPtr[index++].texCoord = D3DXVECTOR2(font[letter].left, 1.0f);

			// Second triangle
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			vertexPtr[index++].texCoord = D3DXVECTOR2(font[letter].left, 0.0f);
			vertexPtr[index].position = D3DXVECTOR3(drawX + font[letter].size, drawY, 0.0f);
			vertexPtr[index++].texCoord = D3DXVECTOR2(font[letter].right, 0.0f);
			vertexPtr[index].position = D3DXVECTOR3(drawX + font[letter].size, drawY-16, 0.0f);
			vertexPtr[index++].texCoord = D3DXVECTOR2(font[letter].right, 1.0f);

			drawX = drawX + font[letter].size + 1.0f;
		}
	}
}

bool FontDraw::LoadFontData(char* fontIndexFilename, char* fontTextureFilename)
{
	std::ifstream in;
	int i;
	char temp;

	font = new FontType[95];

	// Read in 95 characters used for text
	in.open(fontIndexFilename);
	if(in.fail()) return false;
	for(i = 0; i < 95; ++i)
	{
		in.get(temp);
		while(temp != ' ')
			in.get(temp);
		in.get(temp);
		while(temp != ' ')
			in.get(temp);
		
		in >> font[i].left;
		in >> font[i].right;
		in >> font[i].size;
	}

	in.close();

	if(FAILED(D3DX11CreateShaderResourceViewFromFile(dev, fontTextureFilename, NULL, NULL, &
		fontTexture, NULL)))
		return false;
	return true;
}

void FontDraw::ReleaseFontData()
{
	if(font)
	{
		delete [] font;
		font = 0;
	}

	if(fontTexture)
	{
		fontTexture->Release();
		fontTexture = 0;
	}
}