#pragma once

#include <memory>
#include <vector>

#include "RenderTypes.h"

struct TextureDesc
{
	int width = 0;
	int height = 0;

	TextureFilter filter;
	TextureFormat format;
	TextureFormat formatInternal;
	TextureWrap wrap;

	uint8_t* data;
};

struct Texture
{
	Texture(TextureDesc& desc) {}
};
typedef std::shared_ptr<Texture> TexturePtr;
