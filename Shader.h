#pragma once

#include <cstdint>
#include <gl\glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <map>
#include <memory>

enum struct StandardUniforms : uint16_t
{
	Mvp,
	DiffuseMap
};

struct ShaderProgram
{
	ShaderProgram() {}
	ShaderProgram(const char* vsSource, const char* fsSource) {}
	virtual ~ShaderProgram() {}

	virtual void SetUniform(StandardUniforms uniform, glm::mat4& mat) = 0;

protected:
	std::map<StandardUniforms, int> uniforms;
};
typedef std::shared_ptr<ShaderProgram> ProgramPtr;

namespace StandartShaders
{
	ProgramPtr DiffuseShader();
}
