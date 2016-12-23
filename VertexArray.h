#pragma once

#include <array>
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

enum struct ElementType : uint16_t
{
	Float,
	Float2,
	Float3,
	Float4
};

enum struct ElementUsage : uint16_t
{
	Position,
	Uv
};

struct VertexComponent
{
	VertexComponent();
	VertexComponent(ElementUsage u, ElementType t) : usage(u), type(t) {}

	ElementUsage Usage() const { return usage; }
	ElementType Type() const { return type; }
	int32_t GetByteSize() const 
	{ 
		switch (type)
		{
		case ElementType::Float:
			return 4;
			break;
		case ElementType::Float2:
			return 8;
			break;
		case ElementType::Float3:
			return 12;
			break;
		case ElementType::Float4:
			return 16;
			break;
		default:
			break;
		}
	}

private:
	ElementUsage usage;
	ElementType type;
};

struct VertexFormat
{
	VertexFormat() {}

	void Add(const VertexComponent& comp)
	{
		int32_t num = this->GetNumComponents();
		comps[num++] = comp;
	}
	void Add(ElementUsage u, ElementType t)
	{
		Add(VertexComponent(u, t));
	}

	int32_t GetNumComponents() const { return num; }
	const VertexComponent& GetComponent(int32_t index) const { return comps[index]; }

	int32_t GetByteSize() const
	{
		int32_t byteSize = 0;
		for (int32_t i = 0; i < num; i++) 
		{
			byteSize += comps[i].GetByteSize();
		}
		return byteSize;
	}
	int32_t GetComponentByteOffset(int32_t index) const
	{
		int32_t byteOffset = 0;
		for (int32_t i = 0; i < index; i++) 
		{
			byteOffset += comps[i].GetByteSize();
		}
		return byteOffset;
	}

private:
	enum
	{
		MaxNumVertexComponents
	};

	std::array<VertexComponent, MaxNumVertexComponents> comps;
	int32_t num;
};

struct VertexArray
{
	VertexArray(const void* vertData, size_t vertSize, const void* indData, size_t indSize, size_t indCount) : indiciesCount(indCount){}
	virtual ~VertexArray() {}
	uint32_t indiciesCount;
};

typedef std::shared_ptr<VertexArray> VertexArrayPtr;


