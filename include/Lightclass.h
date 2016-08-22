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

	DirectX::XMFLOAT4 GetAmbientColor();
    DirectX::XMFLOAT4 GetDiffuseColor();
    DirectX::XMFLOAT3 GetDirection();

private:
	DirectX::XMFLOAT4 m_ambientColor;
    DirectX::XMFLOAT4 m_diffuseColor;
    DirectX::XMFLOAT3 m_direction;
};