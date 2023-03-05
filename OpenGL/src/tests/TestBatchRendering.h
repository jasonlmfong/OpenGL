#pragma once

#include "Test.h"

#include "Texture.h"

namespace test {

	class TestBatchRendering : public Test
	{
	public:
		TestBatchRendering();
		~TestBatchRendering();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		std::unique_ptr<Shader> m_Shader;
		unsigned int m_Texture1, m_Texture2;

		// MVP
		glm::mat4 m_Proj, m_Model;

		//translations
		glm::vec3 m_Translation;

		glm::vec2 m_QuadPosition = { 100.0f, 100.0f };
	};

}