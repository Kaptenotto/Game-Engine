Texture2D diffuseTex	 : register(t0);
Texture2D normalTex		 : register(t1);
Texture2D shadowTex		 : register(t2);

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

struct GBUFFER_VS_IN
{
	float3 pos : POSITION;
	float2 uvs : TEXCOORD;
	float3 norm : NORMAL;
};
struct GBUFFER_VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 wPos : WPOS;
};
struct GBUFFER_GS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 wPos : WPOS;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};
struct GBUFFER_PS_OUT
{
	float4 diffuseRes	: SV_Target0;
	float4 normalRes	: SV_Target1;
	float4 positionRes	: SV_Target2;
	float4 shadowRes	: SV_Target3;
};
//VERTEX
GBUFFER_VS_OUT GBUFFER_VS_main(GBUFFER_VS_IN input)
{
	// multiply with the vartious matrices, store wPos before camview and projection to get an accurate wPos for each vert.
	GBUFFER_VS_OUT output = (GBUFFER_VS_OUT)0;

	output.uvs = input.uvs;
	output.norm = float4(input.norm, 0);
	output.pos = float4(input.pos, 1);

	output.pos = mul(output.pos, worldMatrix);
	output.wPos = output.pos;
	output.pos = mul(output.pos, camView);
	output.pos = mul(output.pos, projectionMatrix);

	return output;
}

//GEOMETRY
[maxvertexcount(6)]
void GBUFFER_GS_main(
	triangle GBUFFER_VS_OUT input[3],
	inout TriangleStream <GBUFFER_GS_OUT> TriStream)
{
	GBUFFER_GS_OUT output = (GBUFFER_GS_OUT)0;

	//Normal calculations. generate a normal for each triangle passing the geo shader
	//we use wPos here so that the calculations of the normal/binormal/tangent are not affected by the position and angle of the users camera in real-time.
	
	//Calculate faceedge a and b which are vectors.
	float3 faceEdgeA = input[1].wPos - input[0].wPos;
	float3 faceEdgeB = input[2].wPos - input[0].wPos;

	// Same as previousbut for the uvedges
	float2 uvEdge1 = input[1].uvs - input[0].uvs;
	float2 uvEdge2 = input[2].uvs - input[0].uvs;

	//We invert the cross product since our normals are inverted
	//cross between two vectors = new vector in this case normal
	float3 normal = normalize(-cross(faceEdgeA, faceEdgeB));

	// Multiplying vector with vector minus vector with vector * vector with vector - vector with vector
	// We do this to get our tangent vector.
	float3 tangent = (uvEdge2[1] * faceEdgeA - uvEdge1[1] * faceEdgeB)*(1 / (uvEdge1[0] * uvEdge2[1] - uvEdge2[0] * uvEdge1[1]));
	tangent = normalize(tangent);

	//Same as previous but with normal and tangent to get a binormal
	float3 binormal = normalize(-cross(normal, tangent));

	tangent = mul(float4(tangent, 1), worldMatrix).xyz;
	binormal = mul(float4(binormal, 1), worldMatrix).xyz;

	for (int i = 0; i < 3; i++)
	{
		// calculate position to get into projection space so we
		// can get our direction needed to be able to check if the face is back or frontfaced to cam.
		float3 pos = mul(worldMatrix, input[i].pos);
		pos = mul(camView, pos);
		pos = mul(projectionMatrix, pos);

		//Calculate our direction of the cam normally dont by camPos - pos
		float4 direction = normalize(float4(0, 0, 0, 0) - float4(pos, 1));

		// we do the dot product to get a scalar value to see if the direction is a negative or positive
		// one of the face, which gives the result if its backfaced or not.
		if (dot(direction, normal) <= 0) // change < to invert the direction of the backface culling
		{
			//if the check passes it means the normal of the triangle in question is facing towards the camera and should not be culled
			output.pos = input[i].pos;
			output.uvs = input[i].uvs;
			// We do this calculation to get per vertice normal in the world.
			output.norm = float4(mul(input[i].norm.xyz, (float3x3)worldMatrix), 0);
			output.tangent = tangent;
			output.binormal = binormal;
			output.wPos = input[i].wPos;

			TriStream.Append(output);
		}
		else
		{
			//triangle in question should be culled
		}
	}
}

//PIXEL
GBUFFER_PS_OUT GBUFFER_PS_main(GBUFFER_GS_OUT input)
{
	GBUFFER_PS_OUT output = (GBUFFER_PS_OUT)0;

	float4 texColor;
	float4 norMap;
	float3 Normal;
	float3 lightDirection;
	float4 norColor;

	float bias = 0.00175f;;
	float4 color = ambient;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;
	float4 lightPos;
	float SMAP_SIZE = 2048.0f;

	//sampla diffuse texturen, kan sparas direkt.
	output.diffuseRes = diffuseTex.Sample(SampleTypeClamp, input.uvs);
	
	
	//NORMAL BLOCK

	//sampla normaltexturen från objn
	norMap = normalTex.Sample(SampleTypeClamp, input.uvs);

	//av normalizera värderna för att byta range från 0,1 till -1,1
	norMap = (norMap*2.0f) - 1.0f;

	// Gör z värdet negativt eftersom våra normaler tidigare behövdes inverteras
	norMap.z = (norMap.z * -1);

	//influera de förberäknade normal/binormal tangent värderna med hjälp av normal mappen
	Normal = (norMap.x * input.tangent) + (norMap.y * input.binormal) + (norMap.z * input.norm);
	Normal = normalize(Normal);

	//lightDirection = (float4(-lightDir, 1) - input.wPos);
	//
	//lightIntensity = saturate(dot(Normal, lightDirection));
	//norColor = saturate(diffuse * lightIntensity);

	output.normalRes = float4(Normal,0);

	//SHADOW BLOCK

	//projecering av koordinater
	lightPos = mul(input.wPos, view);
	lightPos = mul(lightPos, projection);

	projectTexCoord.x = lightPos.x / lightPos.w;
	projectTexCoord.y = lightPos.y / lightPos.w;

	lightDepthValue = lightPos.z / lightPos.w;

	//homogenisera the projecerade koordinaterna
	projectTexCoord.x = projectTexCoord.x * 0.5f + 0.5f;
	projectTexCoord.y = projectTexCoord.y * -0.5f + 0.5f;

	//samplar depth stencilen som shadow passet gjort
	depthValue = shadowTex.Sample(SampleTypeClamp, projectTexCoord.xy).r + bias;


	//2x2 point sampling
	float dx = 1.0f / SMAP_SIZE;
	float s0 = (shadowTex.Sample(SampleTypeClamp, projectTexCoord).r + bias < lightDepthValue) ? 0.0f : 1.0f;
	float s1 = (shadowTex.Sample(SampleTypeClamp, projectTexCoord + float2(dx, 0.0f)).r + bias < lightDepthValue) ? 0.0f : 1.0f;
	float s2 = (shadowTex.Sample(SampleTypeClamp, projectTexCoord + float2(0.0f, dx)).r + bias < lightDepthValue) ? 0.0f : 1.0f;
	float s3 = (shadowTex.Sample(SampleTypeClamp, projectTexCoord + float2(dx, dx)).r + bias < lightDepthValue) ? 0.0f : 1.0f;

	//ändra till texel space
	float2 texelpos = projectTexCoord * SMAP_SIZE;

	//linjär interpolering för att få fram cooefficienten för denna pixel
	float2 lerps = frac(texelpos);
	float shadowcooef = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

	//spara cooefficienten så den kan användas i final pass
	output.shadowRes.a = shadowcooef;
	//spara wpos på samma sätt
	output.positionRes = input.wPos;
	//spara depth då den kan vara bra att ha ibland
	float depth = input.pos.z / input.pos.w;
	output.shadowRes.x = depth;

	return output;
}
