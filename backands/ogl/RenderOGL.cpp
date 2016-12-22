#include "Render.h"
#include "opengl_common.h"

#ifdef GL_OFFSET
#undef GL_OFFSET
#endif
#define GL_OFFSET(x) ((const GLvoid *)(x))

namespace Render
{
	static const GLenum glFilter[] =
	{
		GL_LINEAR
	};

	static const GLenum glWrap[] =
	{
		GL_CLAMP_TO_EDGE
	};

	static const GLenum glFormat[] = 
	{
		GL_BGR,
		GL_BGRA,

		GL_RGB8,
		GL_RGBA8
	};

	enum struct TextureFormat : uint8_t
	{
		Bgr,
		Bgra,

		Rgb8,
		Rgba8
	};

	bool checkShaderStatus(GLuint glShader, GLenum param)
	{
		GLint  status, length;
		GLchar buffer[1024];

		glGetShaderiv(glShader, param, &status);

		if (status != GL_TRUE)
		{
			glGetShaderInfoLog(glShader, sizeof(buffer), &length, buffer);
			printf("Shader: %s\n", (const char *)buffer);
		}

		return status;
	}

	struct ShaderProgramGL : public ShaderProgram
	{
		ShaderProgramGL(const char* vsSource, const char* fsSource) :ShaderProgram(vsSource, fsSource)
		{
			GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShaderID, 1, &vsSource, nullptr);
			glCompileShader(vertexShaderID);

			checkShaderStatus(vertexShaderID, GL_COMPILE_STATUS);

			GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShaderID, 1, &fsSource, nullptr);
			glCompileShader(fragmentShaderID);

			checkShaderStatus(fragmentShaderID, GL_COMPILE_STATUS);

			glProgram = glCreateProgram();
			glAttachShader(glProgram, vertexShaderID);
			glAttachShader(glProgram, fragmentShaderID);
			glLinkProgram(glProgram);

			glDeleteShader(vertexShaderID);
			glDeleteShader(fragmentShaderID);

			glUseProgram(glProgram);

			GLuint loc = glGetUniformLocation(glProgram, "MVP");

			uniforms[StandardUniforms::Mvp] = glGetUniformLocation(glProgram, "MVP");
			uniforms[StandardUniforms::DiffuseMap] = glGetUniformLocation(glProgram, "diffuseTexture");

			glUniform1i(glGetUniformLocation(glProgram, "diffuseTexture"), 0);
		}

		GLuint glProgram;

		virtual void SetUniform(StandardUniforms uniform, glm::mat4& mat)
		{
			glUniformMatrix4fv(uniforms[uniform], 1, GL_FALSE, glm::value_ptr(mat));
		}
	};

	struct TextureGL : public Texture
	{
		TextureGL(TextureDesc& desc) : Texture(desc)
		{
			glGenTextures(1, &glTexture);
			glBindTexture(GL_TEXTURE_2D, glTexture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter[(int)desc.filter]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter[(int)desc.filter]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap[(int)desc.wrap]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap[(int)desc.wrap]);

			glTexImage2D(GL_TEXTURE_2D, 0, glFormat[(int)desc.formatInternal], desc.width, desc.height, 0, glFormat[(int)desc.format],
				GL_UNSIGNED_BYTE, desc.data);
		}

		GLuint glTexture;
	};

	struct VertexArrayGL : VertexArray
	{
		VertexArrayGL(const void* vertData, size_t vertSize, const void* indData, size_t indSize, size_t indCount) : 
			VertexArray(vertData, vertSize, indData, indSize, indCount)
		{
			glGenVertexArrays(1, &glVao);
			glBindVertexArray(glVao);

			GLuint glVbo;
			glGenBuffers(1, &glVbo);
			glBindBuffer(GL_ARRAY_BUFFER, glVbo);
			glBufferData(GL_ARRAY_BUFFER, vertSize, vertData, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, NULL);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, GL_OFFSET(sizeof(float) * 3));

			GLuint glIbo;
			glGenBuffers(1, &glIbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIbo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indData, GL_STATIC_DRAW);
		}

		GLuint glVao;
	};

	void Clear(uint8_t flags)
	{
		GLbitfield glMask = 0;

		if (flags & ClearFlags::Color)
			glMask |= GL_COLOR_BUFFER_BIT;
		if (flags & ClearFlags::Depth)
			glMask |= GL_DEPTH_BUFFER_BIT;
		if (flags & ClearFlags::Stencil)
			glMask |= GL_STENCIL_BUFFER_BIT;

		glClear(glMask);
	}

	void Execute(RenderOp& rop, int count)
	{
		ShaderProgramGL* programGL = dynamic_cast<ShaderProgramGL*>(rop.program.get());
		VertexArrayGL* vertexArrayGL = dynamic_cast<VertexArrayGL*>(rop.vertexArray.get());

		glUseProgram(programGL->glProgram);
		glUniformMatrix4fv(glGetUniformLocation(programGL->glProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(rop.MVP));

		glBindVertexArray(vertexArrayGL->glVao);
		glDrawElements(GL_TRIANGLES, vertexArrayGL->indiciesCount, GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);

		glUseProgram(0);
	}

	ProgramPtr CreateProgram(const char* vsSource, const char* fsSource)
	{
		return std::make_shared<ShaderProgramGL>(vsSource, fsSource);
	}

	TexturePtr CreateTexture(TextureDesc& desc)
	{	
		return std::make_shared<TextureGL>(desc);
	}

	VertexArrayPtr CreateVertexArray(const void* vertData, size_t vertSize, const void* indData, size_t indSize, size_t indCount)
	{
		return std::make_shared<VertexArrayGL>(vertData, vertSize, indData, indSize, indCount);
	}
}