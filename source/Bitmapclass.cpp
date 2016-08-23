#include "bitmapclass.h"

BitmapClass::BitmapClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_Texture = 0;
}


BitmapClass::BitmapClass(const BitmapClass& /*other*/)
{

}


BitmapClass::~BitmapClass()
{

}


bool BitmapClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, char* textureFilename, int bitmapWidth, int bitmapHeight)
{
    bool result = true;

    //Store screen size
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    //Store size in pixels that this bitmap should be rendered at
    m_bitmapWidth = bitmapWidth;
    m_bitmapHeight = bitmapHeight;

    //Initialize previous rendering position to -1
    m_previousPosX = -1;
    m_previousPosY = -1;

    //Initialize vertex and index buffers
    result = InitializeBuffers(device);
    if (!result)
    {
        return false;
    }

    //Load texture for this model
    result = LoadTexture(device, deviceContext, textureFilename);
    if (!result)
    {
        return false;
    }

    return true;
}


void BitmapClass::Shutdown()
{
    //Release the model texture
    ReleaseTexture();

    //Shutdown the vertex and index buffers
    ShutdownBuffers();
}


bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
    bool result = true;

    //Re-build dymanic vertex buffer for rendering to possibly different location on the screen
    result = UpdateBuffers(deviceContext, positionX, positionY);
    if (!result)
    {
        return false;
    }

    //Put the vertex and index buffers on the graphics pipeline to prepare for drawing
    RenderBuffers(deviceContext);

    return true;
}


int BitmapClass::GetIndexCount()
{
    return m_indexCount;
}


ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
    return m_Texture->GetTexture();
}


bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices = nullptr;
    unsigned long* indices = nullptr;
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    D3D11_BUFFER_DESC indexBufferDesc = {};
    D3D11_SUBRESOURCE_DATA vertexData = {};
    D3D11_SUBRESOURCE_DATA indexData = {};
    HRESULT result = 0;

    //Set number of vertices in vertex array
    m_vertexCount = 6;

    //Set number of indices in index array
    m_indexCount = m_vertexCount;

    //Create vertex array
    vertices = new VertexType[m_vertexCount];

    //Create index array
    indices = new unsigned long[m_indexCount];

    //Initialze vertex array to zeros at first
    memset(vertices, 0, (sizeof(VertexType)* m_vertexCount));

    //Load index array with data
    for (int i = 0; i < m_indexCount; i++)
    {
        indices[i] = i;
    }

    //Set up description of dynamic vertex buffer
    //Dynamic vertex buffer (vs static) allows for modification of the data inside the buffer each frame if needed
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //Makes this buffer dynamic
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    //Give the subresource structure a pointer to the vertex data
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    //Create the vertex buffer
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    //Set up the description of the static index buffer
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    //Give the subresource structure a pointer to the index data
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    //Create the index buffer
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    //Release the arrays now that the vertex and index buffers have been created and loaded
    delete[] vertices;
    vertices = 0;

    delete[] indices;
    indices = 0;

    return true;
}


void BitmapClass::ShutdownBuffers()
{
    //Release the index buffer
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    //Release the vertex buffer
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }
}


bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
    float left = 0.0f;
    float right = 0.0f;
    float top = 0.0f;
    float bottom = 0.0f;
    VertexType* vertices = nullptr;
    D3D11_MAPPED_SUBRESOURCE mappedResource = {};
    VertexType* verticesPtr = nullptr;
    HRESULT result = 0;

    //If the position we are rendering this bitmap to has not changed then don't update the vertex buffer
    //since it currently has correct parameters
    if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
    {
        return true;
    }

    //If it has changed then update position its being rendered to
    m_previousPosX = positionX;
    m_previousPosY = positionY;

    //Calculate scren coordinates of left side of bitmap
    left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

    //Calculate screen coordinates of right side of bitmap
    right = left + (float)m_bitmapWidth;

    //Calculate screen coordinates of top of bitmap
    top = (float)(m_screenHeight / 2) - (float)positionY;

    //Calculate screen coordinates of bottom of bitmap
    bottom = top - (float)m_bitmapHeight;

    // Create the vertex array.
    vertices = new VertexType[m_vertexCount];
    if (!vertices)
    {
        return false;
    }

    //Load the vertex array with data:
    //First triangle
    vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);  // Top left.
    vertices[0].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = DirectX::XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
    vertices[1].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = DirectX::XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
    vertices[2].texture = DirectX::XMFLOAT2(0.0f, 1.0f);

    //Second triangle
    vertices[3].position = DirectX::XMFLOAT3(left, top, 0.0f);  // Top left.
    vertices[3].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = DirectX::XMFLOAT3(right, top, 0.0f);  // Top right.
    vertices[4].texture = DirectX::XMFLOAT2(1.0f, 0.0f);

    vertices[5].position = DirectX::XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
    vertices[5].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

    //Lock the vertex buffer so it can be written to
    result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    //Get a pointer to the data in the vertex buffer
    verticesPtr = (VertexType*)mappedResource.pData;

    //Copy the data into the vertex buffer
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

    //Unlock the vertex buffer
    deviceContext->Unmap(m_vertexBuffer, 0);

    //Release the vertex array as it is no longer needed
    delete[] vertices;
    vertices = 0;

    return true;
}


void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    //Set vertex buffer to active in input assembler so it can be rendered
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    //Set index buffer to active in input assembler so it can be rendered
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    //Set type of primitive that should be rendered from vertex buffer, in this case triangles
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool BitmapClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
    bool result = true;

    // Create the texture object.
    m_Texture = new TextureClass;
    if (!m_Texture)
    {
        return false;
    }

    // Initialize the texture object.
    result = m_Texture->Initialize(device, deviceContext, filename);
    if (!result)
    {
        return false;
    }

    return true;
}


void BitmapClass::ReleaseTexture()
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