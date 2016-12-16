#include <gl\glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

#include "Engine.h"
#include "utils.h"

#define GLM_FORCE_RADIANS
#undef GL_OFFSET
#define GL_OFFSET(x) ((const GLvoid *)(x))

SDL_Window* _window = nullptr;
SDL_GLContext _context = nullptr;

glm::vec2 SCREEN_SIZE(1024, 768);

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
glm::mat4 modelMatrix;
glm::mat4 MVP;

static GLuint colorTexture = 0;

struct Vertex
{
	glm::vec3 m_pos;
	glm::vec2 m_tex;

	Vertex() {}

	Vertex(glm::vec3 pos, glm::vec2 tex)
	{
		m_pos = pos;
		m_tex = tex;
	}
};

int main(int argc, char* args[])
{
	Engine eng("Engine", SCREEN_SIZE.x, SCREEN_SIZE.y);

	const float aspectRatio = SCREEN_SIZE.x / SCREEN_SIZE.y;
	projectionMatrix = glm::perspective(45.0f, aspectRatio, 1.0f, 10.0f);

	const float s = 1.0f;
	const Vertex vertices[] =
	{
		// front
		Vertex(glm::vec3(-s, s, s), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(s, s, s), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(s,-s, s), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-s,-s, s), glm::vec2(0.0f, 0.0f)),

		// back
		Vertex(glm::vec3(s, s,-s), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-s, s,-s), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(-s,-s,-s), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(s,-s,-s), glm::vec2(0.0f, 0.0f)),

		// top
		Vertex(glm::vec3(-s, s,-s), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(s, s,-s), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(s, s, s), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-s, s, s), glm::vec2(0.0f, 0.0f)),

		// bottom
		Vertex(glm::vec3(s,-s,-s), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-s,-s,-s), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(-s,-s, s), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(s,-s, s), glm::vec2(0.0f, 0.0f)),

		// left
		Vertex(glm::vec3(-s, s,-s), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-s, s, s), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(-s,-s, s), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-s,-s,-s), glm::vec2(0.0f, 0.0f)),

		// right
		Vertex(glm::vec3(s, s, s), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(s, s,-s), glm::vec2(1.0f, 1.0f)),
		Vertex(glm::vec3(s,-s,-s), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(s,-s, s), glm::vec2(0.0f, 0.0f)),
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

	modelMatrix = glm::mat4(1.0f);
	viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.5f, -7.0f));

	GLuint glVao;
	glGenVertexArrays(1, &glVao);
	glBindVertexArray(glVao);

	GLuint glVbo;
	glGenBuffers(1, &glVbo);
	glBindBuffer(GL_ARRAY_BUFFER, glVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, NULL);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, GL_OFFSET(sizeof(float)*3));

	GLuint glIbo;
	glGenBuffers(1, &glIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	colorTexture = TextureCreateFromTGA("texture.tga");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	const char* vsSource = R"(
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

	const char* fsSource = R"(
						#version 330 core
						uniform sampler2D colorTexture;
						in vec2 fragTexcoord;
						out vec4 color;
						void main(void)
						{
							color = texture(colorTexture, fragTexcoord);;
						}
					)";

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vsSource, nullptr);
	glCompileShader(vertexShaderID);

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fsSource, nullptr);
	glCompileShader(fragmentShaderID);

	GLuint glProgram = glCreateProgram();
	glAttachShader(glProgram, vertexShaderID);
	glAttachShader(glProgram, fragmentShaderID);
	glLinkProgram(glProgram);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	GLint mvpUniformLoc = glGetUniformLocation(glProgram, "MVP");
	glUniform1i(glGetUniformLocation(glProgram, "colorTexture"), 0);

	while (eng.Run())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(glProgram);

		MVP = projectionMatrix*viewMatrix*modelMatrix;
		if (mvpUniformLoc != -1)
			glUniformMatrix4fv(mvpUniformLoc, 1, GL_FALSE, glm::value_ptr(MVP));

		glBindVertexArray(glVao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);

		glUseProgram(0);

		eng.SwapBuffers();
	}

	return 0;
}