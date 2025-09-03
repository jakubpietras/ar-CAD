#pragma once
#include "VertexArray.h"
#include "Shader.h"
#include "Primitive.h"
//#include "core/Scene/Components.h"

namespace ar
{
	struct MeshComponent;
	enum class RenderPassType
	{
		MAIN,
		SELECTION
	};

	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void Submit(const Primitive primitive, const std::shared_ptr<Shader>& shader, 
			const std::shared_ptr<VertexArray>& vertexArray, bool indexed = false, uint32_t instanceCount = 1);
		static void Submit(const Primitive primitive, const std::shared_ptr<Shader>& shader,
			uint32_t vertexCount, bool indexed = false, uint32_t instanceCount = 1);
		static void Submit(MeshComponent& mesh, bool indexed = false, uint32_t instanceCount = 1);

		static constexpr ar::mat::Vec3 SELECTION_COLOR{ 1.0f, 0.56f, 0.0f };
		static constexpr ar::mat::Vec3 HOLE_COLOR{ 0.5f, 0.5f, 1.0f };


	private:
		static std::unique_ptr<VertexArray> s_DummyVAO;
	};
}