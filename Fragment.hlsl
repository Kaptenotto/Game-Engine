Texture2D txDiffuse : register (t0);
Texture2D depthMapTexture : register(t1);
Texture2D shaderTexture : register(t2);
SamplerState SampleTypeClamp : register(s0);

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

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 wPos : WPOS;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	//Normal stuff
	float4 texColor;
	float4 norMap;
	float3 Normal;
	float3 lightDirection;
	float4 norColor;

	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;
	float4 lightPos;
	float SMAP_SIZE = 2048.0f;
	color = ambient;

	textureColor = txDiffuse.Sample(SampleTypeClamp, input.uvs);
	norMap = shaderTexture.Sample(SampleTypeClamp, input.uvs);

	norMap = (norMap*2.0f) - 1.0f;

	norMap.z = (norMap.z * -1);

	Normal = (norMap.x * input.tangent) + (norMap.y * input.binormal) + (norMap.z * input.norm);
	Normal = normalize(Normal);

	lightDirection = (float4(-lightDir, 1) - input.wPos);

	lightIntensity  = saturate(dot(Normal, lightDirection));
	norColor = saturate(diffuse * lightIntensity);

	//norColor = norColor * color;
	//norColor = norColor * texureColor;

	//return float4(input.norm.xyz, 1.0f);

	bias = 0.00175f;

	lightPos = mul(input.wPos, view);
	lightPos = mul(lightPos, projection);

	projectTexCoord.x = lightPos.x / lightPos.w;
	projectTexCoord.y = lightPos.y / lightPos.w;
	
	lightDepthValue = lightPos.z / lightPos.w;
	
	projectTexCoord.x = projectTexCoord.x * 0.5f + 0.5f;
	projectTexCoord.y = projectTexCoord.y * -0.5f + 0.5f;

	depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord.xy).r + bias;

	

	float dx = 1.0f / SMAP_SIZE;
	float s0 = (depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r + bias < lightDepthValue) ? 0.0f : 1.0f;
	float s1 = (depthMapTexture.Sample(SampleTypeClamp, projectTexCoord + float2(dx, 0.0f)).r + bias < lightDepthValue) ? 0.0f : 1.0f;
	float s2 = (depthMapTexture.Sample(SampleTypeClamp, projectTexCoord + float2(0.0f, dx)).r + bias < lightDepthValue) ? 0.0f : 1.0f;
	float s3 = (depthMapTexture.Sample(SampleTypeClamp, projectTexCoord + float2(dx, dx)).r + bias < lightDepthValue) ? 0.0f : 1.0f;

	float2 texelpos = projectTexCoord * SMAP_SIZE;
	float2 lerps = frac(texelpos);
	float shadowcooef = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

	textureColor = textureColor * shadowcooef + textureColor * norColor;

	textureColor = textureColor * color;
	//norColor = norColor * textureColor;
	//textureColor = saturate(textureColor + norColor);
	return textureColor;
};
