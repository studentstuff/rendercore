#pragma once

#include <cstdio>
#include <stdint.h>

#include "gl/glew.h"
#include "Render.h"

#define GLM_FORCE_RADIANS

#undef GL_OFFSET
#define GL_OFFSET(x) ((const GLvoid *)(x))

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

static Render::RenderObject* CreateCubeRO(float size, GLuint program)
{
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
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, GL_OFFSET(sizeof(float) * 3));

	GLuint glIbo;
	glGenBuffers(1, &glIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	return new Render::RenderObject(glVao, program, 36);
}

GLuint CreateProgram()
{
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

	glUniform1i(glGetUniformLocation(glProgram, "colorTexture"), 0);

	return glProgram;
}

static GLuint TextureCreateFromTGA(const char *fileName);

GLuint CreateTexture(const char* name)
{
	GLuint glTexture = TextureCreateFromTGA(name);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glTexture);

	return glTexture;
}

static bool LoadFile(const char *fileName, bool binary, uint8_t **buffer, uint32_t *size)
{
	FILE     *input;
	uint32_t fileSize, readed;

	const char mode[] = { 'r', binary ? 'b' : 't', '\0' };

	if ((input = fopen(fileName, mode)) == NULL)
	{
		printf("Opening file '%s'\n", fileName);
		return false;
	}

	fseek(input, 0, SEEK_END);
	fileSize = (uint32_t)ftell(input);
	rewind(input);

	if (fileSize == 0)
	{
		printf("Empty file '%s'\n", fileName);
		fclose(input);
		return false;
	}

	*buffer = new uint8_t[fileSize];

	readed = fread(*buffer, 1, fileSize, input);

	fclose(input);

	if (readed != fileSize)
	{
		printf("Reading file '%s'\n", fileName);
		delete[] * buffer;
		return false;
	}

	*size = fileSize;

	return true;
}

// формат заголовка TGA-файла
#pragma pack(1)
struct TGAHeader
{
	uint8_t  idlength;
	uint8_t  colormap;
	uint8_t  datatype;
	uint8_t  colormapinfo[5];
	uint16_t xorigin;
	uint16_t yorigin;
	uint16_t width;
	uint16_t height;
	uint8_t  bitperpel;
	uint8_t  description;
};
#pragma pack()

// создает текстуру из TGA-файла с изображением
static GLuint TextureCreateFromTGA(const char *fileName)
{
	TGAHeader *header;
	uint8_t   *buffer;
	uint32_t  size;
	GLint     format, internalFormat;
	GLuint    texture;

	// попытаемся загрузить текстуру из файла
	if (!LoadFile(fileName, true, &buffer, &size))
		return 0;

	// если размер файла заведомо меньше заголовка TGA
	if (size <= sizeof(TGAHeader))
	{
		printf("Too small file '%s'\n", fileName);
		delete[] buffer;
		return 0;
	}

	header = (TGAHeader*)buffer;

	// проверим формат TGA-файла - несжатое RGB или RGBA изображение
	if (header->datatype != 2 || (header->bitperpel != 24 && header->bitperpel != 32))
	{
		printf("Wrong TGA format '%s'\n", fileName);
		delete[] buffer;
		return 0;
	}

	// получим формат текстуры
	format = (header->bitperpel == 24 ? GL_BGR : GL_BGRA);
	internalFormat = (format == GL_BGR ? GL_RGB8 : GL_RGBA8);

	// запросим у OpenGL свободный индекс текстуры
	glGenTextures(1, &texture);

	// сделаем текстуру активной
	glBindTexture(GL_TEXTURE_2D, texture);

	// установим параметры фильтрации текстуры - линейная фильтрация
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// установим параметры "оборачиваниея" текстуры - отсутствие оборачивания
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// загрузим данные о цвете в текущую автивную текстуру
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, header->width, header->height, 0, format,
		GL_UNSIGNED_BYTE, (const GLvoid*)(buffer + sizeof(TGAHeader) + header->idlength));

	// после загрузки в текстуру данные о цвете в памяти нам больше не нужны
	delete[] buffer;

	// проверим на наличие ошибок
	//OPENGL_CHECK_FOR_ERRORS();

	return texture;
}

// удаляет текстуру
static void TextureDestroy(GLuint texture)
{
	// освободим занятый индекс текстуры
	glDeleteTextures(1, &texture);
}