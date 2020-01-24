#pragma once

class Viewport
{
public:
	Viewport();
	Viewport(GLint x, GLint y, GLsizei width, GLsizei height);
	~Viewport();

	void SetPosition(GLint x, GLint y);
	void SetSize(GLsizei width, GLsizei height);

	void GetPosition(GLint& x, GLint& y);
	void GetSize(GLsizei& width, GLsizei& height);

protected:
	GLint m_X;
	GLint m_Y;
	GLsizei m_Width;
	GLsizei m_Height;
};
