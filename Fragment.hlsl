Texture2D txDiffuse : register (t0); // för att lägga till fler texturer txDiffuse[x]
SamplerState sampAni;

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 normal : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	
	float3 s = txDiffuse.Sample(sampAni, input.uvs).xyz;
	float4 normal;
	normal = normalize(input.normal);
	return float4(input.uvs,1.0f, 1.0f);
};


/* Normal/tangent calculations for the pixelshader.


Texture2D txDiffuse : register(t0);
SamplerState sampAni;

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	float4 lightPos : POSITION_LIGHT;
};

//float4 PS_main(PS_IN input) : SV_TARGET
//{
//	float3 lightInt;
//	float3 finalColor;
//	float4 diffuseColor = (1.0, 1.0, 1.0, 1.0);
//
//	float3 s = txDiffuse.Sample(sampAni, input.Tex).xyz;
//	lightInt = saturate(dot(input.Normal, input.lightPos));
//	finalColor = saturate(diffuseColor * lightInt);
//	finalColor = finalColor * s;
//
//	return float4(finalColor, 1.0);
//
//}
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);

//set diffuse color of material
float4 diffuse = difColor;

//if material has a normal map, we can set it now
if (hasTexture == true)
{
	diffuse = ObjTexture.Sample(ObjSamplerState, input.TexCoord);

}
if (hasNormMap == true)
{
	float4 normalMap = ObjNormMap.Sample(ObjSamplerState, input.TexCoord);
	normalMap = (2.of*normalMap) - 1.0f;

	input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);
	float3 biTangent = cross(input.normal, input.tangent);

	float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

	input.normal = normalize(mul(normalMap, texSpace));
}

	float3 finalColor;
	finalColor = diffuse * light.ambient;	
	finalColor += saturate(dot(light.dir, input.normal)*light.diffuse*diffuse);
	return float4(finalColor, diffuse.a);

}*/ // SHIT STRUDLE