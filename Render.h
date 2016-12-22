#pragma once

#include <cstdint>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <map>
#include <memory>

#include "RenderTypes.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"

struct Camera;

namespace Render
{
	struct RenderOp
	{
		glm::mat4 MVP;

		// TMP
		ProgramPtr program;
		VertexArrayPtr vertexArray;
	};

	struct Frame
	{
		glm::mat4 modelMatrix;
	};

	struct RenderObject
	{
		void GatherRenderOps(/*RenderOp *& pRops, int& numRops, RenderMode rm*/) {}

		Frame frame;

		// TMP : hardcoded renderOP
		RenderOp rop;

		// TMP!!
		RenderObject(VertexArrayPtr va, ProgramPtr pr) : vertexarray(va), program(pr) {}
		
		VertexArrayPtr vertexarray;
		ProgramPtr program;
	};

	struct RenderQueue
	{
		void PushRenderObj(RenderObject* robj, glm::mat4 viewProjMat);

		void Reset();

		void Sort() {}
		void Render();

	private:
		std::vector<RenderOp> renderOps;
	};

	//
	// Render Device
	// 
	void Clear(uint8_t flags);
	void Execute(RenderOp& rop, int count);

	ProgramPtr CreateProgram(const char* vsSource, const char* fsSource);
	TexturePtr CreateTexture(TextureDesc& desc);
	VertexArrayPtr CreateVertexArray(const void* vertData, size_t vertSize, const void* indData, size_t indSize, size_t indCount);

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