#include "TestBatchDynamicGeometry.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"

namespace test {

    struct Vertex
    {
        glm::vec2 Position;
        glm::vec2 TexCoord;
        glm::vec4 Color;
        float TextureID;
    };

    static std::vector<Vertex> CreateQuad(float x, float y, 
        float tintRed, float tintGreen, float tintBlue, float tintA, float textureID)
    {
        float size = 100.0f;

        Vertex v0{};
        v0.Position = { x, y };
        v0.TexCoord = { 0.0, 0.0 };
        v0.Color = { tintRed, tintGreen, tintBlue, tintA };
        v0.TextureID = textureID;

        Vertex v1{};
        v1.Position = { x + size, y };
        v1.TexCoord = { 1.0f, 0.0f };
        v1.Color = { tintRed, tintGreen, tintBlue, tintA };
        v1.TextureID = textureID;

        Vertex v2{};
        v2.Position = { x + size, y + size };
        v2.TexCoord = { 1.0f, 1.0f };
        v2.Color = { tintRed, tintGreen, tintBlue, tintA };
        v2.TextureID = textureID;

        Vertex v3{};
        v3.Position = { x, y + size };
        v3.TexCoord = { 0.0f, 1.0f };
        v3.Color = { tintRed, tintGreen, tintBlue, tintA };
        v3.TextureID = textureID;

        return { v0, v1, v2, v3 };
    }

    TestBatchDynamicGeometry::TestBatchDynamicGeometry()
        : m_Proj(glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f)),
        m_Model(glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
        m_Translation(300, 200, 0)
    {
        // blending
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        /*float vertices[] = {
            -50.0f, -50.0f, 0.0f, 0.0f, 0.18f, 0.60f, 0.96f, 1.0f, 0.0f, // 0
             50.0f, -50.0f, 0.5f, 0.0f, 0.18f, 0.60f, 0.96f, 1.0f, 0.0f, // 1
             50.0f,  50.0f, 0.5f, 1.0f, 0.18f, 0.60f, 0.96f, 1.0f, 0.0f, // 2
            -50.0f,  50.0f, 0.0f, 1.0f, 0.18f, 0.60f, 0.96f, 1.0f, 0.0f, // 3

            -50.0f, 250.0f, 0.0f, 0.0f, 0.91f, 0.26f, 0.21f, 1.0f, 0.0f, // 4
             50.0f, 250.0f, 0.5f, 0.0f, 0.91f, 0.26f, 0.21f, 1.0f, 0.0f, // 5
             50.0f, 350.0f, 0.5f, 1.0f, 0.91f, 0.26f, 0.21f, 1.0f, 0.0f, // 6
            -50.0f, 350.0f, 0.0f, 1.0f, 0.91f, 0.26f, 0.21f, 1.0f, 0.0f, // 7

            150.0f, 150.0f, 0.0f, 0.0f, 1.00f, 0.93f, 0.24f, 1.0f, 1.0f, // 8
            250.0f, 150.0f, 1.0f, 0.0f, 1.00f, 0.93f, 0.24f, 1.0f, 1.0f, // 9
            250.0f, 250.0f, 1.0f, 1.0f, 1.00f, 0.93f, 0.24f, 1.0f, 1.0f, // 10
            150.0f, 250.0f, 0.0f, 1.0f, 1.00f, 0.93f, 0.24f, 1.0f, 1.0f  // 11
        };*/

        m_VAO = std::make_unique<VertexArray>();

        m_VB = std::make_unique<VertexBuffer>(nullptr, 12 * sizeof(Vertex), "dynamic");
        VertexBufferLayout layout;
        layout.Push<float>(2); // window coord
        layout.Push<float>(2); // texture coord
        layout.Push<float>(4); // color
        layout.Push<float>(1); // texture ID

        // links the vertex array with the vertex buffer
        m_VAO->AddBuffer(*m_VB, layout);

        unsigned int indices[] = {
            0, 1, 2,   // triangle 1
            2, 3, 0,   // triangle 2
            4, 5, 6,   // triangle 3
            6, 7, 4,   // triangle 4
            8, 9, 10,  // triangle 5
            10, 11, 8  // triangle 6
        };

        m_IB = std::make_unique<IndexBuffer>(indices, 6 * 3);

        m_Shader = std::make_unique<Shader>("res/shaders/Multi.shader");
        m_Shader->Bind();

        // load texture
        m_Texture1 = std::make_unique<Texture>("res/textures/Penguin.png");
        m_Texture2 = std::make_unique<Texture>("res/textures/icon.png");
        int samplers[2] = { 0, 1 };
        m_Shader->SetUniform1iv("u_Textures", 2, samplers);
    }

    TestBatchDynamicGeometry::~TestBatchDynamicGeometry()
    {
    }

    void TestBatchDynamicGeometry::OnUpdate(float deltaTime)
    {
    }

    void TestBatchDynamicGeometry::OnRender()
    {
        // set dynamic vertex buffer
        /*float vertices[] = {
            -50.0f, -50.0f, 0.0f, 0.0f, 0.18f, 0.60f, 0.96f, 1.0f, 0.0f, // 0
             50.0f, -50.0f, 0.5f, 0.0f, 0.18f, 0.60f, 0.96f, 1.0f, 0.0f, // 1
             50.0f,  50.0f, 0.5f, 1.0f, 0.18f, 0.60f, 0.96f, 1.0f, 0.0f, // 2
            -50.0f,  50.0f, 0.0f, 1.0f, 0.18f, 0.60f, 0.96f, 1.0f, 0.0f, // 3

            -50.0f, 250.0f, 0.0f, 0.0f, 0.91f, 0.26f, 0.21f, 1.0f, 0.0f, // 4
             50.0f, 250.0f, 0.5f, 0.0f, 0.91f, 0.26f, 0.21f, 1.0f, 0.0f, // 5
             50.0f, 350.0f, 0.5f, 1.0f, 0.91f, 0.26f, 0.21f, 1.0f, 0.0f, // 6
            -50.0f, 350.0f, 0.0f, 1.0f, 0.91f, 0.26f, 0.21f, 1.0f, 0.0f, // 7

            150.0f, 150.0f, 0.0f, 0.0f, 1.00f, 0.93f, 0.24f, 1.0f, 1.0f, // 8
            250.0f, 150.0f, 1.0f, 0.0f, 1.00f, 0.93f, 0.24f, 1.0f, 1.0f, // 9
            250.0f, 250.0f, 1.0f, 1.0f, 1.00f, 0.93f, 0.24f, 1.0f, 1.0f, // 10
            150.0f, 250.0f, 0.0f, 1.0f, 1.00f, 0.93f, 0.24f, 1.0f, 1.0f  // 11
        };*/

        m_VB->Bind();

        std::vector<Vertex> q0 = CreateQuad(m_Quad0Position[0], m_Quad0Position[1], 0.18f, 0.60f, 0.96f, 1.0f, 0.0f);
        std::vector<Vertex> q1 = CreateQuad(m_Quad1Position[0], m_Quad1Position[1], 0.91f, 0.26f, 0.21f, 1.0f, 0.0f);
        std::vector<Vertex> q2 = CreateQuad(m_Quad2Position[0], m_Quad2Position[1], 1.00f, 0.93f, 0.24f, 1.0f, 1.0f);

        Vertex vertices[12]{};
        memcpy(vertices, q0.data(), q0.size() * sizeof(Vertex));
        memcpy(vertices + q0.size(), q1.data(), q1.size() * sizeof(Vertex));
        memcpy(vertices + 8, q2.data(), q2.size() * sizeof(Vertex));

        // load data into vertex buffer
        /*glMapBuffer();
        glUnmapBuffer();*/
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));

        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;

        m_Shader->Bind();
        m_Texture1->Bind(0);
        m_Texture2->Bind(1);

        {
            glm::mat4 view = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * view * m_Model;
            m_Shader->SetUniformMat4f("u_MVP", mvp);

            /* bind va and ib, then create a shape */
            renderer.Draw(*m_VAO, *m_IB, *m_Shader);
        }
    }

    void TestBatchDynamicGeometry::OnImGuiRender()
    {
        ImGui::SliderFloat3("Translation", &m_Translation.x, 0.0f, 1080.0f);
        ImGui::DragFloat2("Quad 1 Position", m_Quad0Position, 1.0f);
        ImGui::DragFloat2("Quad 2 Position", m_Quad1Position, 1.0f);
        ImGui::DragFloat2("Quad 3 Position", m_Quad2Position, 1.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

}