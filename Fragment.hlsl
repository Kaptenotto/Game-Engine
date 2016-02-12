Texture2D txDiffuse : register (t0);
Texture2D depthMapTexture : register(t1);
SamplerState sampAni;
SamplerState SampleTypeClamp : register(s1)
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

cbuffer Lights : register (b1)
{
	float3 dir;
	float3 position;
	float4 ambient;
	float4 diffuse;
}


struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 normal : NORMAL;
	float4 lightPos : POSITION;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;

	bias = 0.001f;
	color = ambient;
	projectTexCoord.x = input.lightPos.x / input.lightPos.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightPos.y / input.lightPos.w / 2.0f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord.r);
		lightDepthValue = input.lightPos.z / input.lightPos.w;
		lightDepthValue = lightDepthValue - bias;

		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.normal, input.lightPos)); //kan behöva ny tex (lightpos sepparat från lightpos
			if (lightIntensity > 0.0f)
			{
				color += (diffuse * lightIntensity);
				color = saturate(color);
			}
		}
	}

	textureColor = depthMapTexture.Sample(sampAni, input.uvs);
	color = color * textureColor;

	return color;


	//Detta kan fucka up om det inte finns en textur. 
	//"Vi bränner den bron när vi kommer till den" - Jesus, 2012
	//float4 s = txDiffuse.Sample(sampAni, input.uvs);
	//return s;
	
	
};