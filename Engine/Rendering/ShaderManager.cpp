#include "Engine/Pch.h"

#include <fstream>
#include <istream>
#include <sstream>

#include "Engine/Logging/Logging.h"

namespace ShaderManager
{
	std::map<std::pair<std::string, unsigned int>, unsigned int> s_Shaders;

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

		std::string filename = "data/shaders/" + shader_name + ".glsl";

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
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			Logging::Log("ShaderManager", std::string("shader compilation error: ") + shader_name + ": " + infoLog);
			return 0;
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
		glGetShaderiv(shader_program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader_program, 512, NULL, infoLog);
			Logging::Log("ShaderManager", std::string("program compilation error: ") + infoLog);
			return 0;
		}

		return shader_program;
	}
}