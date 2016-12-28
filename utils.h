#pragma once

#include <cstdio>
#include <stdint.h>
#include <memory>

#include "gl/glew.h"
#include "Render.h"
#include "Shader.h"

#define GLM_FORCE_RADIANS

static Render::RenderObject* CreateCubeRO(float size, ProgramPtr program)
{
	const float s = size;
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

	const uint32_t indicies[] =
	{
		0, 3, 1,  1, 3, 2, // front
		4, 7, 5,  5, 7, 6, // back
		8,11, 9,  9,11,10, // top
		12,15,13, 13,15,14, // bottom
		16,19,17, 17,19,18, // left
		20,23,21, 21,23,22  // right
	};

	VertexFormat format;
	format.Add(ComponentUsage::Position, ComponentType::Float3);
	format.Add(ComponentUsage::Uv, ComponentType::Float2);

	VertexArrayPtr vertexArray = Render::CreateVertexArray(vertices, sizeof(vertices), indicies, sizeof(indicies), 36, format);
	return new Render::RenderObject(vertexArray, program);
}

static TexturePtr CreateTexture(const char *fileName);

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
static TexturePtr CreateTexture(const char *fileName)
{
	TGAHeader *header;
	uint8_t   *buffer;
	uint32_t  size;

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

	TextureDesc desc;
	desc.format = header->bitperpel == 24 ? TextureFormat::Bgr : TextureFormat::Bgra;
	desc.formatInternal = desc.format == TextureFormat::Bgr ? TextureFormat::Rgb8 : TextureFormat::Rgba8;
	desc.filter = TextureFilter::Linear;
	desc.wrap = TextureWrap::ClampToEdge;
	desc.width = header->width;
	desc.height = header->height;
	desc.data = (buffer + sizeof(TGAHeader) + header->idlength);

	//FIXME : memleak
	//delete[] buffer;

	return Render::CreateTexture(desc);
}
