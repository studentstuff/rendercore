#pragma once

#include <cstdint>
#include <gl\glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>

struct Camera;

namespace Render
{
	enum struct PrimitiveType : uint8_t
	{
		Triangles,
	};

	enum ClearFlags : uint8_t
	{
		Color =   1 << 0,
		Depth =   1 << 1,
		Stencil = 1 << 2,
		All = Color | Depth | Stencil
	};

	struct RenderOp
	{
		glm::mat4 MVP;
		uint32_t indCount;

		// TMP
		GLuint vao;
		GLuint program;
	};

	enum struct RenderMode : uint8_t
	{
		Default
	};

	struct Frame
	{
		glm::mat4 modelMatrix;
	};

	struct RenderObject
	{
		void GatherRenderOps(/*RenderOp*& pRops, int& numRops, RenderMode rm*/) {}

		Frame frame;

		// TMP : hardcoded renderOP
		RenderOp rop;

		// TMP!!
		RenderObject(GLuint vao, GLuint program, uint32_t icount) : glVao(vao), glProgram(program), indCount(icount) {}
		GLuint glVao;
		GLuint glProgram;

		uint32_t indCount;
	};

	struct RenderQueue
	{
		void PushRenderObj(RenderObject* p, glm::mat4 viewProjMat);

		void Reset();

		void Sort() {}
		void Render();

	private:
		std::vector<RenderOp> renderObjects;
	};

	//
	// Render Device
	// 
	void Clear(uint8_t flags);
	void Execute(RenderOp& rop, int count);

	//
	// Render Manager
	//

	void RenderAll(Camera& cam, std::vector<RenderObject*> objects);
} 

struct Camera
{
	bool FrustumTest(Render::RenderObject* obj)
	{
		return true;
	}

	glm::mat4 projMat;
	glm::mat4 viewMat;
};
