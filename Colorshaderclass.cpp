#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
    m_vertexShader = 0;
    m_pixelShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;
}


ColorShaderClass::ColorShaderClass(const ColorShaderClass& /*other*/)
{

}


ColorShaderClass::~ColorShaderClass()
{

}


//--- Initialize(): Call init functions for shaders ---//
bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result = true;

	//Init vertex and pixel shaders
	result = InitializeShader(device, hwnd, L"color.vs", L"color.ps");
	if (!result)
	{
		return false;
	}

	return true;
}


//--- Shutdown(): Calls the shutdown of the shader ---//
void ColorShaderClass::Shutdown()
{
	//Shutdown vertex and pixel shaders as well as related objects
	ShutdownShader();
}


//--- Render(): Set parameters inside shader, then calls RenderShader() to draw object ---//
bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix,
	DirectX::XMMATRIX projectionMatrix)
{
	bool result = true;

	//Set shader parameters that it will use for rendering
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//Now render prepared buffers with shader
	RenderShader(deviceContext, indexCount);

	return true;
}


//--- InitializeShader(): Loads shader files, makes usable to DirectX and GPU. ---//
bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result = 0;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2] = {};
	unsigned int numElements = 0;
	D3D11_BUFFER_DESC matrixBufferDesc = {};

	//Compile vertex shader code
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage) {
			//If shader failed to compile, it should have written to error message
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		//If ther ewas nothing in error message then it could not find shader file itself
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Compile pixel shader code
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Create vertex shader from buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//Create pixel shader from buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void ColorShaderClass::ShutdownShader()
{
	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix,
	DirectX::XMMATRIX projectionMatrix)
{
	HRESULT result = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	MatrixBufferType* dataPtr = nullptr;
	unsigned int bufferNumber = 0;

	//Transpose matrices to prepare them for shader (required by DirectX 11)
	worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
	viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
	projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);

	//Lock constant buffer so it can be written to
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//Get pointer to data in constant buffer
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//Copy matrices into constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//Unlock constant buffer
	deviceContext->Unmap(m_matrixBuffer, 0);

	//Set position of constant buffer in vertex shader
	bufferNumber = 0;

	//Set constant buffer in vertex shader with updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}


//--- RenderShader(): Second function called in Render(). Let GPU know format of data in vertex buffer. ---//
//--- Set vertex shader and pixel shader to render vertex buffer. ---//
//--- Render triangle by calling DrawIndexed ---//
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	//Set vertex and pixel shaders that will be used to render triangle
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//Render triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);
}