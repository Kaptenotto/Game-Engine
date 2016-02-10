Texture2D shaderTexture[2];
SamplerState SampleType;

cbuffer lightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
};
cbuffer lightBuffer2
{
	float3 lightPosition;
	float padding;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};


float4 main() : SV_TARGET
{
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);

<<<<<<< HEAD
	float4 textureColor;
	float4 bumpMap;
	float3 bumpNormal;
	float3 lightDir;
	float lightIntesity;
	float4 color;

	textureColor = shaderTextures[0].Sample(SampleType, input.tex);
	bumpMap = shaderTextures[1].Sample(Sampletype, input.tex);
	bumpMa = (bumpMap * 2.0f) - 1.0f;

	bumpNormal = (bumpMap.x *input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z*input.Normal);

	bumpNormal = normalize(bumpNormal);

	lightDir = -lightDirection;
	lightIntesity = saturate(dot(bumpNormal, lightDir));
	color = saturate(diffuseColor*lightIntensity);
	color = color*textureColor;

	return color;
=======

>>>>>>> origin/Normal/texture-Map
}