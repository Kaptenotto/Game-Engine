//matrix gLightWVP;
//
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
cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
}

cbuffer Lights : register (b1)
{
	float3 dir;
	matrix position;
	matrix projection;
	float4 ambient;
	float4 diffuse;
}

struct VS_IN
{
	float3 pos : POSITION;
	float2 uvs : TEXCOORD;
	float3 norm : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 lightPos : POSITION;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = float4(input.pos, 1);
	//output.Color = float3 (1.0f,1.0f,1.0f);

	output.uvs = input.uvs;
	output.norm = float4(input.norm,1);
	output.lightPos = position; //COORDS FOR LIGHT IN WORLD SPACE HERE?

	return output;
}