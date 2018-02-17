#include "Engine/Pch.h"

#include "Engine/Rendering/Viewport.h"


Viewport::Viewport()
{
	SetPosition(0, 0);
	SetSize(0, 0);
}


Viewport::Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	SetPosition(x, y);
	SetSize(width, height);
}

Viewport::~Viewport()
{
}

void Viewport::SetPosition(GLint x, GLint y)
{
	m_X = x;
	m_Y = y;
}

void Viewport::SetSize(GLsizei width, GLsizei height)
{
	m_Width = width;
	m_Height = height;
}

void Viewport::GetPosition(GLint& x, GLint& y)
{
	x = m_X;
	y = m_Y;
}

void Viewport::GetSize(GLsizei& width, GLsizei& height)
{
	width = m_Width;
	height = m_Height;
}
