#pragma once

#include <map>
#include <string>
#include <vector>

namespace ShaderManager
{
	void Initialize();

	void Terminate();

	unsigned int LoadProgram(const std::string& filename);

	struct AttributeBinding
	{
		std::string name;
		int num_floats;
	};

	const std::vector<AttributeBinding>& GetAttributeBindings(unsigned int shader_program);
}
