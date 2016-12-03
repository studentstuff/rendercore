#pragma once

#include <glew.h>
#include <glm.hpp>
#include <memory>

namespace Render
{
	struct ShaderProgram
	{
		ShaderProgram(const char* vsSource, const char* fsSource);

		void Use();
		void Unuse();

		GLint GetUniformLoc(const char* uniformName);
		void SetUniform(GLint loc, glm::mat4& mat);

	private:
		GLuint glProgram;
	};
	typedef std::unique_ptr<ShaderProgram> ProgramPtr;

	struct VertexBuffer
	{
		VertexBuffer(const void* data, size_t size);

	private:
		GLuint glVbo = 0;
	};
	typedef std::unique_ptr<VertexBuffer> VertexBufferPtr;

	struct IndexBuffer
	{
		IndexBuffer(const void* data, size_t size);

	private:
		GLuint ibo = 0;
	};
	typedef std::unique_ptr<IndexBuffer> IndexBufferPtr;

	struct VertexArray
	{
		VertexArray();

		void SetupAttrib(int attrib, size_t size, GLenum type, GLboolean normalized, size_t vertexSize, const GLvoid* offset);

		void Use();
		void Unuse();

	private:
		GLuint glVao = 0;
	};
	typedef std::unique_ptr<VertexArray> VertexArrayPtr;

	struct Frame
	{
		glm::mat4 modelMatrix;
	};

	struct RenderObject
	{
		Frame frame;

		virtual ~RenderObject() {}

		virtual void Draw() = 0;
	};
	typedef std::unique_ptr<RenderObject> RenderObjectPtr;

	struct MeshRenderObject : public RenderObject
	{
		MeshRenderObject(GLenum type, const void* vertData, size_t verticesSize, size_t vertexSize, const void* indData, size_t indiciesSize, size_t indCount);

		VertexArrayPtr  vao;
		VertexBufferPtr vbo;
		IndexBufferPtr  ibo;

		GLenum primitiveType;
		int indicesCount;

		virtual void Draw();
	};
	typedef std::unique_ptr<MeshRenderObject> MeshRenderObjectPtr;

	ProgramPtr CreateProgram(const char* vsSource, const char* fsSource);

	VertexArrayPtr  CreateVertexArray();
	VertexBufferPtr CreateVertexBuffer(const void* data, size_t size);
	IndexBufferPtr  CreateIndexBuffer(const void* data, size_t size);

	void SetVertexArray(VertexArrayPtr& vao);
	void SetProgram(ProgramPtr& program);

	void DrawIndexed(GLenum primitiveType, int indCount);
}
