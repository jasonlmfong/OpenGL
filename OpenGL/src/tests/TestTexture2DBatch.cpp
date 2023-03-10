#include "TestTexture2DBatch.h"

#include "Renderer.h"
#include "imgui/imgui.h"


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {

    TestTexture2DBatch::TestTexture2DBatch()
        : m_Proj(glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f)),
        m_Model(glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
        m_Translation(300, 200, 0)
    {
        float positions[] = {
            -50.0f, -50.0f, 0.0f, 0.0f, 0.18f, 0.6f, 0.96f, 1.0f, // 0
             50.0f, -50.0f, 1.0f, 0.0f, 0.18f, 0.6f, 0.96f, 1.0f, // 1
             50.0f,  50.0f, 1.0f, 1.0f, 0.18f, 0.6f, 0.96f, 1.0f, // 2
            -50.0f,  50.0f, 0.0f, 1.0f, 0.18f, 0.6f, 0.96f, 1.0f, // 3

            150.0f, 150.0f, 0.0f, 0.0f, 1.0f, 0.93f, 0.24f, 1.0f, // 4
            250.0f, 150.0f, 1.0f, 0.0f, 1.0f, 0.93f, 0.24f, 1.0f, // 5
            250.0f, 250.0f, 1.0f, 1.0f, 1.0f, 0.93f, 0.24f, 1.0f, // 6
            150.0f, 250.0f, 0.0f, 1.0f, 1.0f, 0.93f, 0.24f, 1.0f  // 7
        };

        unsigned int indices[] = {
            0, 1, 2, // triangle 1
            2, 3, 0, // triangle 2
            4, 5, 6, // triangle 3
            6, 7, 4  // triangle 4
        };

        // blending
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        m_VB = std::make_unique<VertexBuffer>(positions, sizeof(positions));
        VertexBufferLayout layout;
        layout.Push<float>(2); // window coord
        layout.Push<float>(2); // texture coord
        layout.Push<float>(4); // color

        // links the vertex array with the vertex buffer
        m_VAO->AddBuffer(*m_VB, layout);
        m_IB = std::make_unique<IndexBuffer>(indices, 12);

        m_Shader = std::make_unique<Shader>("res/shaders/TextureBatch.shader");
        m_Shader->Bind();

        // load texture
        m_Texture = std::make_unique<Texture>("res/textures/Penguin.png");
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    TestTexture2DBatch::~TestTexture2DBatch()
    {
    }

    void TestTexture2DBatch::OnUpdate(float deltaTime)
    {
    }

    void TestTexture2DBatch::OnRender()
    {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;

        m_Texture->Bind();
        m_Shader->Bind();

        {
            glm::mat4 view = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * view * m_Model;
            m_Shader->SetUniformMat4f("u_MVP", mvp);

            /* bind va and ib, then create a shape */
            renderer.Draw(*m_VAO, *m_IB, *m_Shader);
        }
    }

    void TestTexture2DBatch::OnImGuiRender()
    {
        ImGui::SliderFloat3("Translation", &m_Translation.x, 0.0f, 1080.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

}