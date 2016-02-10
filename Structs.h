#ifndef STRUCTS_SHADER
#define STRUCTS_SHADER
#include <DirectXMath.h>
struct MatrixBuffer {
	DirectX::XMMATRIX World;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX Projection;
	DirectX::XMMATRIX LightView;
	DirectX::XMMATRIX LightProjection;
};
struct TriangleVertex
{
	float x, y, z;
	float r, g, b;
};
struct LightBufferType
{
	DirectX::XMVECTOR ambientColor;
	DirectX::XMVECTOR diffuseColor;
};

struct LightBufferType2
{
	DirectX::XMVECTOR lightPosition;
	float padding;
};

#endif