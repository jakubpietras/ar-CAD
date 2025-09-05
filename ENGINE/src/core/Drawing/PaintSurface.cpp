#include "arpch.h"
#include "PaintSurface.h"

namespace ar
{
	PaintSurface::PaintSurface(size_t width, size_t height)
		: m_Width(width), m_Height(height), m_BufferSize(0)
	{
		InitializeTexture(width, height);
		InitializeBuffer(width, height);
		m_Tex->SetData(m_Buffer.data(), m_BufferSize);
	}

	void PaintSurface::SetPixel(size_t x, size_t y, unsigned char red, unsigned char green, unsigned char blue)
	{
		size_t index = (y * m_Width + x) * 4;
		m_Buffer[index] = red;
		m_Buffer[index + 1] = green;
		m_Buffer[index + 2] = blue;
		m_Buffer[index + 3] = 255;

		m_Tex->SetData(m_Buffer.data(), m_BufferSize);
	}

	void PaintSurface::InitializeTexture(size_t width, size_t height)
	{
		TextureDesc desc {};
		desc.Width = width;
		desc.Height = height;
		desc.Format = TextureFormat::RGBA8;
		m_Tex = Scope<Texture>(Texture::Create(desc));
	}

	void PaintSurface::InitializeBuffer(size_t width, size_t height)
	{
		m_BufferSize = width * height * 4;
		m_Buffer = std::vector<unsigned char>(m_BufferSize, 255);
	}

}