#include "PCH.h"
#include "PSComponent.h"

Aen::ParticleSystemComponent::ParticleSystemComponent()
{
	this->m_maxParticles = 0;
	this->m_vertexBuffer = 0;
	this->m_SRVBuffer = 0;
	this->cRuntimeBuffer = 0;
	this->m_outputSRV = 0;
	this->m_UAV = 0;
	this->m_systemPos = { 0.0f,40.0f,0.0f };
}

Aen::ParticleSystemComponent::~ParticleSystemComponent()
{
}

bool Aen::ParticleSystemComponent::Initialize(ID3D11Device*& device, std::wstring textureFilename)
{
	bool result;
	result = LoadTexture(device,textureFilename);
	if (!result)
	{
		std::cout << "Failed initialize Texture" << std::endl;
		return false;
	}
	result = InitializeParticleSystem();
	if (!result)
	{
		std::cout << "Failed to initialize particlesystem" << std::endl;
		return false;
	}
	result = InitializeBuffers(device);
	if (!result)
	{
		std::cout << "Failed to initialize buffers" << std::endl;
		return false;
	}
	return result;
}

void Aen::ParticleSystemComponent::Shutdown()
{
	ShutdownBuffers();
	ShutdownParticleSystem();
	return;
}

bool Aen::ParticleSystemComponent::Frame(float frameTime, ID3D11DeviceContext*& deviceContext)
{
	EmitParticles(frameTime);
	return true;
}

void Aen::ParticleSystemComponent::Render(ID3D11DeviceContext*& deviceContext)
{
	RenderBuffers(deviceContext);
	return;
}

void Aen::ParticleSystemComponent::Move(float x, float y, float z)
{
	this->m_systemPos.x += x;
	this->m_systemPos.y += x;
	this->m_systemPos.z += x;
	if (this->m_systemPos.y >= 30)
	{
		this->m_systemPos.y = 0.0f;
	}
}

int Aen::ParticleSystemComponent::GetVertexCount()
{
	return this->m_vertexCount;
}

int Aen::ParticleSystemComponent::GetParticleCount()
{
	return this->m_currentParticleCount;
}

int Aen::ParticleSystemComponent::GetMaxParticleCount()
{
	return this->m_maxParticles;
}

float Aen::ParticleSystemComponent::GetRunTime()
{
	return this->m_runTime;
}

bool Aen::ParticleSystemComponent::UpdateBuffers(ID3D11DeviceContext*& deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource{};
	HRESULT result;
	result = deviceContext->Map(this->m_directOutputBuffer, 0, D3D11_MAP_READ_WRITE, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	m_Particle* verticesPtr;

	verticesPtr = (m_Particle*)mappedResource.pData;
	memcpy(verticesPtr, (void*)this->m_particleList, sizeof(m_Particle) * this->m_maxParticles);
	deviceContext->Unmap(this->m_directOutputBuffer, 0);
	return true;
}

ID3D11Buffer* Aen::ParticleSystemComponent::GetConstantRunTimeBufferPtr() const
{
	return this->cRuntimeBuffer;
}

ID3D11Buffer* const* Aen::ParticleSystemComponent::GetConstantRunTimeBufferReference() const
{
	return &this->cRuntimeBuffer;
}

ID3D11ShaderResourceView* const* Aen::ParticleSystemComponent::GetOutputSRV() const
{
	return &this->m_outputSRV;
}

ID3D11UnorderedAccessView* const* Aen::ParticleSystemComponent::GetOutputUAV() const
{
	return &this->m_UAV;
}

bool Aen::ParticleSystemComponent::LoadTexture(ID3D11Device*& device, std::wstring filename)
{
	return false;
}

bool Aen::ParticleSystemComponent::InitializeParticleSystem()
{
	int i;
	this->m_particleDeviationX = 100.0f;
	this->m_particleDeviationY = 10.0f;
	this->m_particleDeviationZ = 100.0f;
	this->m_particleVelocity = 10.0f;
	this->m_particleVelocityVariation = 1.0f;
	this->m_particleSize = 0.12f;
	this->m_particlesPerSecond = 2500.0f;
	this->m_maxParticles = 250;
	this->m_particleList = new m_Particle[m_maxParticles];
	if (!m_particleList)
	{
		return false;
	}
	//Initalizing all particles at start
	for (i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].m_alive = true;
	}
	this->m_currentParticleCount = 0;
	this->m_accumulatedTime = 0.0f;
	return true;
}

void Aen::ParticleSystemComponent::ShutdownParticleSystem()
{
	if (m_particleList)
	{
		delete[] m_particleList;
		m_particleList = 0;
	}
	return;
}

bool Aen::ParticleSystemComponent::InitializeBuffers(ID3D11Device*& device)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vbd, indexBufferDesc, dummy;
	D3D11_SUBRESOURCE_DATA vd, indexData;
	HRESULT result;
	this->m_vertexCount = this->m_maxParticles;


	//____________________ Direct Output & Output UAV & Output SRV ____________________//
	D3D11_BUFFER_DESC dobd = { 0 };
	dobd.ByteWidth = sizeof(m_Particle) * this->m_maxParticles;
	dobd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	dobd.StructureByteStride = sizeof(m_Particle);
	dobd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	dobd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = this->m_particleList;
	result = device->CreateBuffer(&dobd, 0, &this->m_directOutputBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//UAV going to CS
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
	uavd.Format = DXGI_FORMAT_UNKNOWN;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavd.Buffer.FirstElement = 0;
	uavd.Buffer.Flags = 0;
	uavd.Buffer.NumElements = this->m_maxParticles;

	result = device->CreateUnorderedAccessView(this->m_directOutputBuffer, &uavd, &this->m_UAV);
	if (FAILED(result))
	{
		return false;
	}

	//SRV going to VS
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	//ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = this->m_maxParticles;

	result = device->CreateShaderResourceView(this->m_directOutputBuffer, &desc, &this->m_outputSRV);
	if (FAILED(result))
	{
		return false;
	}


	//____________________ Constant Buffer ____________________//
	D3D11_BUFFER_DESC cb = { 0 };
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(CSInputBuffer);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	result = device->CreateBuffer(&cb, NULL, &this->cRuntimeBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void Aen::ParticleSystemComponent::ShutdownBuffers()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	return;
}

void Aen::ParticleSystemComponent::EmitParticles(float frameTime)
{
	this->m_runTime += frameTime;
	bool emitParticle, found;
	emitParticle = false;
	float velocity;
	float pos[3];
	float color[3];
	int index, i, j;
	this->m_accumulatedTime += frameTime;
	if (this->m_accumulatedTime > 1.0f / this->m_particlesPerSecond)
	{
		emitParticle = true;
		this->m_accumulatedTime = 0.0f;
	}
	if (emitParticle && this->m_currentParticleCount < (this->m_maxParticles - 1))
	{
		this->m_currentParticleCount++;
		DirectX::XMFLOAT3 randPos
		{
			(((float)rand() - (float)rand()) / RAND_MAX) * this->m_particleDeviationX,
			(((float)rand() - (float)rand()) / RAND_MAX) * this->m_particleDeviationY,
			(((float)rand() - (float)rand()) / RAND_MAX) * this->m_particleDeviationZ
		};
		DirectX::XMFLOAT3 randColour
		{
			(((float)rand() - (float)rand()) / RAND_MAX) + 0.5f,
			(((float)rand() - (float)rand()) / RAND_MAX) + 0.5f,
			(((float)rand() - (float)rand()) / RAND_MAX) + 0.5f
		};


		float randVelocity = this->m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * this->m_particleVelocityVariation;
		index = 0;
		//m_particleList[this->m_currentParticleCount].position[0] = randPos.x + this->m_systemPos.x;
		//m_particleList[this->m_currentParticleCount].position[1] = randPos.y + this->m_systemPos.y;
		//m_particleList[this->m_currentParticleCount].position[2] = randPos.z + this->m_systemPos.z;
		//m_particleList[this->m_currentParticleCount].m_velocity = randVelocity;
		//m_particleList[this->m_currentParticleCount].color[0] = randColour.x;
		//m_particleList[this->m_currentParticleCount].color[1] = randColour.y;
		//m_particleList[this->m_currentParticleCount].color[2] = randColour.z;
		//m_particleList[this->m_currentParticleCount].m_color[3] = 1.0f;
		m_particleList[this->m_currentParticleCount].m_alive = true;
		found = false;
	}
	return;
}

void Aen::ParticleSystemComponent::UpdateParticles(float frameTime)
{
	int i;
	for (i = 0; i < m_currentParticleCount; i++)
	{
		this->m_particleList[i].m_pos[1] = (m_particleList[i].m_pos[1] - m_particleList[i].m_velocity * frameTime);
	}
	return;
}

void Aen::ParticleSystemComponent::RenderBuffers(ID3D11DeviceContext*& deviceContext)
{
	UpdateBuffers(deviceContext);
	unsigned int stride = 0;
	unsigned int offset = 0;
	ID3D11Buffer* unbound[] = { NULL };
	deviceContext->IASetVertexBuffers(4, 1, unbound, &stride, &offset);
	return;
}
