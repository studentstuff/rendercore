
namespace Render
{
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
}
