#include <glew.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <memory>

#include "engine.h"
#include "render.h"

struct Vertex
{
	glm::vec3 pos;
	
	Vertex() {}
	
	Vertex(glm::vec3 p)
	{
		pos = p;
	}
};

const char* vsSource = R"(
						#version 330 core
						uniform mat4 MVP;
						in vec3 position;
						void main(void)
						{
							gl_Position = MVP * vec4(position, 1.0);
						}
					)";

const char* fsSource = R"(
						#version 330 core
						uniform sampler2D colorTexture;
						out vec4 color;
						void main(void)
						{
							color = vec4 (1.0, 1.0, 1.0, 1.0);
						}
					)";

glm::vec2 SCREEN_SIZE(800, 600);

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelMatrix;

glm::mat4 MVP;

static const char* windowName = "Engine";

std::unique_ptr<Engine> eng;
Render::ProgramPtr program;
Render::MeshRenderObjectPtr mesh;

Render::MeshRenderObjectPtr CreateMesh(GLenum type, const void* vertData, size_t verticesSize, size_t vertexSize, const void* indData, size_t indiciesSize, size_t indCount)
{
	return std::make_unique<Render::MeshRenderObject>(type, vertData, verticesSize, vertexSize, indData, indiciesSize, indCount);
}

int main(int argc, char* args[])
{
	eng = std::make_unique<Engine>(windowName, SCREEN_SIZE.x, SCREEN_SIZE.y);

	program = Render::CreateProgram(vsSource, fsSource);
	program->Use();

	// Setup matrix

	const float aspectRatio = SCREEN_SIZE.x / SCREEN_SIZE.y;
	projectionMatrix = glm::perspective(45.0f, aspectRatio, 1.0f, 10.0f);

	modelMatrix = glm::mat4(1.0f);

	viewMatrix = glm::lookAt(
		glm::vec3(0, 0, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Vertices

	const float s = 1.0f;
	const Vertex cubeVertices[] =
	{
		// front
		Vertex(glm::vec3(-s, s, s)),
		Vertex(glm::vec3(s, s, s)),
		Vertex(glm::vec3(s,-s, s)),
		Vertex(glm::vec3(-s,-s, s)),

		// back
		Vertex(glm::vec3(s, s,-s)),
		Vertex(glm::vec3(-s, s,-s)),
		Vertex(glm::vec3(-s,-s,-s)),
		Vertex(glm::vec3(s,-s,-s)),

		// top
		Vertex(glm::vec3(-s, s,-s)),
		Vertex(glm::vec3(s, s,-s)),
		Vertex(glm::vec3(s, s, s)),
		Vertex(glm::vec3(-s, s, s)),

		// bottom
		Vertex(glm::vec3(s,-s,-s)),
		Vertex(glm::vec3(-s,-s,-s)),
		Vertex(glm::vec3(-s,-s, s)),
		Vertex(glm::vec3(s,-s, s)),

		// left
		Vertex(glm::vec3(-s, s,-s)),
		Vertex(glm::vec3(-s, s, s)),
		Vertex(glm::vec3(-s,-s, s)),
		Vertex(glm::vec3(-s,-s,-s)),

		// right
		Vertex(glm::vec3(s, s, s)),
		Vertex(glm::vec3(s, s,-s)),
		Vertex(glm::vec3(s,-s,-s)),
		Vertex(glm::vec3(s,-s, s)),
	};

	const uint32_t cubeIndices[] =
	{
		0, 3, 1,  1, 3, 2, // front
		4, 7, 5,  5, 7, 6, // back
		8,11, 9,  9,11,10, // top
		12,15,13, 13,15,14, // bottom
		16,19,17, 17,19,18, // left
		20,23,21, 21,23,22  // right
	};

	mesh = CreateMesh(GL_TRIANGLES, cubeVertices, sizeof(cubeVertices), sizeof(Vertex), cubeIndices, sizeof(cubeIndices), 36);

	GLint mvpUniformLoc = program->GetUniformLoc("MVP");

	bool running = true;

	while (eng->Run())
	{
		glClear(GL_COLOR_BUFFER_BIT);

		Render::SetProgram(program);

		MVP = projectionMatrix*viewMatrix*modelMatrix;
		if (mvpUniformLoc != -1)
			program->SetUniform(mvpUniformLoc, MVP);

		Render::SetVertexArray(mesh->vao);
		Render::DrawIndexed(GL_TRIANGLES, mesh->indicesCount);

		eng->SwapBuffers();
	}

	return 0;
}