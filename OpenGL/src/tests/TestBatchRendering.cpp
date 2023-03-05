#include "TestBatchRendering.h"

#include "Renderer.h"
#include "BatchRenderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"

#include <array>

namespace test {

    TestBatchRendering::TestBatchRendering()
        : m_Proj(glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f)),
        m_Model(glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
        m_Translation(300, 200, 0)
    {
        m_Shader = std::make_unique<Shader>("res/shaders/Multi.shader");
        m_Shader->Bind();

        int samplers[32];
        for (int i = 0; i < 32; i++)
            samplers[i] = i;
        m_Shader->SetUniform1iv("u_Textures", 32, samplers);

        BatchRenderer::Init();

        // load texture
        m_Texture1 = Texture("res/textures/Penguin.png");
        m_Texture2 = Texture("res/textures/icon.png");
    }

    TestBatchRendering::~TestBatchRendering()
    {
        BatchRenderer::Shutdown();
    }

    void TestBatchRendering::OnUpdate(float deltaTime)
    {
    }

    void TestBatchRendering::OnRender()
    {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        glm::mat4 view = glm::translate(glm::mat4(1.0f), m_Translation);
        glm::mat4 mvp = m_Proj * view * m_Model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);

        BatchRenderer::ResetStats();
        BatchRenderer::BeginBatch();

        // draw background
        for (float y = 0.0f; y < 1080.0f; y += 10.0f)
        {
            for (float x = 0.0f; x < 1080.0f; x += 10.0f)
            {
                glm::vec4 color = { (x / 108.0f), 0.2f, (y / 108.0f), 1.0f };
                BatchRenderer::DrawQuad({ x, y }, { 9.0f, 9.0f }, color);
            }
        }

        // draw grid with alternating textures
        for (int y = 0; y < 500; y += 101)
        {
            for (int x = 0; x < 500; x += 101)
            {
                unsigned int tex = (x + y) % 2 == 0 ? m_Texture1 : m_Texture2;
                BatchRenderer::DrawQuad({ x, y }, { 100.0f, 100.0f }, tex);
            }
        }

        // penguin
        BatchRenderer::DrawQuad({ 100.0f, 100.0f }, { 200.0f, 200.0f }, m_Texture1);
        BatchRenderer::EndBatch();

        BatchRenderer::Flush();
    }

    void TestBatchRendering::OnImGuiRender()
    {
        ImGui::SliderFloat3("Translation", &m_Translation.x, 0.0f, 1080.0f);
        ImGui::DragFloat2("Quad 1 Position", m_QuadPosition, 1.0f);
        ImGui::Text("Quads: %d", BatchRenderer::GetStats().QuadCount);
        ImGui::Text("Draws: %d", BatchRenderer::GetStats().DrawCount);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

}