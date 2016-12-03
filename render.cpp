#include <cstdio>
#include <vector>
#include <gtc/type_ptr.hpp>

#include "render.h"

namespace Render
{
	GLuint createShader(GLenum shaderType, const char* source)
	{
		GLuint glShader = glCreateShader(shaderType);
		glShaderSource(glShader, 1, &source, nullptr);
		glCompileShader(glShader);

		GLint status = GL_FALSE;
		GLint infoLogLength = 0;

		glGetShaderiv(glShader, GL_COMPILE_STATUS, &status);
		glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 1)
		{
			std::vector<GLchar> message(infoLogLength);
			glGetShaderInfoLog(glShader, infoLogLength, NULL, &message[0]);
			printf("CheckCompileStatus: %s\n", &message[0]);
		}

		return glShader;
	}

	ShaderProgram::ShaderProgram(const char* vsSource, const char* fsSource)
	{
		GLuint vertexShaderID = createShader(GL_VERTEX_SHADER, vsSource);
		GLuint fragmentShaderID = createShader(GL_FRAGMENT_SHADER, fsSource);

		glProgram = glCreateProgram();
		glAttachShader(glProgram, vertexShaderID);
		glAttachShader(glProgram, fragmentShaderID);
		glLinkProgram(glProgram);

		GLint status = GL_FALSE;
		GLint infoLogLength = 0;

		glGetProgramiv(glProgram, GL_LINK_STATUS, &status);
		glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 1)
		{
			std::vector<char> message(infoLogLength);
			glGetProgramInfoLog(glProgram, infoLogLength, NULL, &message[0]);
			printf("Shader program: %s\n", &message[0]);
		}

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
	}

	void ShaderProgram::Use()
	{
		glUseProgram(glProgram);
	}

	void ShaderProgram::Unuse()
	{
		glUseProgram(0);
	}

	GLint ShaderProgram::GetUniformLoc(const char* uniformName)
	{
		return glGetUniformLocation(glProgram, uniformName);
	}

	void ShaderProgram::SetUniform(GLint loc, glm::mat4& mat)
	{
		glUniformMatrix4fv(loc, 1, GL_TRUE, glm::value_ptr(mat));
	}

	VertexBuffer::VertexBuffer(const void* data, size_t size)
	{
		glGenBuffers(1, &glVbo);
		glBindBuffer(GL_ARRAY_BUFFER, glVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(size), data, GL_STATIC_DRAW);
	}

	IndexBuffer::IndexBuffer(const void* data, size_t size)
	{
		GLuint ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(size), data, GL_STATIC_DRAW);
	}

	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &glVao);
		glBindVertexArray(glVao);
	}

	void VertexArray::SetupAttrib(int attrib, size_t size, GLenum type, GLboolean normalized, size_t vertexSize, const GLvoid* offset)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(attrib, size, type, normalized, vertexSize, offset);
	}

	void VertexArray::Use()
	{
		glBindVertexArray(glVao);
	}

	void VertexArray::Unuse()
	{
		glBindVertexArray(0);
	}

	void MeshRenderObject::Draw()
	{
		vao->Use();
		glDrawElements(primitiveType, indicesCount, GL_UNSIGNED_INT, NULL);
		vao->Unuse();
	}

	MeshRenderObject::MeshRenderObject(GLenum type, const void* vertData, size_t verticesSize, size_t vertexSize, const void* indData, size_t indiciesSize, size_t indCount) : 
		primitiveType(type), indicesCount(indCount)
	{
		vao = CreateVertexArray();
		vao->SetupAttrib(0, sizeof(glm::vec3), GL_FLOAT, GL_FALSE, vertexSize, 0);

		vbo = CreateVertexBuffer(vertData, verticesSize);
		ibo = CreateIndexBuffer(indData, indiciesSize);
	}

	ProgramPtr CreateProgram(const char* vsSource, const char* fsSource)
	{
		return std::make_unique<ShaderProgram>(vsSource, fsSource);
	}

	VertexArrayPtr  CreateVertexArray()
	{
		return std::make_unique<VertexArray>();
	}

	VertexBufferPtr CreateVertexBuffer(const void* data, size_t size)
	{
		return std::make_unique<VertexBuffer>(data, size);
	}

	IndexBufferPtr  CreateIndexBuffer(const void* data, size_t size)
	{
		return std::make_unique<IndexBuffer>(data, size);
	}

	void SetVertexArray(VertexArrayPtr& vao)
	{
		vao->Use();
	}

	void SetProgram(ProgramPtr& program)
	{
		program->Use();
	}

	void DrawIndexed(GLenum primitiveType, int indCount)
	{
		
	}
}