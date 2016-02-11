//
//matrix gLightWVP;
//
////shadowdepthmapstuff
//float4 VS(float3 posLight : POSITION) : SV_POSITION
//{
//	return mul(float4(posLight, 1.0f), gLightWVP);
//};
//
//BlendState NoBlend
//{
//	BlendEnable[0] = FALSE;
//};
//
//RasterizerState rs
//{
//	FillMode = Solid;
//	CullMode = Front;
//};
//
//DepthStencilState EnableDepth
//{
//	DepthEnable = TRUE;
//	DepthWriteMask = ALL;
//	DepthFunc = LESS_EQUAL;
//};
//technique10 RenderShadowMap
//{
//	pass P0
//	{
//		SetVertexShader(CompileShader(vs_4_0, VS()));
//		SetGeometryShader(NULL);
//		SetPixelShader(NULL);
//		SetDepthStencilState(EnableDepth, 0);
//		SetBlendState(NoBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
//		SetRasterizerState(rs);
//	}
//};

//cbuffer MatrixBuffer : register (b0)
//{
//	matrix worldMatrix;
//	matrix camView;
//	matrix projectionMatrix;
//}

struct VS_IN
{
	float4 pos : POSITION;
	float3 tex : COLOR;
	float3 norm : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	//float3 norm : TEXCOORD1;
	float3 tex : COLOR;
	//float4 posLightH : TEXCOORD3;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	//matrix cMatrix = mul(mul(worldMatrix, camView), projectionMatrix);

	//output.pos = mul(input.pos, cMatrix);
	output.pos = input.pos;
	//output.norm = mul(input.norm, worldMatrix);
	output.tex = input.tex;
	//output.posLightH = mul(input.pos, gLightWVP);

	return output;
}