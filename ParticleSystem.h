#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

class ParticleSystem
{

private:
	struct ParticleType
	{
		float positionX, positionY, positionZ;
		float red, green, blue;
		float velocity;
		bool active;
	};
public:
	ParticleSystem();
	~ParticleSystem();
	ParticleSystem(const ParticleSystem&);

	bool initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	bool Frame(float, ID3D11DeviceContext*);
	void Render(ID3D11DeviceContext*);
	int GetIndexCount();

private:
	XMFLOAT4 particleColor{ 0.5f,0.5f,0.5f,0.5f };
	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();

	bool UpdateBuffers(ID3D11DeviceContext*);
	
	void RenderBuffers(ID3D11DeviceContext*);

	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;

	int m_currentParticleCount;
	float m_accumulatedTime;

	

	ParticleType* m_particleList;

	int m_vertexCount, m_indexCount;
	VertexType* m_vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

};

#endif // !PARTICLESYSTEM_H


