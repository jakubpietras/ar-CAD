#pragma once
#include <vector>
#include "VertexTypes.h"

namespace ar
{
	template <typename T>
	class ShaderStorageBuffer
	{
	public:
		ShaderStorageBuffer() { glCreateBuffers(1, &m_ID); }
		~ShaderStorageBuffer() { glDeleteBuffers(1, &m_ID); }
		inline void Bind(uint32_t index) 
		{ 
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_ID);
		}
		void UpdateSubData(const void* data, size_t offset, size_t size)
		{
			glNamedBufferSubData(m_ID, offset, size, data);
		}
		void UpdateData(const void* data, size_t size)
		{
			glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);
			AR_GL_CHECK();
		}
		void ReadData(void* dst, size_t size)
		{
			glGetNamedBufferSubData(m_ID, 0, size, dst);
		}

	private:
		uint32_t m_ID;
	};
}
