Texture2D txDiffuse : register (t0);
Texture2D depthMapTexture : register(t1);
SamplerState sampAni;
SamplerState SampleTypeClamp : register(s0)
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};
cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
}
cbuffer Lights : register (b1)
{
	float4 position;
	matrix view;
	matrix projection;
	float4 ambient;
	float4 diffuse;
}

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 wPos : WPOS;
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
	float4 lightPos;

	bias = 0.00001f;
	color = ambient;

	//matrix cMatrix = mul(view, projection);
	lightPos = mul(input.wPos, view);
	lightPos = mul(lightPos, projection);

	projectTexCoord.x = lightPos.x / lightPos.w;
	projectTexCoord.y = lightPos.y / lightPos.w;
	
	lightDepthValue = lightPos.z / lightPos.w;
	
	projectTexCoord.x = projectTexCoord.x * 0.5f + 0.5f;
	projectTexCoord.y = projectTexCoord.y * -0.5f + 0.5f;

	depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord.xy).r + bias;

	if(depthValue < lightDepthValue)

	{
		lightIntensity = saturate(dot(input.norm, position)); //kan behöva ny tex (lightpos sepparat från lightpos
		if (lightIntensity > 0.0f)
		{
			color += (diffuse * lightIntensity);
			color = saturate(color);
		}
	}

	//color += float4(.0f, .0f, 1.0f, 1.f);


	textureColor = txDiffuse.Sample(SampleTypeClamp, input.uvs);
	color = color * textureColor;

	return color;


	//Detta kan fucka up om det inte finns en textur. 
	//"Vi bränner den bron när vi kommer till den" - Jesus, 2012
	//float4 s = txDiffuse.Sample(sampAni, input.uvs);
	//return s;
	
	
};