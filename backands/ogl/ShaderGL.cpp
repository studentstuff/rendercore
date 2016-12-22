#include "opengl_common.h"
#include "Render.h"
#include "Shader.h"

namespace StandartShaders
{
	ProgramPtr DiffuseShader()
	{
		static const char* diffuseVs = R"(
						#version 330 core
						uniform mat4 MVP;
						in vec3 position;
						in vec2 uv;
						out vec2 fragTexcoord;
						void main(void)
						{
							gl_Position = MVP * vec4(position, 1.0);
							fragTexcoord = uv;
						}
					)";

		static const char* diffuseFs = R"(
						#version 330 core
						uniform sampler2D diffuseTexture;
						in vec2 fragTexcoord;
						out vec4 color;
						void main(void)
						{
							color = texture(diffuseTexture, fragTexcoord);;
						}
					)";

		return Render::CreateProgram(diffuseVs, diffuseFs);
	}
}
