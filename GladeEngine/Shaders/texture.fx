////////////////////////////////////////////////////////////////////////////////
// Filename: texture.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer CBPerObject
{
	float4x4 worldMatrix;
};

cbuffer CBPerFrame
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	
	// ignore below for now...
	float3 lightDirection;
	float3 lightPosition;
	float4 lightColor;
};

Texture2D shaderTexture;
float4 highlightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
	
	// Store the highlight color for the pixel shader.
	output.color = input.color;
    
	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TexturePixelShader(PixelInputType input) : SV_Target
{
	if(input.tex.x != -1)
	{
		float4 textureColor = shaderTexture.Sample(SampleType, input.tex);
		return textureColor * input.color;
	}
	
	return input.color;
}


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 TextureTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, TextureVertexShader()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TexturePixelShader()));
	}
}