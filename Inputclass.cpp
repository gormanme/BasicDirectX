#include "inputclass.h"

InputClass::InputClass()
{

}


InputClass::InputClass(const InputClass& /*other*/)
{

}


InputClass::~InputClass()
{

}


void InputClass::Initialize()
{
	int i = 0;

	//Initialize all the keys to being released and not pressed
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
}


void InputClass::KeyDown(unsigned int input)
{
	//If a key is pressed tehn save that state in the key array
	m_keys[input] = true;
}


void InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}