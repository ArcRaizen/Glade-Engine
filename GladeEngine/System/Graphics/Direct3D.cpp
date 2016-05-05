#include "Direct3D.h"

using namespace Glade;

#ifndef HR						 
#define HR(x)					 \
{								 \
	HRESULT hr = (x)			 \
	if(FAILED(hr)) return false; \
}								 
#endif

#ifndef ReleaseCOM
#define ReleaseCOM(x)		\
{							\
	(x)->Release();			\
	(x) = 0;				\
}
#endif

Direct3D::Direct3D() : gpSwapChain(0), gpDevice(0), gpDeviceContext(0), gpRenderTargetView(0), gpDepthStencilBuffer(0),
					gpDepthStencilState(), gpDepthStencilView(0), gpRasterStateSolid(0), gpRasterStateWireframe(0), gpLayout(0),
					gpEffect(0), sWorldMatrix(0), sViewMatrix(0), sProjectionMatrix(0), sTexture(0), sTechnique(0)
{ }

Direct3D::~Direct3D()
{ }

bool Direct3D::Initialize(int width, int height, bool vsync, HWND hWnd, bool fullscreen, float screenDepth, float screenNear, unsigned int texSize)
{
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	UINT stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	// Save vsync setting
	vsyncEnabled = vsync;

	// Create DirectX graphics interface factory
	if(FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
		return false;

	// Use factory to create adapter for primary graphics interface
	if(FAILED(factory->EnumAdapters(0, &adapter)))
		return false;

	// Enumerate primary adapter output
	if(FAILED(adapter->EnumOutputs(0, &adapterOutput)))
		return false;

	// Get number of modes that fit DXGI_FORMAT_R8G8B8A8_UNORM display format for adapter output
	if(FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
		return false;

	// Create list to hold all possible display modes for montior/video card combo
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList) return false;

	// Fill display mode list
	if(FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
		return false;

	// Go through all display modes and find the one that matches the screen width/height
	// When a match is found, store the numerator/denominator of refresh rate
	for(i = 0; i < numModes; ++i)
	{
		if(displayModeList[i].Width == (unsigned int)width && displayModeList[i].Height == (unsigned int)height)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// Get adapter (video card) description
	if(FAILED(adapter->GetDesc(&adapterDesc)))
		return false;

	// Store dedicated video card memory in megabytes
	videoCardMem = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert name of video card to character array and save
	if(wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128) != 0)
		return false;

	// Release stuff
	delete [] displayModeList;
	ReleaseCOM(adapterOutput);
	ReleaseCOM(adapter);
	ReleaseCOM(factory);

	// Initialize swap chain desc
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set refresh rate of back buffer
	if(vsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hWnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set fullscreen or windowed
	if(fullscreen)
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	if(FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
					       D3D11_SDK_VERSION, &swapChainDesc, &gpSwapChain, &gpDevice, NULL, &gpDeviceContext)))
		return false;

	// Get pointer to back buffer
	if(FAILED(gpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
		return false;

	// Create render target view with back buffer pointer
	if(FAILED(gpDevice->CreateRenderTargetView(backBufferPtr, NULL, &gpRenderTargetView)))
		return false;

	// Release pointer to back buffer
	ReleaseCOM(backBufferPtr);

	// Initialize Depth Stencil Buffer
	if(!InitDepthStencilBuffer(width, height))
		return false;

	// Initialize Rasterizer State(s)
	if(!InitRasterizerState(hWnd))
		return false;

	// Initialize Shader and Shader Variables
	if(!InitShader(hWnd))
		return false;

	// Initialize Vertex Layout
	if(!InitVertexLayout())
		return false;

	// Setup the viewport for rendering.
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create viewport
	gpDeviceContext->RSSetViewports(1, &viewport);


	// Create texture list
	textureListSize = texSize;
	textureList = new ID3D11ShaderResourceView*[textureListSize];
	for(unsigned int i = 0; i < texSize; ++i)
		textureList[i] = nullptr;
	return true;
}

bool Direct3D::InitDepthStencilBuffer(int width, int height)
{
	// Initialize and setup descript of depth buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = width;
		depthBufferDesc.Height = height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

	// Create the texture for depth buffer using filled out description
	if(FAILED(gpDevice->CreateTexture2D(&depthBufferDesc, NULL, &gpDepthStencilBuffer)))
		return false;

	// Initialize and set up description of stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	if(FAILED(gpDevice->CreateDepthStencilState(&depthStencilDesc, &gpDepthStencilState)))
		return false;

	// Set depth stencil state
	gpDeviceContext->OMSetDepthStencilState(gpDepthStencilState, 1);
	
	// Initialize and set up the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Creat depth stencil view
	if(FAILED(gpDevice->CreateDepthStencilView(gpDepthStencilBuffer, &depthStencilViewDesc, &gpDepthStencilView)))
		return false;

	// Bind render target view and depth stencil buffer to output render pipeline
	gpDeviceContext->OMSetRenderTargets(1, &gpRenderTargetView, gpDepthStencilView);
	return true;
}

bool Direct3D::InitRasterizerState(HWND hWnd)
{
	// Setup the raster description which will determine how and what polygons will be drawn.
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create rasterizer state(s) from description
	if(FAILED(gpDevice->CreateRasterizerState(&rasterDesc, &gpRasterStateSolid)))
		return false;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	if(FAILED(gpDevice->CreateRasterizerState(&rasterDesc, &gpRasterStateWireframe)))
		return false;

	// Set rasterizer state to solid
	gpDeviceContext->RSSetState(gpRasterStateSolid);
	return true;
}

bool Direct3D::InitShader(HWND hWnd)
{
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* shaderBuffer = 0;
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	
	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	if(FAILED(D3DX11CompileFromFile("../GladeEngine/Shaders/texture.fx", 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &shaderBuffer, &errorMessage, 0)))
	{
		if(errorMessage != 0)
		{
			MessageBox(hWnd, (char*)errorMessage->GetBufferPointer(), 0, 0);
			ReleaseCOM(errorMessage);
		}
		return false;
	}

	if(FAILED(D3DX11CreateEffectFromMemory(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), 0, gpDevice, &gpEffect)))
		return false;
	ReleaseCOM(shaderBuffer);

	// Get Shader effect variables
	sWorldMatrix = gpEffect->GetVariableByName("worldMatrix")->AsMatrix();
	sViewMatrix = gpEffect->GetVariableByName("viewMatrix")->AsMatrix();
	sProjectionMatrix = gpEffect->GetVariableByName("projectionMatrix")->AsMatrix();
	sTexture = gpEffect->GetVariableByName("shaderTexture")->AsShaderResource();
	sTechnique = gpEffect->GetTechniqueByName("TextureTechnique");
	return true;
}

// Describe our Vertex structure to DirectX knows what to do with each component.
bool Direct3D::InitVertexLayout()
{
	// Setup layout of data that goes into shader
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
	{	
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
		0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int numElements = sizeof(vertexDesc) / sizeof(vertexDesc[0]);

	D3DX11_PASS_DESC passDesc;
	sTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	if(FAILED(gpDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &gpLayout)))
		return false;
	
	gpDeviceContext->IASetInputLayout(gpLayout);
	return true;
}

void Direct3D::Shutdown()
{
	if(gpSwapChain)
		gpSwapChain->SetFullscreenState(false, NULL);

	ReleaseCOM(gpRasterStateSolid)
	ReleaseCOM(gpRasterStateWireframe)
	ReleaseCOM(gpDepthStencilView)
	ReleaseCOM(gpDepthStencilState)
	ReleaseCOM(gpDepthStencilBuffer)
	ReleaseCOM(gpRenderTargetView)
	ReleaseCOM(gpDeviceContext)
	ReleaseCOM(gpDevice)
	ReleaseCOM(gpSwapChain)

	for(unsigned int i = 0; i < textureListSize; ++i)
	{
		if(textureList[i] == nullptr) break;
		ReleaseCOM(textureList[i]);
	}
	delete [] textureList;
}

Direct3D::ShaderResource* Direct3D::CreateBuffers(MeshData* meshData, const char* texFilename)
{
	ID3D11Buffer* vb, *ib;

	// Create Vertex buffer description
	D3D11_BUFFER_DESC vBufferDesc;
	vBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vBufferDesc.ByteWidth = sizeof(Vertex) * meshData->vertices.size();
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;
	vBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	vInitData.pSysMem = &meshData->vertices[0];

	// Create Index buffer description
	D3D11_BUFFER_DESC iBufferDesc;
	iBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iBufferDesc.ByteWidth = sizeof(unsigned int) * meshData->indices.size();
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
		
	D3D11_SUBRESOURCE_DATA iInitData;
	iInitData.pSysMem = &meshData->indices[0];

	// Create Vertex/Index buffers
	gpDevice->CreateBuffer(&vBufferDesc, &vInitData, &vb);
	gpDevice->CreateBuffer(&iBufferDesc, &iInitData, &ib);

	// Check if we have already loaded the specified texture
	std::string tex = texFilename;
	if(texFilenameMap.find(tex) == texFilenameMap.end())
	{
		// If not, create the ShaderResourceView for this texture and store it
		texFilenameMap[tex] = texFilenameMap.size();

		// Filled the list, increase the size of it
		if(texFilenameMap.size() == textureListSize)	
		{
			ID3D11ShaderResourceView** temp = new ID3D11ShaderResourceView*[textureListSize*2];
			for(unsigned int i = 0; i < textureListSize; ++i)
				temp[i] = textureList[i];
			for(unsigned int i = textureListSize - 1; i < textureListSize*2; ++i)
				temp[i] = nullptr;
			delete [] textureList;
			textureList = temp;
			textureListSize *= 2;
		}
		
		D3DX11CreateShaderResourceViewFromFile(gpDevice, texFilename, 0, 0, &textureList[texFilenameMap[tex]], 0);
	}

	// Return return info to calling object.
	// This info can be passed back to draw an object
	return new ShaderResource(vb, ib, meshData->vertices.size(), meshData->indices.size(), texFilenameMap[tex]);
}

// Choose which Rasterizer State to use
void Direct3D::SetRasterizerState(RasterState rs)
{
	switch(rs)
	{
		case RasterState::WIREFRAME:
			gpDeviceContext->RSSetState(gpRasterStateWireframe);
			break;
		case RasterState::SOLID:
		default:
			gpDeviceContext->RSSetState(gpRasterStateSolid);
			return;
	}
}

// Choose primitive topology to draw with
void Direct3D::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY top)
{
	gpDeviceContext->IASetPrimitiveTopology(top);
}

void Direct3D::StartFrame(float re, float gr, float bl, float al, Matrix view, Matrix proj)
{
	// Clear back buffer
	float color[4] = {re,gr,bl,al};
	gpDeviceContext->ClearRenderTargetView(gpRenderTargetView, color);

	// Clear depth buffer
	gpDeviceContext->ClearDepthStencilView(gpDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	gpDeviceContext->IASetInputLayout(gpLayout);

	// Update CBuffer variables
	sViewMatrix->SetMatrix((float*)view);
	sProjectionMatrix->SetMatrix((float*)proj);
}

void Direct3D::EndFrame()
{
	// Present back buffer to screen
	if(vsyncEnabled)
		gpSwapChain->Present(1, 0);
	else
		gpSwapChain->Present(0, 0);
}


void Direct3D::Render(ShaderResource* sr, int flag/*=0*/)
{
	AssertMsg(sr->world != nullptr, "Object World Matrix not set for rendering");

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	gpDeviceContext->IASetVertexBuffers(0, 1, &sr->vb, &stride, &offset);
	gpDeviceContext->IASetIndexBuffer(sr->ib, DXGI_FORMAT_R32_UINT, 0);

	sWorldMatrix->SetMatrix((float*)sr->world);
	sTexture->SetResource(textureList[sr->texIndex]);
	sTechnique->GetPassByIndex(0)->Apply(0, gpDeviceContext);

	gpDeviceContext->DrawIndexed(sr->numIndices, 0, 0);
}
void Direct3D::GetVideoCardInfo(char* cardName, int& mem)
{
	strcpy_s(cardName, 128, videoCardDescription);
	mem = videoCardMem;
}