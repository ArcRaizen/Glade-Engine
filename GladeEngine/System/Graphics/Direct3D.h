#pragma once
#ifndef GLADE_DIRECT3D_H
#define GLADE_DIRECT3D_H

/*
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DX11EffectsD.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "effect11.lib")
*/

#include <D3D11.h>
#include <DirectXMath.h>
#include <D3DX11async.h>
#include "d3dx11effect.h"
#include "../../Math/Matrix.h"
#include "../GeometryGenerator.h"
#include "../../Utils/SmartPointer/WeakPointer.h"
#include <map>
#include "../../Utils/Assert.h"

using namespace DirectX;
namespace Glade {
class DebugDraw;
class Direct3D
{
public:
	struct ShaderResource
	{
		ShaderResource(ID3D11Buffer* v, ID3D11Buffer* i, unsigned int numV, unsigned int numI, unsigned int index) :
			vb(v), ib(i), numVertices(numV), numIndices(numI), texIndex(index), world(nullptr) { }
		ID3D11Buffer* vb;
		ID3D11Buffer* ib;
		unsigned int numVertices, numIndices, texIndex;
		Matrix* world;
		D3DXVECTOR4* color;
	};

	enum class RasterState { SOLID, WIREFRAME };
	friend class DebugDraw;
	friend class FontDraw;

	Direct3D();
	~Direct3D();

	bool Initialize(int width, int height, bool vsync, HWND hWnd, bool fullscreen, float depth, float near, unsigned int texSize=10);
	void Shutdown();

	void StartFrame(float* view, float* proj, float r=0.390625f, float g=0.582031f, float b=0.925781f, float a=1.0f);
	void SetViewProj(float* view, float* proj);
	void EndFrame();

	// ~~~~ Management Functions ~~~~
	ShaderResource* CreateBuffers(SmartPointer<MeshData> meshData);
	void SetRasterizerState(RasterState rs);
	void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY top);
	void EnableZBuffer();
	void DisableZBuffer();
	void CreateNewViewport(float w, float h, float topLeftX, float topLeftY, float minZ, float maxZ);
	void SetViewports(int index=-1);

	// Render an Object from its ShaderResource data
	void Render(ShaderResource* sr, int flag=0);

	void GetVideoCardInfo(char*, int&);

private:
	// ~~~~ Initialization ~~~~
	bool InitDepthStencilBuffer(int width, int height);
	bool InitRasterizerState(HWND hWnd);
	bool InitShader(HWND hWnd);
	bool InitBlendStates();
	bool InitVertexLayout();

	bool	vsyncEnabled;
	int		videoCardMem;
	char	videoCardDescription[128];

	// Keep a list of all textures used
	// Map keeps filenames of textures and the index of those textures in the list 
	unsigned int textureListSize;
	ID3D11ShaderResourceView**	textureList;
	std::map<std::string, int>	texFilenameMap;

	// COM Objects/Pointers
	IDXGISwapChain*				gpSwapChain;
	ID3D11Device*				gpDevice;
	ID3D11DeviceContext*		gpDeviceContext;
	ID3D11RenderTargetView*		gpRenderTargetView;
	ID3D11Texture2D*			gpDepthStencilBuffer;
	ID3D11DepthStencilState*	gpDepthStencilState;
	ID3D11DepthStencilState*	gpDepthDisabledStencilState;
	ID3D11DepthStencilView*		gpDepthStencilView;
	ID3D11RasterizerState*		gpRasterStateSolid;
	ID3D11RasterizerState*		gpRasterStateWireframe;
	ID3D11InputLayout*			gpLayout;
	ID3D11BlendState*			gpBlendNoBlend;
	ID3D11BlendState*			gpBlendAlpha;
	ID3D11BlendState*			gpBlendAdditive;
	ID3D11BlendState*			gpBlendSubtractive;
	D3D11_VIEWPORT*				gpViewports;
	unsigned int				numViewports;

	// Shader stuff
	ID3DX11Effect*							gpEffect;
	ID3DX11EffectMatrixVariable*			sWorldMatrix;
	ID3DX11EffectMatrixVariable*			sViewMatrix;
	ID3DX11EffectMatrixVariable*			sProjectionMatrix;
	ID3DX11EffectShaderResourceVariable*	sTexture;
	ID3DX11EffectVectorVariable*			sHighlight;
	ID3DX11EffectTechnique*					sTechnique;
};
}	// namespace
#endif	// GLADE_DIRECT3D_H

