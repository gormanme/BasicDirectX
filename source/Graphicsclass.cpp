#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
    m_Direct3D = 0;
    m_Camera = 0;
    m_Model = 0;
//	m_ColorShader = 0;
    m_LightShader = 0;
    m_Light = 0;
    m_Bitmap = 0;
    m_Text = 0;
	m_ModelList = 0;
	m_Frustum = 0;
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
    DirectX::XMMATRIX baseViewMatrix = {};

    //Create the Direct3D object
    m_Direct3D = new D3DClass();

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

    //Initialize a base view matrix with the camera for 2D user interface rendering
    m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
    m_Camera->Render();
    m_Camera->GetViewMatrix(baseViewMatrix);

    //Create the text object
    m_Text = new TextClass();

    //Initialize the text object
    result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
        return false;
    }

    //Create Model object
    m_Model = new ModelClass();

    //Initialize Model object
    result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "assets/objects/sphere.txt", "assets/images/stone01.tga");
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
    //m_Light->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f); //15% white color
    //m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); //White light
    m_Light->SetDirection(0.0f, 0.0f, 1.0f); //Pointing down positive Z axis.
    //m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f); //White
    //m_Light->SetSpecularPower(32.0f); //The lower the specular power, the greater the specular effect.

	//Create the model list object
	m_ModelList = new ModelListClass();

	//Initialize the model list object
	m_ModelList->Initialize(25);

	//Create the frustum object
	m_Frustum = new FrustumClass();

    ////Create the bitmap object
    //m_Bitmap = new BitmapClass();

    ////Initialize the bitmap object
    //result = m_Bitmap->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth, screenHeight, "assets/images/stone01.tga", 256, 256);
    //if (!result)
    //{
    //    MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
    //    return false;
    //}

    return true;
}


void GraphicsClass::Shutdown()
{

	// Release the frustum object.
	if (m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	// Release the model list object.
	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}


    // Release the text object.
    if (m_Text)
    {
        m_Text->Shutdown();
        delete m_Text;
        m_Text = 0;
    }

    // Release the bitmap object.
    if (m_Bitmap)
    {
        m_Bitmap->Shutdown();
        delete m_Bitmap;
        m_Bitmap = 0;
    }

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


bool GraphicsClass::Frame(int fps, int cpu, float rotationY)
{
    bool result = true;
    static float rotation = 0.0f;

    //Set the frames per second
    result = m_Text->SetFps(fps, m_Direct3D->GetDeviceContext());
    if (!result)
    {
        return false;
    }

    //Set the CPU usage
    result = m_Text->SetCpu(cpu, m_Direct3D->GetDeviceContext());
    if (!result)
    {
        return false;
    }

	//Set the position of the camera
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	//Set the rotation of the camera
	m_Camera->SetRotation(0.0f, rotationY, 0.0f);

    //Update rotation variable each frame
    //rotation += (float)DirectX::XM_PI * 0.005f;
    //if (rotation > 360.0f)
    //{
    //    rotation -= 360.0f;
    //}

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
    DirectX::XMMATRIX orthoMatrix = {};
    bool result = true;
    rotation = 0;
	int modelCount = 0;
	int renderCount = 0;
	int index = 0;
	float positionX = 0.0f;
	float positionY = 0.0f;
	float positionZ = 0.0f;
	float radius = 1.0f;
	DirectX::XMFLOAT4 color = {};
	bool renderModel = true;

    //Clear the buffers to begin the scene
    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    //Generate view matrix based on camera's position
    m_Camera->Render();

    //Get world, view, and projection matrices from camera and d3d objects
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

	//Construct the frustum
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	//Get the number of modelss that will be rendered
	modelCount = m_ModelList->GetModelCount();

	//Go through all the models and render them only if they can be seen by the camera view
	for (index = 0; i < modelCount; index++)
	{
		//Get the position and color of the sphere model at this index
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		//Check if the sphere model is in the view frustum
		renderModel = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);

		//If it can be seen then render it, if not skip this model and check the next sphere
		if (renderModel)
		{
			//Move the model to the location it should be rendered at
			worldMatrix = DirectX::XMMatrixTranslation(positionX, positionY, positionZ);

			//Put the model vertex and insex buffers on the graphics pipeline to prepare them for drawing
			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model->GetTexture(), m_Light->GetDirection(), color);
		}
	}

    //Turn off Z buffer to begin all 2D rendering
    //m_Direct3D->TurnZBufferOff();

    //Turn on alpha blending before rendering the text
    //m_Direct3D->TurnOnAlphaBlending();

    //Render the text strings
    //result = m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix);
    //if (!result)
    //{
    //    return false;
    //}

    //Turn off alpha blending after rendering the text
    //m_Direct3D->TurnOffAlphaBlending();

    ////Put bitmap vertex and index buffers on graphics pipeline to prepare them for drawing
    //result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 100, 100);
    //if (!result)
    //{
    //    return false;
    //}

    //Render the bitmap with the texture shader
    //result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());
    //if (!result)
    //{
    //	return false;
    //}

    ////Turn the Z buffer back on now that all 2D rendering has completed
    //m_Direct3D->TurnZBufferOn();

 //   //Rotate world matrix by rotation value so that the triangle will spin
 //   worldMatrix = DirectX::XMMatrixRotationY(rotation);

    ////Put model vertex and index buffers on graphics pipeline to prepare for drawing
    //m_Model->Render(m_Direct3D->GetDeviceContext());

    //Render model using color shader
    //result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
    //    m_Bitmap->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(),
    //    m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
  //  result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
  //      m_Model->GetTexture(), m_Light->GetDirection(),m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(),
        //m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
    if (!result)
    {
        return false;
    }

    //Turn Z buffer back on now that all 2D rendering has completed
    m_Direct3D->TurnZBufferOn();

    //Present the rendered scene to the screen
    m_Direct3D->EndScene();

    return true;
}