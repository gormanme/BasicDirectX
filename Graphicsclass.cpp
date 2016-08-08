#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
    m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& /*other*/)
{

}


GraphicsClass::~GraphicsClass()
{

}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result = true;

    //Create the Direct3D object
    m_Direct3D = new D3DClass();
    if (!m_Direct3D)
    {
        return false;
    }

    //Initialize the Direct3D object
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

	//Code added here for drawing triangle (camera, model, and shader classes)
	//Create camera object
	m_Camera = new CameraClass();
	if (!m_Camera)
	{
		return false;
	}

	//Set initial position of camera
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	//Create Model object
	m_Model = new ModelClass();
	if (!m_Model)
	{
		return false;
	}

	//Initialize Model object
	result = m_Model->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	//Create the color shader object
	m_ColorShader = new ColorShaderClass();
	if (!m_ColorShader)
	{
		return false;
	}

	//Initialize the color shader object
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

    return true;
}


void GraphicsClass::Shutdown()
{

	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

    //Release the Direct3D object
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
    }
}


bool GraphicsClass::Frame()
{
	bool result = true;;

	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

    return true;
}


bool GraphicsClass::Render()
{
	DirectX::XMMATRIX worldMatrix = {};
	DirectX::XMMATRIX viewMatrix = {};
	DirectX::XMMATRIX projectionMatrix = {};
	bool result = true;

	//Clear the buffers to begin the scene
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Generate view matrix based on camera's position
	m_Camera->Render();

	//Get world, view, and projection matrices from camera and d3d objects
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	//Put model vertex and index buffers on graphics pipeline to prepare for drawing
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//Render model using color shader
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}


	//Present the rendered scene to the screen
	m_Direct3D->EndScene();

    return true;
}