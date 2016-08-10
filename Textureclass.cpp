#include "textureclass.h"

TextureClass::TextureClass()
{
    m_targaData = 0;
    m_texture = 0;
    m_textureView = 0;
}


TextureClass::TextureClass(const TextureClass& /*other*/)
{

}


TextureClass::~TextureClass()
{

}


bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
    bool result = true;
    int height = 0;
    int width = 0;
    D3D11_TEXTURE2D_DESC textureDesc = {};
    HRESULT hResult = 0;
    unsigned int rowPitch = 0;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    //Load targa image data into memory
    result = LoadTarga(filename, height, width);
    if(!result)
    {
        return false;
    }

    //Setup description of the texture
    textureDesc.Height = height;
    textureDesc.Width = width;
    textureDesc.MipLevels = 0;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    //Create the empty texture
    hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
    if (FAILED(hResult))
    {
        return false;
    }

    //Set the row pitch of the targa image data
    rowPitch = (width * 4) * sizeof(unsigned char);

    //Copy targa image data into texture
    //Note: Using UpdateSubresource here instead of Map and Unmap since it is getting loaded once (or loaded rarely).
    deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

    //Setup shader resource view description
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = (unsigned int)-1;

    //Create shader resource view for the texture
    hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
    if (FAILED(hResult))
    {
        return false;
    }

    //Generate mipmaps for texture
    deviceContext->GenerateMips(m_textureView);

    //Release targa image data since it has been loaded into texture
    delete[] m_targaData;
    m_targaData = 0;

    return true;
}


void TextureClass::Shutdown()
{
    //Release texture view resource
    if (m_textureView)
    {
        m_textureView->Release();
        m_textureView = 0;
    }

    //Release texture
    if (m_texture)
    {
        m_texture->Release();
        m_texture = 0;
    }

    //Release targa data
    if (m_targaData)
    {
        delete[] m_targaData;
        m_targaData = 0;
    }

    return;
}


ID3D11ShaderResourceView* TextureClass::GetTexture()
{
    return m_textureView;
}


//--- LoadTarga(): Open texture targa file, read it into array, then flip and lad into m_targaData array in correct order. ---//
bool TextureClass::LoadTarga(char* filename, int& height, int& width)
{
    int error = 0;
    int bpp = 0;
    int imageSize = 0;
    int index = 0;
    int i, j, k = 0;
    FILE* filePtr = nullptr;
    unsigned int count = 0;
    TargaHeader targaFileHeader = {};
    unsigned char* targaImage = nullptr;

    //Open targa file for reading in binary
    error = fopen_s(&filePtr, filename, "rb");
    if (error == 0)
    {
        return false;
    }

    //Read in the file header
    count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
    if (count != 1)
    {
        return false;
    }

    //Get important info from header
    height = (int)targaFileHeader.height;
    width = (int)targaFileHeader.width;
    bpp = (int)targaFileHeader.bpp;

    //Check that it is 32 bit and not 24 bit. 24 bit targas are not supported
    if (bpp != 32)
    {
        return false;
    }

    //Calculate size of 32 bit image data
    imageSize = width * height * 4;

    //Allocate memory for targa image data
    targaImage = new unsigned char[imageSize];
    if (!targaImage)
    {
        return false;
    }

    //Read in targa image data
    count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
    if (count != (unsigned int)imageSize)
    {
        return false;
    }

    //Close the file
    error = fclose(filePtr);
    if (error != 0)
    {
        return false;
    }

    //Allocate memory for targa destination data
    m_targaData = new unsigned char[imageSize];
    if (!m_targaData)
    {
        return false;
    }

    //Initialize the index into targa destination data array
    index = 0;

    //Initialize index into targa image data
    k = (width * height * 4) - (width * 4);

    //Copy targa image data into targa destination array in correct order ssince targa format is stored upside down
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            m_targaData[index + 0] = targaImage[k + 2]; //Red
            m_targaData[index + 1] = targaImage[k + 1]; //Green
            m_targaData[index + 2] = targaImage[k + 0]; //Blue
            m_targaData[index + 3] = targaImage[k + 3]; //Alpha

            //Increment indexes into target data
            k += 4;
            index += 4;
        }

        //set targa image data index backto preceding row at beginning of the column since its reading it upside down
        k -= (width * 8);
    }

    //Release targa image data now that it was copied into destination array
    delete[] targaImage;
    targaImage = 0;

    return true;
}