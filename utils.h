#pragma once

#include <cstdio>
#include <stdint.h>

#include "gl/glew.h"

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

// ������ ��������� TGA-�����
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

// ������� �������� �� TGA-����� � ������������
static GLuint TextureCreateFromTGA(const char *fileName)
{
	TGAHeader *header;
	uint8_t   *buffer;
	uint32_t  size;
	GLint     format, internalFormat;
	GLuint    texture;

	// ���������� ��������� �������� �� �����
	if (!LoadFile(fileName, true, &buffer, &size))
		return 0;

	// ���� ������ ����� �������� ������ ��������� TGA
	if (size <= sizeof(TGAHeader))
	{
		printf("Too small file '%s'\n", fileName);
		delete[] buffer;
		return 0;
	}

	header = (TGAHeader*)buffer;

	// �������� ������ TGA-����� - �������� RGB ��� RGBA �����������
	if (header->datatype != 2 || (header->bitperpel != 24 && header->bitperpel != 32))
	{
		printf("Wrong TGA format '%s'\n", fileName);
		delete[] buffer;
		return 0;
	}

	// ������� ������ ��������
	format = (header->bitperpel == 24 ? GL_BGR : GL_BGRA);
	internalFormat = (format == GL_BGR ? GL_RGB8 : GL_RGBA8);

	// �������� � OpenGL ��������� ������ ��������
	glGenTextures(1, &texture);

	// ������� �������� ��������
	glBindTexture(GL_TEXTURE_2D, texture);

	// ��������� ��������� ���������� �������� - �������� ����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// ��������� ��������� "�������������" �������� - ���������� ������������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// �������� ������ � ����� � ������� �������� ��������
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, header->width, header->height, 0, format,
		GL_UNSIGNED_BYTE, (const GLvoid*)(buffer + sizeof(TGAHeader) + header->idlength));

	// ����� �������� � �������� ������ � ����� � ������ ��� ������ �� �����
	delete[] buffer;

	// �������� �� ������� ������
	//OPENGL_CHECK_FOR_ERRORS();

	return texture;
}

// ������� ��������
static void TextureDestroy(GLuint texture)
{
	// ��������� ������� ������ ��������
	glDeleteTextures(1, &texture);
}