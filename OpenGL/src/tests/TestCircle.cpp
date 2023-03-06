#include "TestCircle.h"

#include "Renderer.h"
#include "imgui/imgui.h"


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {

    TestCircle::TestCircle()
        : m_Proj(glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f)),
        m_Model(glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
        m_Translation1(300, 200, 0), m_Translation2(600, 300, 0), m_Green(0.0f), m_Increment(0.05f)
    {
        float positions[] = {
            -50.0f, -50.0f, 0.0f, 0.0f, // 0
             50.0f, -50.0f, 1.0f, 0.0f, // 1
             50.0f,  50.0f, 1.0f, 1.0f, // 2
            -50.0f,  50.0f, 0.0f, 1.0f  // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // blending
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        m_VB = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        // links the vertex array with the vertex buffer
        m_VAO->AddBuffer(*m_VB, layout);
        m_IB = std::make_unique<IndexBuffer>(indices, 6);

        m_Shader = std::make_unique<Shader>("res/shaders/Circle.shader");
        m_Shader->Bind();
        m_Shader->SetUniform1f("u_Thickness", 0.8f);
        m_Shader->SetUniform4f("u_Color", 0.26f, 0.52f, 0.96f, 1.0f);

        // load texture
        uint32_t color = 0xffffffff;
        m_Texture = std::make_unique<Texture>(color);
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    TestCircle::~TestCircle()
    {
    }

    void TestCircle::OnUpdate(float deltaTime)
    {
    }

    void TestCircle::OnRender()
    {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;

        m_Texture->Bind();
        m_Shader->Bind();
        m_Shader->SetUniform4f("u_Color", 0.26f, m_Green, 0.96f, 1.0f);

        {
            glm::mat4 view = glm::translate(glm::mat4(1.0f), m_Translation1);
            glm::mat4 mvp = m_Proj * view * m_Model;
            m_Shader->SetUniformMat4f("u_MVP", mvp);

            /* bind va and ib, then create a shape */
            renderer.Draw(*m_VAO, *m_IB, *m_Shader);
        }

        /* draw a second shape using a second MVP*/
        {
            glm::mat4 view = glm::translate(glm::mat4(1.0f), m_Translation2);
            glm::mat4 mvp = m_Proj * view * m_Model;
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IB, *m_Shader);
        }

        if (m_Green > 1.0f)
            m_Increment = -0.5f;
        else if (m_Green < 0.0f)
            m_Increment = 0.5f;

        m_Green += m_Increment;
    }

    void TestCircle::OnImGuiRender()
    {
        ImGui::SliderFloat3("Translation 1", &m_Translation1.x, 0.0f, 1080.0f);
        ImGui::SliderFloat3("Translation 2", &m_Translation2.x, 0.0f, 1080.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

}