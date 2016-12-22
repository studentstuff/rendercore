#pragma once

#include <cstdint>
#include <glm.hpp>

enum struct PrimitiveType : uint8_t
{
	Triangles,
};

enum ClearFlags : uint8_t
{
	Color = 1 << 0,
	Depth = 1 << 1,
	Stencil = 1 << 2,
	All = Color | Depth | Stencil
};

enum struct RenderMode : uint8_t
{
	Default
};

enum struct TextureFilter : uint8_t
{
	Linear
};

enum struct TextureFormat : uint8_t
{
	Bgr,
	Bgra,

	Rgb8,
	Rgba8
};

enum struct TextureWrap : uint8_t
{
	ClampToEdge
};