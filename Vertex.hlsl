
matrix gLightWVP

//shadowdepthmapstuff
float4 VS(float3 posL : POSITION) : SV_POSITION
{
	return mul(float4(posLight, 1.0f), gLightWVP);
}

BlendState NoBlend
{
	BlendEnable[0] = FALSE;
};

RasterizerState rs
{
	FillMode = Solid;
	CullMode = Front;
};
DepthStencilState EnableDepth;
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
		SetRaserizerState(rs);
	};
};

cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
}

struct VS_IN
{
	float4 pos : POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD0;
};

struct VS_OUT
{
	float3 pos : POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD0;
	float4 posLightH : TEXCOORD3;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	matrix cMatrix = mul(mul(worldMatrix, ViewMatrix), projectionMatrix);

	output.pos = mul(input.pos, cMatrix);
	output.normal = mul(input.normal, WorldMatrix);
	output.tex = input.tex;
	output.posLightH = mul(input.pos, LightWVP);

	return output;
}