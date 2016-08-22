//LightClass: Simple class to only maintain the direction and color of lights

#pragma once

#include <directxmath.h>

class LightClass
{
public:
    LightClass();
    LightClass(const LightClass&);
    ~LightClass();

	void SetAmbientColor(float, float, float, float);
    void SetDiffuseColor(float, float, float, float);
    void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	DirectX::XMFLOAT4 GetAmbientColor();
    DirectX::XMFLOAT4 GetDiffuseColor();
    DirectX::XMFLOAT3 GetDirection();
	DirectX::XMFLOAT4 GetSpecularColor();
	float GetSpecularPower();

private:
	DirectX::XMFLOAT4 m_ambientColor;
    DirectX::XMFLOAT4 m_diffuseColor;
    DirectX::XMFLOAT3 m_direction;
	DirectX::XMFLOAT4 m_specularColor;
	float m_specularPower;
};