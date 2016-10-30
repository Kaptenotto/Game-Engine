Texture2D	diffuseTexture			 : register(t0);
Texture2D	normalTexture			 : register(t1);
Texture2D	positionTexture			 : register(t2);
Texture2D	shadowTex				 : register(t3);

SamplerState	 linearSampler		 : register(s0);
SamplerState	 pointSampler		 : register(s1);

cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
	float4 camPos;

}

cbuffer Lights : register (b1)
{
	float4 position;
	matrix view;
	matrix projection;
	float4 ambient;
	float4 diffuse;
	float3 lightDir;
}

struct VS_IN
{
	float3 pos			 : POSITION;
	float2 uvs			 : TEXCOORD0;
};

struct VS_OUT
{

	float4 pos			 : SV_POSITION;
	float2 uvs			 : TEXCOORD0;
};
//VERTEX
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;



	output.pos = float4(input.pos, 1.0f);
	output.uvs = input.uvs;

	return output;
}
//PIXEL
float4 PS_main(VS_OUT input) : SV_TARGET
{
	
	float4 finalColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 diffuseSamp = diffuseTexture.Sample(pointSampler,input.uvs);
	//return diffuseSamp;

	float3 worldPos = positionTexture.Sample(pointSampler, input.uvs).xyz;
	//return float4(worldPos,1);
	
	//NORMAL BLOCK
	float4 normalSamp = normalTexture.Sample(pointSampler, input.uvs);
	//return normalSamp;

	//LIGHT CALC
	float3 lightDirection = (float4(-lightDir, 1) - worldPos);

	float lightIntensity = saturate(dot(normalSamp, lightDirection));
	float4 norColor = saturate(diffuse * lightIntensity);

	//SHADOW BLOCK
	float4 shadowSamp = shadowTex.Sample(pointSampler, input.uvs);
	//return float4(shadowSamp.a, 0, 0, 1);
	//return float4(shadowSamp.x, 0, 0, 1);

	finalColor = diffuseSamp * (norColor * 0.5f) + diffuseSamp * ambient * shadowSamp.a;
	finalColor = saturate(finalColor);
	return finalColor;
}