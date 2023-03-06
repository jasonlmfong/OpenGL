#pragma once

#include "Test.h"

#include "Texture.h"
#include <array>

static const size_t MaxQuadCount = 1000;
static const size_t MaxVertexCount = MaxQuadCount * 4;
static const size_t MaxIndexCount = MaxQuadCount * 6;
static const size_t MaxTextures = 32;

namespace test {

	class TestBatchRendering : public Test
	{
	public:
		struct Vertex
		{
			glm::vec2 Position;
			glm::vec2 TexCoord;
			glm::vec4 Color;
			float TextureID;
		};

		struct Stats
		{
			uint32_t DrawCount = 0;
			uint32_t QuadCount = 0;
		};

		TestBatchRendering();
		~TestBatchRendering();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

		void BeginBatch();
		void EndBatch();
		void Flush();

		void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		void DrawQuad(const glm::vec2& position, const glm::vec2& size, const uint32_t textureID);

		const TestBatchRendering::Stats GetStats();
		void ResetStats();

	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		std::unique_ptr<Shader> m_Shader;
		unsigned int m_TextureWhite, m_Texture1, m_Texture2;

		// MVP
		glm::mat4 m_Proj, m_Model;

		//translations
		glm::vec3 m_Translation;

		glm::vec2 m_Quad1Position = { 100.0f, 100.0f };
		glm::vec2 m_Quad2Position = { 350.0f, 350.0f };

		uint32_t m_IndexCount = 0;

		Vertex* m_QuadBuffer = nullptr;
		Vertex* m_QuadBufferPtr = nullptr;

		std::array<uint32_t, MaxTextures> m_TextureSlots;
		uint32_t m_TextureSlotIndex = 1;

		// statistics
		
		Stats m_RenderStats;
	};

}