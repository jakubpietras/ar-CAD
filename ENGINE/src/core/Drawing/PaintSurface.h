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
		void DrawLine(int x0, int y0, int x1, int y1, unsigned char red, unsigned char green, unsigned char blue, bool wrapU, bool wrapV);
		void FloodFill(int x, int y, unsigned char red, unsigned char green, unsigned char blue, bool wrapU, bool wrapV);
		bool IsSameColor(int x, int y, unsigned char r, unsigned char g, unsigned char b) const;
		
		inline uint32_t GetHandle() const { return m_Tex->GetID(); }
		inline size_t GetWidth() const { return m_Width; }
		inline size_t GetHeight() const { return m_Height; }


	private:
		size_t m_Width, m_Height, m_BufferSize;
		std::vector<unsigned char> m_Buffer;
		Ref<Texture> m_Tex;

		void InitializeTexture(size_t width, size_t height);
		void InitializeBuffer(size_t width, size_t height);

		bool IsValidPixel(int x, int y) const;
		void SetPixelUnchecked(int x, int y, unsigned char red, unsigned char green, unsigned char blue);
	};
}


