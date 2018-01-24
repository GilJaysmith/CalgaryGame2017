#pragma once

#include <map>
#include <string>
#include <vector>

namespace ShaderManager
{
	void Initialize();

	void Terminate();

	unsigned int LoadProgram(const std::string& filename);

	void SetUniform4fv(const std::string& uniform_name, const glm::mat4& v);
}
