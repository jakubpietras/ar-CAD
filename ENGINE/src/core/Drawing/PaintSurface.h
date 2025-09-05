#pragma once
#include "core/Core.h"
#include "core/Renderer/Texture.h"

namespace ar
{
	class PaintSurface
	{
	public:
		PaintSurface(size_t width, size_t height);
		void SetPixel(size_t x, size_t y, unsigned char red, unsigned char green, unsigned char blue);
		inline uint32_t GetHandle() const { return m_Tex->GetID(); }
		inline size_t GetWidth() const { return m_Width; }
		inline size_t GetHeight() const { return m_Height; }

	private:
		size_t m_Width, m_Height, m_BufferSize;
		std::vector<unsigned char> m_Buffer;
		Scope<Texture> m_Tex;

		void InitializeTexture(size_t width, size_t height);
		void InitializeBuffer(size_t width, size_t height);
	};
}


