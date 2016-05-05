#ifndef GLADE_DEBUG_DRAW_H
#define GLADE_DEBUG_DRAW_H

#include "GraphicsLocator.h"
#include "../../Math/Vector.h"
#include "Assert.h"
#include <queue>

namespace Glade { 
class DebugDraw
{
public:
	struct Color 
	{ 
		Color(gFloat _r, gFloat _g, gFloat _b, gFloat _a) : 
			r(_r), g(_g), b(_b), a(_a) { }
		gFloat  r, g, b, a; 
	};

	bool InitDebugDraw();
	void ReleaseDebugDraw();

	void Render(Matrix view, Matrix proj);
	void PushLine(Vector p1, Vector p2, Color c);
	void PushTriangle(Vector p1, Vector p2, Vector p3, Color c);
	void PushSquare(Vector p1, Vector p2, Vector p3, Vector p4, Color c);
	void PushBox(Vector center, Vector extents, Color c);

private:
	void	DrawLine();
	void	DrawTriangle();
	void	DrawSquare();
	void	DrawCube();
	void	IncreaseVertexBufferSize();

	std::queue<int>			drawQueue;
	Direct3D*				g;
	ID3D11Device*			dev;
	ID3D11DeviceContext*	devCon;
	ID3D11InputLayout*		gpDebugLayout;
	ID3DX11Effect*			gpDebugEffect;
	ID3DX11EffectTechnique*	sDebugTechnique;
	ID3DX11EffectMatrixVariable* sDebugViewMatrix;
	ID3DX11EffectMatrixVariable* sDebugProjMatrix;

	ID3D11Buffer*			vertexBuffer;
	D3D11_BUFFER_DESC		vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA	vertexData;

	SVertexC*		vertices;
	int				vertexBufferSize;
	int				vertexBufferMaxSize;
	int				vertexBufferCounter;

	bool maintainBetweenDraw;
	bool resetQueue;

};
}	// namespace
#endif	// GLADE_DEBUG_DRAW_H