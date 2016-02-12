
matrix gLightWVP;


float4 VS(float3 posLight : POSITION) : SV_POSITION
{
	return mul(float4(posLight, 1.0f), gLightWVP);
};

BlendState NoBlend
{
	BlendEnable[0] = FALSE;
};

RasterizerState rs
{
	FillMode = Solid;
	CullMode = Front;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};
technique10 RenderShadowMap
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(rs);
	}
};
cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
}

struct VS_IN
{
	float3 pos : POSITION;
	float3 Color : COLOR;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 Color : COLOR;
	float4 lightPos : TEXCOORD3;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = float4(input.pos, 1);
	output.Color = input.Color;
	output.lightPos = mul(input.pos, gLightWVP);

	return output;
}