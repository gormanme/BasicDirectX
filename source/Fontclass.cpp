#include "fontclass.h"

FontClass::FontClass()
{
	m_Font = 0;
	m_Texture = 0;
}


FontClass::FontClass(const FontClass& /*other*/)
{

}


FontClass::~FontClass()
{

}


bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fontFilename, char* textureFilename)
{
	bool result = true;

	//Load in text file containing font data
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	//Load texture that has font characters on it
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void FontClass::Shutdown()
{
	//Release the font texture
	ReleaseTexture();

	//Release the font data
	ReleaseFontData();
}


bool FontClass::LoadFontData(char* filename)
{
	ifstream fin = {};
	char temp = {};

	//Create font spacing buffer
	m_Font = new FontType[95];

	//Read in font size and spacing between chars
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	//read in 95 used ASCII characters for text
	for (int i = 0; i < 95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	//Close the file
	fin.close();

	return true;
}


void FontClass::ReleaseFontData()
{
	//Release the font data array
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = 0;
	}
}


bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result = true;

	//Create texture object
	m_Texture = new TextureClass();

	//Initialize the texture object
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;

}


void FontClass::ReleaseTexture()
{
	//Release the texture object
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}


ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}


void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr = nullptr;
	int numLetters = 0;
	int index = 0;
	int letter = 0;

	//Coerce the input vertices into a VertexType structure
	vertexPtr = (VertexType*)vertices;

	//Get number of letters in the sentence
	numLetters = (int)strlen(sentence);

	//Draw each letter onto a quad
	for (int i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		//If the letter is a space then just move over three pixels
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			//First triangle in quad
			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].left, 1.0f);
			index++;

			//Second triangle in quad
			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(m_Font[letter].right, 1.0f);
			index++;

			//Update the x location for drawing by the size of the letter and one pixel
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}
}