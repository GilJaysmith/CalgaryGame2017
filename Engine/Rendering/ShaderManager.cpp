#include "Engine/Pch.h"

#include <fstream>
#include <istream>
#include <sstream>

#include "Engine/Rendering/ShaderManager.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"

namespace ShaderManager
{
	std::map<std::pair<std::string, unsigned int>, unsigned int> s_Shaders;
	std::map<GLuint, std::map<std::string, GLint>> s_ShaderUniforms;

	void Initialize()
	{

	}

	void Terminate()
	{
		for (auto it : s_Shaders)
		{
			glDeleteShader(it.second);
		}
		s_Shaders.clear();
	}

	unsigned int LoadShader(const std::string& shader_name, unsigned int shader_type)
	{
		auto it = s_Shaders.find(std::pair<std::string, unsigned int>(shader_name, shader_type));
		if (it != s_Shaders.end())
		{
			return it->second;
		}

		std::string filename = "data/shaders/" + shader_name;

		std::ifstream in(filename);
		std::stringstream buffer;
		buffer << in.rdbuf();
		std::string contents(buffer.str());
		const char* shader_text = contents.c_str();

		unsigned int shader = glCreateShader(shader_type);
		glShaderSource(shader, 1, &shader_text, nullptr);

		glCompileShader(shader);

		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			Logging::Log("ShaderManager", std::string("shader compilation error: ") + shader_name + ": " + infoLog);
			exit(1);
		}

		s_Shaders[std::pair<std::string, unsigned int>(shader_name, shader_type)] = shader;
		return shader;
	}

	unsigned int MakeProgram(const std::vector<unsigned int>& shaders, const std::map<unsigned int, std::string>& buffer_outputs)
	{
		unsigned int shader_program = glCreateProgram();
		for (auto shader : shaders)
		{
			glAttachShader(shader_program, shader);
		}

		for (auto it : buffer_outputs)
		{
			glBindFragDataLocation(shader_program, it.first, it.second.c_str());
		}

		glLinkProgram(shader_program);

		int success;
		char infoLog[512];
		glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader_program, 512, NULL, infoLog);
			Logging::Log("ShaderManager", std::string("program compilation error: ") + infoLog);
			exit(1);
		}

		return shader_program;
	}

	std::map<std::string, unsigned int> s_Programs;

	unsigned int LoadProgram(const std::string& program_name)
	{
		auto it = s_Programs.find(program_name);
		if (it != s_Programs.end())
		{
			return it->second;
		}

		std::string program_filename = "data/shaders/" + program_name + ".yaml";
		YAML::Node node = YAML::LoadFile(program_filename);

		std::vector<unsigned int> shaders;
		for (auto shader : node["shaders"])
		{
			std::string shader_type_string = shader["type"].as<std::string>();
			auto shader_type = shader_type_string == "vertex" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
			auto shader_name = shader["shader"].as<std::string>();
			shaders.push_back(ShaderManager::LoadShader(shader_name, shader_type));
		}

		std::map<unsigned int, std::string> outputs;
		for (unsigned int i = 0; i < node["outputs"].size(); ++i)
		{
			outputs[i] = node["outputs"][i].as<std::string>();
		}

		GLuint shader_program = ShaderManager::MakeProgram(shaders, outputs);
		s_Programs[program_name] = shader_program;

		return shader_program;
	}

	unsigned int s_ActiveShader = -1;

	void SetActiveShader(unsigned int shader_program)
	{
		if (shader_program != s_ActiveShader)
		{
			glUseProgram(shader_program);
			s_ActiveShader = shader_program;
		}
	}

	GLint GetUniformLocation(GLuint program, const std::string& uniform_name)
	{
		std::map<std::string, GLint>& uniforms = s_ShaderUniforms[program];
		auto it = uniforms.find(uniform_name);
		if (it != uniforms.end())
		{
			return it->second;
		}
		GLint uniform_location = glGetUniformLocation(program, uniform_name.c_str());
		s_ShaderUniforms[program][uniform_name] = uniform_location;
		return uniform_location;
	}

	void SetUniformMatrix4fv(const std::string& uniform_name, const glm::mat4& v)
	{
		for (auto shader : s_Programs)
		{
			unsigned int shader_program = shader.second;
			SetUniformMatrix4fv(uniform_name, v, shader_program);
		}
	}
	
	void SetUniformMatrix4fv(const std::string& uniform_name, const glm::mat4& v, unsigned int shader_program)
	{
		GLint uniformLocation = GetUniformLocation(shader_program, uniform_name.c_str());
		if (uniformLocation > -1)
		{
			SetActiveShader(shader_program);
			glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(v));
		}
	}

	void SetUniform3fv(const std::string& uniform_name, const glm::vec3& v)
	{
		for (auto shader : s_Programs)
		{
			unsigned int shader_program = shader.second;
			GLint uniformLocation = GetUniformLocation(shader_program, uniform_name.c_str());
			if (uniformLocation > -1)
			{
				SetActiveShader(shader_program);
				glUniform3fv(uniformLocation, 1, glm::value_ptr(v));
			}
		}
	}

}