#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	m_particleList = 0;
	m_vertices = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

ParticleSystem::ParticleSystem(const ParticleSystem& other)
{

}
ParticleSystem::~ParticleSystem()
{
}

bool ParticleSystem::initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	result = InitializeParticleSystem();
	if (!result)
	{
		return false;
	}

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;

}

void ParticleSystem::Shutdown()
{
	ShutdownBuffers();

	ShutdownParticleSystem();

	ReleaseTexture();

	return;
}

bool ParticleSystem::Frame(float frameTime, ID3D11DeviceContext* deviceContext)
{
	bool result;

	KillParticles();

	EmitParticles(frameTime);

	UpdateParticles(frameTime);

	result = UpdateBuffers(deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystem::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

ID3D11ShaderResourceView* ParticleSystem::GetTexture()
{
	// IMPROVISE

}

int ParticleSystem::GetIndexCount()
{
	return m_indexCount;
}

bool ParticleSystem::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	//improvise
}

void ParticleSystem::ReleaseTexture()
{

}

bool ParticleSystem::InitializeParticleSystem()
{
	int i;
	m_particleDeviationX = 0.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.0f;

	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	m_particleSize = 0.2f;

	m_particlesPerSecond = 250.0f;

	m_maxParticles = 5000;

	
	m_particleList = new ParticleType[m_maxParticles];
	if (!m_particleList)
	{
		return false;
	}

	for (i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	m_currentParticleCount = 0;

	m_accumulatedTime = 0.0f;

	return true;
}

void ParticleSystem::ShutdownParticleSystem()
{
	if (m_particleList)
	{
		delete[] m_particleList;
		m_particleList = 0;
	}

	return;
}

bool ParticleSystem::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT hr;

	m_vertexCount = m_maxParticles * 6;

	m_indexCount = m_vertexCount;

	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}
	
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;


	hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	delete[] indices;
	indices = 0;

	return true;

}

void ParticleSystem::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ParticleSystem::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;

	m_accumulatedTime += frameTime;

	emitParticle = false;

	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		//generate randomized particle properties.
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) *m_particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
	}
}