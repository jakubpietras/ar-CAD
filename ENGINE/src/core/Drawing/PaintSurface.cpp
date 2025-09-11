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

	void PaintSurface::DrawLine(int x0, int y0, int x1, int y1, unsigned char red, unsigned char green, unsigned char blue, bool wrapU, bool wrapV)
	{
		bool wrapX = false;
		bool wrapY = false;

		// Check if we should wrap in X direction (U parameter)
		if (wrapU && abs(x1 - x0) > (int)m_Width / 2) {
			wrapX = true;
			if (x1 > x0)
				x0 += m_Width;  // Shift start point
			else
				x1 += m_Width;  // Shift end point
		}

		// Check if we should wrap in Y direction (V parameter)  
		if (wrapV && abs(y1 - y0) > (int)m_Height / 2) {
			wrapY = true;
			if (y1 > y0)
				y0 += m_Height;
			else
				y1 += m_Height;
		}

		// Bresenham's algorithm with wrapping
		int dx = abs(x1 - x0);
		int dy = abs(y1 - y0);
		int sx = x0 < x1 ? 1 : -1;
		int sy = y0 < y1 ? 1 : -1;
		int err = dx - dy;

		int x = x0, y = y0;

		while (true)
		{
			// Wrap coordinates back to valid texture range
			int xi = x % (int)m_Width;
			int yi = y % (int)m_Height;
			if (xi < 0) xi += m_Width;
			if (yi < 0) yi += m_Height;

			// Draw pixel if in bounds
			if (xi >= 0 && xi < (int)m_Width && yi >= 0 && yi < (int)m_Height)
			{
				size_t index = (yi * m_Width + xi) * 4;
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

	void PaintSurface::FloodFill(int x, int y, unsigned char red, unsigned char green, unsigned char blue, bool wrapU, bool wrapV)
	{
		if (!IsValidPixel(x, y)) return;

		size_t index = (y * m_Width + x) * 4;
		unsigned char origR = m_Buffer[index];
		unsigned char origG = m_Buffer[index + 1];
		unsigned char origB = m_Buffer[index + 2];

		if (origR == red && origG == green && origB == blue) return;

		std::vector<bool> visited(m_Width * m_Height, false);
		std::stack<std::pair<int, int>> stack;
		stack.push({ x, y });
		visited[y * m_Width + x] = true;

		while (!stack.empty())
		{
			auto [px, py] = stack.top();
			stack.pop();

			if (!IsValidPixel(px, py) || !IsSameColor(px, py, origR, origG, origB))
				continue;

			SetPixelUnchecked(px, py, red, green, blue);

			// Add 4-connected neighbors with wrapping support
			int neighbors[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

			for (int i = 0; i < 4; i++)
			{
				int nx = px + neighbors[i][0];
				int ny = py + neighbors[i][1];

				// Handle wrapping in U direction
				if (wrapU && (nx < 0 || nx >= (int)m_Width)) {
					nx = (nx + m_Width) % m_Width;
					if (nx < 0) nx += m_Width;
				}

				// Handle wrapping in V direction
				if (wrapV && (ny < 0 || ny >= (int)m_Height)) {
					ny = (ny + m_Height) % m_Height;
					if (ny < 0) ny += m_Height;
				}

				// Check bounds and visited status
				if (nx >= 0 && nx < (int)m_Width && ny >= 0 && ny < (int)m_Height)
				{
					int nidx = ny * m_Width + nx;
					if (!visited[nidx] && IsSameColor(nx, ny, origR, origG, origB))
					{
						visited[nidx] = true;
						stack.push({ nx, ny });
					}
				}
			}
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