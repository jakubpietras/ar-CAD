#include "arpch.h"
#include "PaintSurface.h"
#include <stack>

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

	void PaintSurface::DrawLine(int x0, int y0, int x1, int y1, unsigned char red, unsigned char green, unsigned char blue)
	{
		int dx = abs(x1 - x0);
		int dy = abs(y1 - y0);
		int sx = x0 < x1 ? 1 : -1;
		int sy = y0 < y1 ? 1 : -1;
		int err = dx - dy;

		int x = x0, y = y0;

		while (true)
		{
			if (x >= 0 && x < (int)m_Width && y >= 0 && y < (int)m_Height)
			{
				size_t index = (y * m_Width + x) * 4;
				m_Buffer[index] = red;
				m_Buffer[index + 1] = green;
				m_Buffer[index + 2] = blue;
				m_Buffer[index + 3] = 255;
			}

			if (x == x1 && y == y1) break;

			int e2 = 2 * err;
			if (e2 > -dy)
			{
				err -= dy;
				x += sx;
			}
			if (e2 < dx)
			{
				err += dx;
				y += sy;
			}
		}
		m_Tex->SetData(m_Buffer.data(), m_BufferSize);
	}

	void PaintSurface::FloodFill(int x, int y, unsigned char red, unsigned char green, unsigned char blue)
	{
		if (!IsValidPixel(x, y)) return;

		size_t index = (y * m_Width + x) * 4;
		unsigned char origR = m_Buffer[index];
		unsigned char origG = m_Buffer[index + 1];
		unsigned char origB = m_Buffer[index + 2];

		if (origR == red && origG == green && origB == blue) return;

		std::stack<std::pair<int, int>> stack;
		stack.push({ x, y });

		while (!stack.empty())
		{
			auto [px, py] = stack.top();
			stack.pop();

			if (!IsValidPixel(px, py) || !IsSameColor(px, py, origR, origG, origB))
				continue;

			SetPixelUnchecked(px, py, red, green, blue);
			stack.push({ px + 1, py });
			stack.push({ px - 1, py });
			stack.push({ px, py + 1 });
			stack.push({ px, py - 1 });
		}
		m_Tex->SetData(m_Buffer.data(), m_BufferSize);
	}

	void PaintSurface::InitializeTexture(size_t width, size_t height)
	{
		TextureDesc desc {};
		desc.Width = width;
		desc.Height = height;
		desc.Format = TextureFormat::RGBA8;
		m_Tex = Ref<Texture>(Texture::Create(desc));
	}

	void PaintSurface::InitializeBuffer(size_t width, size_t height)
	{
		m_BufferSize = width * height * 4;
		m_Buffer = std::vector<unsigned char>(m_BufferSize, 255);
	}

	bool PaintSurface::IsValidPixel(int x, int y) const
	{
		return x >= 0 && x < (int)m_Width && y >= 0 && y < (int)m_Height;
	}

	bool PaintSurface::IsSameColor(int x, int y, unsigned char r, unsigned char g, unsigned char b) const
	{
		size_t index = (y * m_Width + x) * 4;
		return m_Buffer[index] == r &&
			m_Buffer[index + 1] == g &&
			m_Buffer[index + 2] == b;
	}

	void PaintSurface::SetPixelUnchecked(int x, int y, unsigned char red, unsigned char green, unsigned char blue)
	{
		size_t index = (y * m_Width + x) * 4;
		m_Buffer[index] = red;
		m_Buffer[index + 1] = green;
		m_Buffer[index + 2] = blue;
		m_Buffer[index + 3] = 255;
	}
}