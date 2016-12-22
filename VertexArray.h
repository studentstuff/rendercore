#pragma once

#include <glm.hpp>
#include <memory>

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

struct VertexArray
{
	VertexArray(const void* vertData, size_t vertSize, const void* indData, size_t indSize, size_t indCount) : indiciesCount(indCount){}
	virtual ~VertexArray() {}
	uint32_t indiciesCount;
};

typedef std::shared_ptr<VertexArray> VertexArrayPtr;


