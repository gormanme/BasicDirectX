#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
    m_Texture = 0;
	m_model = 0;
}


ModelClass::ModelClass(const ModelClass& /*other*/)
{

}


ModelClass::~ModelClass()
{

}


bool ModelClass::Initialize(ID3D11Device *device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename)
{
	bool result = true;

	//Load in the model data
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	//Init vertex and index buffers
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	
    //Load the texture for this model
    result = LoadTexture(device, deviceContext, textureFilename);
    if (!result)
    {
        return false;
    }

	return true;
}


void ModelClass::Shutdown()
{

    //Release model texture
    ReleaseTexture();

	// Shutdown the vertex and index buffers
	ShutdownBuffers();

	//Release model data
	ReleaseModel();

}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(deviceContext);
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
    return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device *device)
{
	VertexType* vertices = nullptr;
	unsigned long* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	D3D11_BUFFER_DESC indexBufferDesc = {};
	D3D11_SUBRESOURCE_DATA vertexData = {};
	D3D11_SUBRESOURCE_DATA indexData = {};
	HRESULT result = 0;

	//Create vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//Create index array
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//Load vertex array and index array with data
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = DirectX::XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = DirectX::XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = DirectX::XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;

}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
    bool result = true;

    //Create the texture object
    m_Texture = new TextureClass();
    if (!m_Texture)
    {
        return false;
    }

    //Initialize texture object
    result = m_Texture->Initialize(device, deviceContext, filename);
    if (!result)
    {
        return false;
    }

    return true;
}


void ModelClass::ReleaseTexture()
{
    //Release the texture object
    if (m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = 0;
    }

    return;
}


bool ModelClass::LoadModel(char* filename)
{
	ifstream fin = {};
	char input = {};
	
	//Open model file
	fin.open(filename);

	//Exit if could not open the file
	if (fin.fail())
	{
		return false;
	}

	//Read up to value of vertex count
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//Read in vertex count
	fin >> m_vertexCount;

	//Set number of indices to be same as vertex count
	m_indexCount = m_vertexCount;

	//Create model using vertex count that was read in
	m_model = new ModelType[m_vertexCount];

	//Read up to beginning of data
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	//Read in vertex data
	for (int i = 0; i < m_vertexCount; i++) {
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	//Close model file
	fin.close();

	return true;
}


void ModelClass::ReleaseModel()
{
	if (m_model) 
	{
		delete[] m_model;
		m_model = 0;
	}
}