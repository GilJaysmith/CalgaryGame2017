#pragma once

#include <map>
#include <string>
#include <vector>

namespace ShaderManager
{
	void Initialize();

	void Terminate();

	unsigned int LoadShader(const std::string& filename, unsigned int shader_type);
	unsigned int MakeProgram(const std::vector<unsigned int>& shaders, const std::map<unsigned int, std::string>& buffer_outputs);
}
