#ifndef GLADE_FONT_DRAW_H
#define GLADE_FONT_DRAW_H

#include "GraphicsLocator.h"
#include <fstream>
#include <map>

namespace Glade  {
class FontDraw
{
public:
	// Indexing data for fonts
	struct FontType
	{
		float left, right;
		int size;
	};

	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	bool InitFontDraw(HWND hWnd, int screenWidth, int screenHeight);
	void Shutdown();

	void	Render(Matrix view, Matrix proj);
	void	PushSentence(std::string name, char* text, int posX, int posY, float r, float g, float b);
	void	PopSentence(std::string name);

private:
	bool	InitializeSentence(std::string name, int maxLength);
	void	BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY);
	bool	LoadFontData(char* fontIndexFilename, char* fontTextureFilename);
	void	ReleaseFontData();

	Direct3D*				g;
	ID3D11Device*			dev;
	ID3D11DeviceContext*	devCon;
	ID3D11InputLayout*		gpFontLayout;
	ID3DX11Effect*			gpFontEffect;
	ID3DX11EffectTechnique*	sFontTechnique;
	ID3DX11EffectMatrixVariable*			sFontViewMatrix;
	ID3DX11EffectMatrixVariable*			sFontProjMatrix;
	ID3DX11EffectShaderResourceVariable*	sTexture;
	ID3DX11EffectVectorVariable*			sColor;
	int width, height;
			
	FontType*					font;
	ID3D11ShaderResourceView*	fontTexture;
	std::map<std::string, SentenceType*> sentences;
};
}	// namespace
#endif	// GLADE_FONT_DRAW_H
