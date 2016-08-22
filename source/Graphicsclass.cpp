#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
    m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
//	m_ColorShader = 0;
    m_LightShader = 0;
    m_Light = 0;
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
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "assets/objects/cube.txt", "assets/images/stone01.tga");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

    //Create the light shader object
    m_LightShader = new LightShaderClass();

    //Initialize the light shader object
    result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
        return false;
    }

    //Create light object
    m_Light = new LightClass();

    //Initialize light object
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f); //15% white color
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); //Purple light
    m_Light->SetDirection(1.0f, 0.0f, 0.0f); //Pointing down positive X axis in order to see ambient lighting

    return true;
}


void GraphicsClass::Shutdown()
{

    // Release the light object.
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    //Release the light shader object
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
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
	bool result = true;
    static float rotation = 0.0f;

    //Update rotation variable each frame
    rotation += (float)DirectX::XM_PI * 0.005f;
    if (rotation > 360.0f)
    {
        rotation -= 360.0f;
    }

	// Render the graphics scene.
	result = Render(rotation);
	if (!result)
	{
		return false;
	}

    return true;
}


bool GraphicsClass::Render(float rotation)
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

    //Rotate world matrix by rotation value so that the triangle will spin
    worldMatrix = DirectX::XMMatrixRotationY(rotation);

	//Put model vertex and index buffers on graphics pipeline to prepare for drawing
	m_Model->Render(m_Direct3D->GetDeviceContext());

    //Render model using color shader
    result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
        m_Model->GetTexture(), m_Light->GetDirection(),m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
    	return false;
    }

	//Render model using color shader
	//result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	//if (!result)
	//{
	//	return false;
	//}

    //Render the model using the texture shader
    result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

	//Present the rendered scene to the screen
	m_Direct3D->EndScene();

    return true;
}