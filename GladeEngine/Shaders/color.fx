//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

cbuffer CBPerFrame
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

struct VertexInputType
{
	float4 position   : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position  : SV_POSITION;
    float4 color : COLOR;
};

PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;
	
	// Just pass vertex color into the pixel shader.
	input.position.w = 1.0f;
	output.position = mul(input.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	//output.position = input.position;
    output.color = input.color;
    
    return output;
}

float4 ColorPixelShader(PixelInputType input) : SV_Target
{
    return input.color;
}

technique10 ColorTechnique
{
    pass pass0
    {
        SetVertexShader( CompileShader( vs_5_0, ColorVertexShader() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, ColorPixelShader() ) );
    }
}
