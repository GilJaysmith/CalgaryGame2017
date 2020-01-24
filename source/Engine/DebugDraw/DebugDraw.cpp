#include "Engine/Pch.h"

#include "Engine/DebugDraw/DebugDraw.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderManager.h"


namespace DebugDraw
{
	struct IRenderableDebugObject : public IDebugObject
	{
		IRenderableDebugObject()
			: m_Colour(1.0f, 1.0f, 1.0f)
			, m_ExternalTransform(nullptr)
			, m_Persistent(true)
		{
		}

		virtual void SetColour(const glm::vec3& colour) override
		{
			m_Colour = colour;
		}
		virtual void SetTransform(const glm::mat4& transform) override
		{
			m_Transform = transform;
		}
		virtual void SetExternalTransform(const glm::mat4* transform) override
		{
			m_ExternalTransform = transform;
		}
		virtual void SetPersistent(bool persistent) override
		{
			m_Persistent = persistent;
		}
		
		virtual void Render() = 0;

		GLuint m_Vao;
		glm::vec3 m_Colour;
		bool m_Persistent;
		glm::mat4 m_Transform;
		const glm::mat4* m_ExternalTransform;
	};

	std::map<std::string, std::set<IRenderableDebugObject*>> s_DebugObjects;
	GLuint s_Shader;
	GLint s_UniTransform;
	GLint s_UniTint;

	GLuint s_DebugLineVao;
	GLuint s_DebugBoxVao;
	GLuint s_DebugSphereVao;
	unsigned int s_NumCirclesInSphere;
	GLuint s_DebugCrosshairVao;
	GLuint s_DebugArrowVao;

	void MakeDebugLine()
	{
		glGenVertexArrays(1, &s_DebugLineVao);
		glBindVertexArray(s_DebugLineVao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		float vert_data[] =
		{
			1.0f, 0.0f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vert_data), vert_data, GL_STATIC_DRAW);

		GLint posAttrib = glGetAttribLocation(s_Shader, "position");
		if (posAttrib > -1)
		{
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(posAttrib);
		}
	}

	const unsigned int NUM_SPHERE_SEGMENTS = 10;

	void MakeDebugSphere()
	{
		s_NumCirclesInSphere = 0;

		std::vector<std::vector<glm::vec3>> circles;

		const float RADIUS = 0.5;

		// Horizontal circles.
		for (int y = 5; y <= 175; y += 17)
		{
			std::vector<glm::vec3> circle;
			float y_angle = (y / 180.0f) * glm::pi<float>();
			float y_coord = RADIUS * cosf(y_angle);
			float x_scale = sinf(y_angle);
			float angle_division = (glm::pi<float>() * 2) / (4 * NUM_SPHERE_SEGMENTS);
			for (int clock = 0; clock < 4 * NUM_SPHERE_SEGMENTS; ++clock)
			{
				float angle = angle_division * clock;
				float x = cosf(angle) * RADIUS * x_scale;
				float z = sinf(angle) * RADIUS * x_scale;
				glm::vec3 pos(x, y_coord, z);
				circle.push_back(pos);
			}
			circles.push_back(circle);
		}

		// Vertical circles.
		for (int y_rotate = 0; y_rotate < 180; y_rotate += 20)
		{
			std::vector<glm::vec3> circle;
			float y_angle = (y_rotate / 180.0f) * glm::pi<float>();
			float angle_division = (glm::pi<float>() * 2) / (4 * NUM_SPHERE_SEGMENTS);
			for (int clock = 0; clock < 4 * NUM_SPHERE_SEGMENTS; ++clock)
			{
				float angle = angle_division * clock;
				float x = cosf(angle) * RADIUS;
				float y = sinf(angle) * RADIUS;
				float z = x * sinf(y_angle);
				x *= cosf(y_angle);
				glm::vec3 pos(x, y, z);
				circle.push_back(pos);
			}
			circles.push_back(circle);
		}

		s_NumCirclesInSphere = static_cast<unsigned int>(circles.size());

		glGenVertexArrays(1, &s_DebugSphereVao);
		glBindVertexArray(s_DebugSphereVao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 3 * 4 * NUM_SPHERE_SEGMENTS * s_NumCirclesInSphere);
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

	void MakeDebugBox()
	{
		glGenVertexArrays(1, &s_DebugBoxVao);
		glBindVertexArray(s_DebugBoxVao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 3 * 24);
		float* vert_data = static_cast<float*>(MemNewBytes(MemoryPool::Rendering, data_size));
		float* f = vert_data;
		glm::vec3 DIMS(1.0f, 1.0f, 1.0f);
		glm::vec3 half_dims = DIMS / 2.0f;
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
			point = glm::vec3(glm::vec4(point, 1.0f));
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

	void MakeDebugCrosshair()
	{
		glGenVertexArrays(1, &s_DebugCrosshairVao);
		glBindVertexArray(s_DebugCrosshairVao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 3 * 6);
		float* vert_data = static_cast<float*>(MemNewBytes(MemoryPool::Rendering, data_size));
		float* f = vert_data;
		const float SCALE = 1.0f;
		std::vector<glm::vec3> line_points = {
			glm::vec3(-SCALE / 2, 0.0f, 0.0f), glm::vec3(SCALE / 2, 0.0f, 0.0f),
			glm::vec3(0.0f, -SCALE / 2, 0.0f), glm::vec3(0.0f, SCALE / 2, 0.0f),
			glm::vec3(0.0f, 0.0f, -SCALE / 2), glm::vec3(0.0f, 0.0f, SCALE / 2)
		};

		for (auto point : line_points)
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
	
	void MakeDebugArrow()
	{
		glGenVertexArrays(1, &s_DebugArrowVao);
		glBindVertexArray(s_DebugArrowVao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 3 * 6);
		float* vert_data = static_cast<float*>(MemNewBytes(MemoryPool::Rendering, data_size));
		float* f = vert_data;
		const float SCALE = 1.0f;
		std::vector<glm::vec3> line_points = {
			glm::vec3(-SCALE / 2, 0.0f, 0.0f), glm::vec3(SCALE / 2, 0.0f, 0.0f),
			glm::vec3(SCALE / 4, 0.0f, -SCALE / 4), glm::vec3(SCALE / 2, 0.0f, 0.0f),
			glm::vec3(SCALE / 4, 0.0f, SCALE / 4), glm::vec3(SCALE / 2, 0.0f, 0.0f)
		};

		for (auto point : line_points)
		{
			point = glm::vec3(glm::vec4(point, 1.0f));
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

	void Initialize()
	{
		s_Shader = ShaderManager::LoadProgram("DebugDraw");
		s_UniTransform = glGetUniformLocation(s_Shader, "world_transform");
		s_UniTint = glGetUniformLocation(s_Shader, "tint");

		MakeDebugLine();
		MakeDebugBox();
		MakeDebugSphere();
		MakeDebugCrosshair();
		MakeDebugArrow();
	}

	void Terminate()
	{

	}

	void Render()
	{
		ShaderManager::SetActiveShader(s_Shader);
		for (auto layer : s_DebugObjects)
		{
			for (auto object : layer.second)
			{
				glUniform3fv(s_UniTint, 1, glm::value_ptr(object->m_Colour));
				if (object->m_ExternalTransform)
				{
					glm::mat4 transform = *object->m_ExternalTransform * object->m_Transform;
					glUniformMatrix4fv(s_UniTransform, 1, GL_FALSE, glm::value_ptr(transform));
				}
				else
				{
					glUniformMatrix4fv(s_UniTransform, 1, GL_FALSE, glm::value_ptr(object->m_Transform));
				}
				object->Render();
			}
		}
	}

	void Release(IRenderableDebugObject* object)
	{
		for (auto layer : s_DebugObjects)
		{
			layer.second.erase(object);
		}
		MemDelete(object);
	}

	class _DebugLine : public IRenderableDebugObject
	{
	public:
		_DebugLine(const glm::vec3& end)
		{
			m_Vao = s_DebugLineVao;
			glm::vec3 up(1.0f, 0.0f, 0.0f);
			glm::vec3 xaxis = glm::cross(up, end);
			xaxis = glm::normalize(xaxis);

			glm::vec3 yaxis = glm::cross(end, xaxis);
			yaxis = glm::normalize(yaxis);

			glm::mat3 mat;
			mat[0][0] = xaxis.x;
			mat[0][1] = yaxis.x;
			mat[0][2] = end.x;

			mat[1][0] = xaxis.y;
			mat[1][1] = yaxis.y;
			mat[1][2] = end.y;

			mat[2][0] = xaxis.z;
			mat[2][1] = yaxis.z;
			mat[2][2] = end.z;

			m_Transform = mat;
			float mag = glm::length(end);
			m_Transform = glm::scale(m_Transform, glm::vec3(mag, mag, mag));
		}

		~_DebugLine()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glDrawArrays(GL_LINES, 0, 2);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}
	};

	IDebugObject* DebugLine(const std::string& layer, const glm::vec3& end)
	{
		_DebugLine* debug_object = MemNew(MemoryPool::Rendering, _DebugLine)(end);
		s_DebugObjects[layer].insert(debug_object);
		return debug_object;
	}

	class _DebugLines : public IRenderableDebugObject
	{
	public:
		_DebugLines(const std::vector<glm::vec3>& points)
		{
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
			glDrawArrays(GL_LINE_STRIP, 0, m_NumPoints);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		unsigned int m_NumPoints;
	};

	IDebugObject* DebugLines(const std::string& layer, const std::vector<glm::vec3>& points)
	{
		_DebugLines* debug_object = MemNew(MemoryPool::Rendering, _DebugLines)(points);
		s_DebugObjects[layer].insert(debug_object);
		return debug_object;
	}

	class _DebugGrid : public IRenderableDebugObject
	{
	public:
		_DebugGrid(float grid_square_size, unsigned int squares_across, unsigned int squares_deep)
		{
			m_NumLines = (squares_across + 1) + (squares_deep + 1);

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
				line_start = glm::vec3(glm::vec4(line_start, 1.0f));
				line_end = glm::vec3(glm::vec4(line_end, 1.0f));
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
				line_start = glm::vec3(glm::vec4(line_start, 1.0f));
				line_end = glm::vec3(glm::vec4(line_end, 1.0f));
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
			glDrawArrays(GL_LINES, 0, m_NumLines * 2);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		unsigned int m_NumLines;
	};

	IDebugObject* DebugGrid(const std::string& layer, float grid_square_size, unsigned int squares_across, unsigned int squares_deep)
	{
		_DebugGrid* debug_object = MemNew(MemoryPool::Rendering, _DebugGrid)(grid_square_size, squares_across, squares_deep);
		s_DebugObjects[layer].insert(debug_object);
		return debug_object;
	}

	class _DebugSphere : public IRenderableDebugObject
	{
	public:
		_DebugSphere(float radius)
		{
			m_Vao = s_DebugSphereVao;
			m_Scale = radius * 2.0f;
			m_Transform = glm::scale(glm::mat4(), glm::vec3(m_Scale, m_Scale, m_Scale));
		}

		~_DebugSphere()
		{
		}

		virtual void SetTransform(const glm::mat4& transform) override
		{
			m_Transform = glm::scale(transform, glm::vec3(m_Scale, m_Scale, m_Scale));
		}

		virtual void Render() override
		{
			glBindVertexArray(s_DebugSphereVao);
			for (unsigned int circle_idx = 0; circle_idx < s_NumCirclesInSphere; ++circle_idx)
			{
				int start_idx = circle_idx * (4 * NUM_SPHERE_SEGMENTS);
				glDrawArrays(GL_LINE_LOOP, start_idx, 4 * NUM_SPHERE_SEGMENTS);
			}
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		float m_Scale;
	};

	IDebugObject* DebugSphere(const std::string& layer, float radius)
	{
		_DebugSphere* debug_object = MemNew(MemoryPool::Rendering, _DebugSphere)(radius);
		s_DebugObjects[layer].insert(debug_object);
		return debug_object;
	}

	class _DebugBox : public IRenderableDebugObject
	{
	public:
		_DebugBox(const glm::vec3& dims)
		{
			m_Vao = s_DebugBoxVao;
			m_Transform = glm::scale(glm::mat4(), dims);
		}

		~_DebugBox()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(s_DebugBoxVao);
			glDrawArrays(GL_LINES, 0, 24);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}
	};

	IDebugObject* DebugBox(const std::string& layer, const glm::vec3& dims)
	{
		_DebugBox* debug_object = MemNew(MemoryPool::Rendering, _DebugBox)(dims);
		s_DebugObjects[layer].insert(debug_object);
		return debug_object;
	}

	class _DebugCrosshair : public IRenderableDebugObject
	{
	public:
		_DebugCrosshair()
		{
			m_Vao = s_DebugCrosshairVao;
		}

		~_DebugCrosshair()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glDrawArrays(GL_LINES, 0, 6);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}
	};

	IDebugObject* DebugCrosshair(const std::string& layer)
	{
		_DebugCrosshair* debug_object = MemNew(MemoryPool::Rendering, _DebugCrosshair);
		s_DebugObjects[layer].insert(debug_object);
		return debug_object;
	}

	class _DebugArrow : public IRenderableDebugObject
	{
	public:
		_DebugArrow()
		{
			m_Vao = s_DebugArrowVao;
		}

		~_DebugArrow()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glDrawArrays(GL_LINES, 0, 6);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		unsigned int m_NumPoints;
	};

	IDebugObject* DebugArrow(const std::string& layer)
	{
		_DebugArrow* debug_object = MemNew(MemoryPool::Rendering, _DebugArrow);
		s_DebugObjects[layer].insert(debug_object);
		return debug_object;
	}

	class _DebugCapsule : public IRenderableDebugObject
	{
	public:
		_DebugCapsule(float radius, float half_height)
		{
			m_NumLines = 0;

			glGenVertexArrays(1, &m_Vao);
			glBindVertexArray(m_Vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			// We want lines around the length of the capsule.
			std::vector<glm::vec3> verts;

			std::vector<glm::vec3> one_end;
			std::vector<glm::vec3> other_end;
			for (int i = 0; i < 360; i += 20)
			{
				float angle = (i / 180.0f) * glm::pi<float>();
				glm::vec3 from = glm::vec3(half_height, radius * sin(angle), radius * cos(angle));
				glm::vec3 to = glm::vec3(-half_height, radius * sin(angle), radius * cos(angle));
				one_end.push_back(from);
				other_end.push_back(to);
			}

			for (int i = 0; i < one_end.size(); ++i)
			{
				verts.push_back(one_end[i]);
				verts.push_back(other_end[i]);
				++m_NumLines;
				verts.push_back(one_end[i]);
				verts.push_back(one_end[(i + 1) % one_end.size()]);
				++m_NumLines;
				verts.push_back(other_end[i]);
				verts.push_back(other_end[(i + 1) % other_end.size()]);
				++m_NumLines;
			}

			unsigned int data_size = static_cast<unsigned int>(sizeof(float) * 3 * verts.size());
			float* vert_data = static_cast<float*>(MemNewBytes(MemoryPool::Rendering, data_size));
			float* f = vert_data;
			for (auto v : verts)
			{
				*f++ = v.x;
				*f++ = v.y;
				*f++ = v.z;
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

		~_DebugCapsule()
		{
		}

		virtual void Render() override
		{
			glBindVertexArray(m_Vao);
			glDrawArrays(GL_LINES, 0, m_NumLines * 2);
		}

		virtual void Release() override
		{
			DebugDraw::Release(this);
		}

	protected:
		unsigned int m_NumLines;
	};

	IDebugObject* DebugCapsule(const std::string& layer, float radius, float half_height)
	{
		_DebugCapsule* debug_object = MemNew(MemoryPool::Rendering, _DebugCapsule)(radius, half_height);
		s_DebugObjects[layer].insert(debug_object);
		return debug_object;
	}
}