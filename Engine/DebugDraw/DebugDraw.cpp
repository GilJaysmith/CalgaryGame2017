#include "Engine/Pch.h"

#include "Engine/DebugDraw/DebugDraw.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderManager.h"


namespace DebugDraw
{
	class IRenderableDebugObject : public IDebugObject
	{
	public:
		virtual void Render() = 0;

	protected:
		GLuint m_Vao;
		glm::vec3 m_Colour;
	};

	std::set<IRenderableDebugObject*> s_DebugObjects;
	GLuint s_Shader;
	GLint s_UniTint;

	void Initialize()
	{
		s_Shader = ShaderManager::LoadProgram("DebugDraw");
		s_UniTint = glGetUniformLocation(s_Shader, "tint");
	}

	void Terminate()
	{

	}

	void Render()
	{
		ShaderManager::SetActiveShader(s_Shader);
		for (auto object : s_DebugObjects)
		{
			object->Render();
		}
	}

	void Release(IRenderableDebugObject* object)
	{
		s_DebugObjects.erase(object);
		delete object;
	}

	class _DebugLine : public IRenderableDebugObject
	{
	public:
		_DebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour)
		{
			m_Colour = colour;

			glGenVertexArrays(1, &m_Vao);
			glBindVertexArray(m_Vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			float vert_data[] =
			{
				start.x, start.y, start.z,
				end.x, end.y, end.z
			};

			glBufferData(GL_ARRAY_BUFFER, sizeof(vert_data), vert_data, GL_STATIC_DRAW);

			GLint posAttrib = glGetAttribLocation(s_Shader, "position");
			if (posAttrib > -1)
			{
				glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(posAttrib);
			}
		}

		~_DebugLine()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glUniform3fv(s_UniTint, 1, glm::value_ptr(m_Colour));
			glDrawArrays(GL_LINES, 0, 6);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}
	};

	IDebugObject* DebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour)
	{
		_DebugLine* debug_object = MemNew(MemoryPool::Rendering, _DebugLine)(start, end, colour);
		s_DebugObjects.insert(debug_object);
		return debug_object;
	}

	class _DebugLines : public IRenderableDebugObject
	{
	public:
		_DebugLines(const std::vector<glm::vec3>& points, const glm::vec3& colour)
		{
			m_Colour = colour;
			m_NumPoints = static_cast<unsigned int>(points.size());

			glGenVertexArrays(1, &m_Vao);
			glBindVertexArray(m_Vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 3 * points.size());
			float* vert_data = static_cast<float*>(MemNewBytes(MemoryPool::Rendering, data_size));
			float* f = vert_data;
			for (auto point : points)
			{
				*f++ = point.x;
				*f++ = point.y;
				*f++ = point.z;
			}

			glBufferData(GL_ARRAY_BUFFER, data_size, vert_data, GL_STATIC_DRAW);

			GLint posAttrib = glGetAttribLocation(s_Shader, "position");
			if (posAttrib > -1)
			{
				glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(posAttrib);
			}

			MemDelete(vert_data);
		}

		~_DebugLines()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glUniform3fv(s_UniTint, 1, glm::value_ptr(m_Colour));
			glDrawArrays(GL_LINE_STRIP, 0, m_NumPoints);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		unsigned int m_NumPoints;
	};

	IDebugObject* DebugLines(const std::vector<glm::vec3>& points, const glm::vec3& colour)
	{
		_DebugLines* debug_object = MemNew(MemoryPool::Rendering, _DebugLines)(points, colour);
		s_DebugObjects.insert(debug_object);
		return debug_object;
	}

	class _DebugGrid : public IRenderableDebugObject
	{
	public:
		_DebugGrid(const glm::mat4& transform, float grid_square_size, unsigned int squares_across, unsigned int squares_deep, const glm::vec3& colour)
		{
			m_Colour = colour;
			m_NumLines = (squares_across + 1) * (squares_deep + 1);

			glGenVertexArrays(1, &m_Vao);
			glBindVertexArray(m_Vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 6 * m_NumLines);
			float* vert_data = static_cast<float*>(MemNewBytes(MemoryPool::Rendering, data_size));
			float* f = vert_data;
			float grid_width = grid_square_size * squares_across;
			float grid_depth = grid_square_size * squares_deep;
			for (unsigned int x = 0; x <= squares_across; ++x)
			{
				float x_coord = (-grid_width / 2) + (grid_square_size * x);
				glm::vec3 line_start(x_coord, 0.0f, -grid_depth / 2);
				glm::vec3 line_end(x_coord, 0.0f, grid_depth / 2);
				line_start = glm::vec3(transform * glm::vec4(line_start, 1.0f));
				line_end = glm::vec3(transform * glm::vec4(line_end, 1.0f));
				*f++ = line_start.x;
				*f++ = line_start.y;
				*f++ = line_start.z;
				*f++ = line_end.x;
				*f++ = line_end.y;
				*f++ = line_end.z;
			}
			for (unsigned int z = 0; z <= squares_deep; ++z)
			{
				float z_coord = (-grid_depth / 2) + (grid_square_size * z);
				glm::vec3 line_start(-grid_width / 2, 0.0f, z_coord);
				glm::vec3 line_end(grid_width / 2, 0.0f, z_coord);
				line_start = glm::vec3(transform * glm::vec4(line_start, 1.0f));
				line_end = glm::vec3(transform * glm::vec4(line_end, 1.0f));
				*f++ = line_start.x;
				*f++ = line_start.y;
				*f++ = line_start.z;
				*f++ = line_end.x;
				*f++ = line_end.y;
				*f++ = line_end.z;
			}

			glBufferData(GL_ARRAY_BUFFER, data_size, vert_data, GL_STATIC_DRAW);

			GLint posAttrib = glGetAttribLocation(s_Shader, "position");
			if (posAttrib > -1)
			{
				glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(posAttrib);
			}

			MemDelete(vert_data);
		}

		~_DebugGrid()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glUniform3fv(s_UniTint, 1, glm::value_ptr(m_Colour));
			glDrawArrays(GL_LINES, 0, m_NumLines);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		unsigned int m_NumLines;
	};

	IDebugObject* DebugGrid(const glm::mat4& transform, float grid_square_size, unsigned int squares_across, unsigned int squares_deep, const glm::vec3& colour)
	{
		_DebugGrid* debug_object = MemNew(MemoryPool::Rendering, _DebugGrid)(transform, grid_square_size, squares_across, squares_deep, colour);
		s_DebugObjects.insert(debug_object);
		return debug_object;
	}

	const unsigned int NUM_SEGMENTS = 10;

	class _DebugSphere : public IRenderableDebugObject
	{
	public:
		_DebugSphere(const glm::vec3& centre, float radius, const glm::vec3& colour)
		{
			m_Colour = colour;
			m_NumCircles = 0;

			std::vector<std::vector<glm::vec3>> circles;

			// Horizontal circles.
			for (int y = 5; y <= 175; y += 17)
			{
				std::vector<glm::vec3> circle;
				float y_angle = (y / 180.0f) * glm::pi<float>();
				float y_coord = radius * cosf(y_angle);
				float x_scale = sinf(y_angle);
				float angle_division = (glm::pi<float>() * 2) / (4 * NUM_SEGMENTS);
				for (int clock = 0; clock < 4 * NUM_SEGMENTS; ++clock)
				{
					float angle = angle_division * clock;
					float x = cosf(angle) * radius * x_scale;
					float z = sinf(angle) * radius * x_scale;
					glm::vec3 pos(x, y_coord, z);
					circle.push_back(pos + centre);
				}
				circles.push_back(circle);
			}

			// Vertical circles.
			for (int y_rotate = 0; y_rotate < 180; y_rotate += 20)
			{
				std::vector<glm::vec3> circle;
				float y_angle = (y_rotate / 180.0f) * glm::pi<float>();
				float angle_division = (glm::pi<float>() * 2) / (4 * NUM_SEGMENTS);
				for (int clock = 0; clock < 4 * NUM_SEGMENTS; ++clock)
				{
					float angle = angle_division * clock;
					float x = cosf(angle) * radius;
					float y = sinf(angle) * radius;
					float z = x * sinf(y_angle);
					x *= cosf(y_angle);
					glm::vec3 pos(x, y, z);
					circle.push_back(pos + centre);
				}
				circles.push_back(circle);
			}

			m_NumCircles = static_cast<unsigned int>(circles.size());

			glGenVertexArrays(1, &m_Vao);
			glBindVertexArray(m_Vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 3 * 4 * NUM_SEGMENTS * m_NumCircles);
			float* vert_data = static_cast<float*>(MemNewBytes(MemoryPool::Rendering, data_size));
			float* f = vert_data;

			for (auto circle : circles)
			{
				for (auto point : circle)
				{
					*f++ = point.x;
					*f++ = point.y;
					*f++ = point.z;
				}
			}

			glBufferData(GL_ARRAY_BUFFER, data_size, vert_data, GL_STATIC_DRAW);

			GLint posAttrib = glGetAttribLocation(s_Shader, "position");
			if (posAttrib > -1)
			{
				glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(posAttrib);
			}

			MemDelete(vert_data);
		}

		~_DebugSphere()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glUniform3fv(s_UniTint, 1, glm::value_ptr(m_Colour));
			for (unsigned int circle_idx = 0; circle_idx < m_NumCircles; ++circle_idx)
			{
				int start_idx = circle_idx * (4 * NUM_SEGMENTS);
				glDrawArrays(GL_LINE_LOOP, start_idx, 4 * NUM_SEGMENTS);
			}
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		unsigned int m_NumCircles;
	};

	IDebugObject* DebugSphere(const glm::vec3& centre, float radius, const glm::vec3& colour)
	{
		_DebugSphere* debug_object = MemNew(MemoryPool::Rendering, _DebugSphere)(centre, radius, colour);
		s_DebugObjects.insert(debug_object);
		return debug_object;
	}

	class _DebugBox : public IRenderableDebugObject
	{
	public:
		_DebugBox(const glm::mat4& transform, const glm::vec3& dims, const glm::vec3& colour)
		{
			m_Colour = colour;

			glGenVertexArrays(1, &m_Vao);
			glBindVertexArray(m_Vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 3 * 24);
			float* vert_data = static_cast<float*>(MemNewBytes(MemoryPool::Rendering, data_size));
			float* f = vert_data;
			glm::vec3 half_dims = dims / 2.0f;
			glm::vec3 bottom_left_rear(-half_dims.x, -half_dims.y, -half_dims.z);
			glm::vec3 bottom_right_rear(half_dims.x, -half_dims.y, -half_dims.z);
			glm::vec3 bottom_left_front(-half_dims.x, -half_dims.y, half_dims.z);
			glm::vec3 bottom_right_front(half_dims.x, -half_dims.y, half_dims.z);
			glm::vec3 top_left_rear(-half_dims.x, half_dims.y, -half_dims.z);
			glm::vec3 top_right_rear(half_dims.x, half_dims.y, -half_dims.z);
			glm::vec3 top_left_front(-half_dims.x, half_dims.y, half_dims.z);
			glm::vec3 top_right_front(half_dims.x, half_dims.y, half_dims.z);
			std::vector<glm::vec3> line_points = {
				bottom_left_rear, bottom_right_rear,
				bottom_right_rear, bottom_right_front,
				bottom_right_front, bottom_left_front,
				bottom_left_front, bottom_left_rear,
				top_left_rear, top_right_rear,
				top_right_rear, top_right_front,
				top_right_front, top_left_front,
				top_left_front, top_left_rear,
				bottom_left_rear, top_left_rear,
				bottom_right_rear, top_right_rear,
				bottom_left_front, top_left_front,
				bottom_right_front, top_right_front
			};

			for (auto point : line_points)
			{
				point = glm::vec3(transform * glm::vec4(point, 1.0f));
				*f++ = point.x;
				*f++ = point.y;
				*f++ = point.z;
			}

			glBufferData(GL_ARRAY_BUFFER, data_size, vert_data, GL_STATIC_DRAW);

			GLint posAttrib = glGetAttribLocation(s_Shader, "position");
			if (posAttrib > -1)
			{
				glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(posAttrib);
			}

			MemDelete(vert_data);
		}

		~_DebugBox()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glUniform3fv(s_UniTint, 1, glm::value_ptr(m_Colour));
			glDrawArrays(GL_LINES, 0, 24);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		unsigned int m_NumPoints;
	};

	IDebugObject* DebugBox(const glm::mat4& transform, const glm::vec3& dims, const glm::vec3& colour)
	{
		_DebugBox* debug_object = MemNew(MemoryPool::Rendering, _DebugBox)(transform, dims, colour);
		s_DebugObjects.insert(debug_object);
		return debug_object;
	}

	class _DebugCrosshair : public IRenderableDebugObject
	{
	public:
		_DebugCrosshair(const glm::vec3& centre, float scale, const glm::vec3& colour)
		{
			m_Colour = colour;

			glGenVertexArrays(1, &m_Vao);
			glBindVertexArray(m_Vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 3 * 6);
			float* vert_data = static_cast<float*>(MemNewBytes(MemoryPool::Rendering, data_size));
			float* f = vert_data;
			std::vector<glm::vec3> line_points = {
				glm::vec3(-scale / 2, 0.0f, 0.0f), glm::vec3(scale / 2, 0.0f, 0.0f),
				glm::vec3(0.0f, -scale / 2, 0.0f), glm::vec3(0.0f, scale / 2, 0.0f),
				glm::vec3(0.0f, 0.0f, -scale / 2), glm::vec3(0.0f, 0.0f, scale / 2)
			};

			for (auto point : line_points)
			{
				point += centre;
				*f++ = point.x;
				*f++ = point.y;
				*f++ = point.z;
			}

			glBufferData(GL_ARRAY_BUFFER, data_size, vert_data, GL_STATIC_DRAW);

			GLint posAttrib = glGetAttribLocation(s_Shader, "position");
			if (posAttrib > -1)
			{
				glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(posAttrib);
			}

			MemDelete(vert_data);
		}

		~_DebugCrosshair()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glUniform3fv(s_UniTint, 1, glm::value_ptr(m_Colour));
			glDrawArrays(GL_LINES, 0, 6);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		unsigned int m_NumPoints;
	};

	IDebugObject* DebugCrosshair(const glm::vec3& centre, float scale, const glm::vec3& colour)
	{
		_DebugCrosshair* debug_object = MemNew(MemoryPool::Rendering, _DebugCrosshair)(centre, scale, colour);
		s_DebugObjects.insert(debug_object);
		return debug_object;
	}

	class _DebugArrow : public IRenderableDebugObject
	{
	public:
		_DebugArrow(const glm::mat4& transform, float scale, const glm::vec3& colour)
		{
			m_Colour = colour;

			glGenVertexArrays(1, &m_Vao);
			glBindVertexArray(m_Vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 3 * 6);
			float* vert_data = static_cast<float*>(MemNewBytes(MemoryPool::Rendering, data_size));
			float* f = vert_data;
			std::vector<glm::vec3> line_points = {
				glm::vec3(-scale / 2, 0.0f, 0.0f), glm::vec3(scale / 2, 0.0f, 0.0f),
				glm::vec3(scale / 4, 0.0f, -scale / 4), glm::vec3(scale / 2, 0.0f, 0.0f),
				glm::vec3(scale / 4, 0.0f, scale / 4), glm::vec3(scale / 2, 0.0f, 0.0f)
			};

			for (auto point : line_points)
			{
				point = glm::vec3(transform * glm::vec4(point, 1.0f));
				*f++ = point.x;
				*f++ = point.y;
				*f++ = point.z;
			}

			glBufferData(GL_ARRAY_BUFFER, data_size, vert_data, GL_STATIC_DRAW);

			GLint posAttrib = glGetAttribLocation(s_Shader, "position");
			if (posAttrib > -1)
			{
				glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(posAttrib);
			}

			MemDelete(vert_data);
		}

		~_DebugArrow()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glUniform3fv(s_UniTint, 1, glm::value_ptr(m_Colour));
			glDrawArrays(GL_LINES, 0, 6);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		unsigned int m_NumPoints;
	};

	IDebugObject* DebugArrow(const glm::mat4& transform, float scale, const glm::vec3& colour)
	{
		_DebugArrow* debug_object = MemNew(MemoryPool::Rendering, _DebugArrow)(transform, scale, colour);
		s_DebugObjects.insert(debug_object);
		return debug_object;
	}
}