#include "WindowClass.h"

WindowClass::WindowClass()
{
}


WindowClass::~WindowClass()
{
}

WindowClass& WindowClass::getInstance()
{
	static WindowClass instance;
	return instance;
}


void WindowClass::setWidth(unsigned short width)
{
	wndWidth = width;
}

void WindowClass::setHeight(unsigned short height)
{
	wndHeight = height;
}

unsigned short WindowClass::getWidth()
{
	return wndWidth;
}

unsigned short WindowClass::getHeight()
{
	return wndHeight;
}

float WindowClass::getAspectRatio()
{
	float result = 4 / 3.0f;
	if (wndWidth == wndHeight)
	{
		return 1.0f;
	}

	if (wndWidth > wndHeight)
	{
		if (wndHeight == 0)
		{
			wndHeight = wndWidth;
		}
		result = wndWidth / wndHeight;
	}
	else
	{
		if (wndWidth == 0)
		{
			wndWidth = wndHeight;
		}
		result = wndHeight / wndWidth;
	}
	return result;
}

unsigned short WindowClass::getResolutionWidth()
{
	return resolutionWidth;
}

unsigned short WindowClass::getResolutionHeight()
{
	return resolutionHeight;
}

void WindowClass::setResolutionWidth(unsigned short width)
{
	resolutionWidth = width;
}

void WindowClass::setResolutionHeight(unsigned short height)
{
	resolutionHeight = height;
}