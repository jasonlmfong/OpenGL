#include "TestBatchDynamicGeometry.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"

#include <array>

namespace test {

    struct Vertex
    {
        glm::vec2 Position;
        glm::vec2 TexCoord;
        glm::vec4 Color;
        float TextureID;
    };

    static Vertex* CreateQuad(Vertex* target, float x, float y, 
        float tintRed, float tintGreen, float tintBlue, float tintA, float textureID)
    {
        float size = 100.0f;

        // add 1st vertex
        target->Position = { x, y };
        target->TexCoord = { 0.0, 0.0 };
        target->Color = { tintRed, tintGreen, tintBlue, tintA };
        target->TextureID = textureID;
        target++;

        // add 2nd vertex
        target->Position = { x + size, y };
        target->TexCoord = { 1.0f, 0.0f };
        target->Color = { tintRed, tintGreen, tintBlue, tintA };
        target->TextureID = textureID;
        target++;

        // add 3rd vertex
        target->Position = { x + size, y + size };
        target->TexCoord = { 1.0f, 1.0f };
        target->Color = { tintRed, tintGreen, tintBlue, tintA };
        target->TextureID = textureID;
        target++;

        // add 4th vertex
        target->Position = { x, y + size };
        target->TexCoord = { 0.0f, 1.0f };
        target->Color = { tintRed, tintGreen, tintBlue, tintA };
        target->TextureID = textureID;
        target++;

        return target;
    }

    TestBatchDynamicGeometry::TestBatchDynamicGeometry()
        : m_Proj(glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f)),
        m_Model(glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
        m_Translation(300, 200, 0)
    {
        // blending
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        const size_t MaxQuadCount = 1000;
        const size_t MaxVertexCount = MaxQuadCount * 4;
        const size_t MaxIndexCount = MaxQuadCount * 6;

        m_VAO = std::make_unique<VertexArray>();

        m_VB = std::make_unique<VertexBuffer>(nullptr, MaxVertexCount * sizeof(Vertex), "dynamic");
        VertexBufferLayout layout;
        layout.Push<float>(2); // window coord
        layout.Push<float>(2); // texture coord
        layout.Push<float>(4); // color
        layout.Push<float>(1); // texture ID

        // links the vertex array with the vertex buffer
        m_VAO->AddBuffer(*m_VB, layout);

        /*unsigned int indices[] = {
            0, 1, 2,   // triangle 1
            2, 3, 0,   // triangle 2
            4, 5, 6,   // triangle 3
            6, 7, 4,   // triangle 4
            8, 9, 10,  // triangle 5
            10, 11, 8  // triangle 6
        };*/

        uint32_t indices[MaxIndexCount];
        uint32_t offset = 0;
        for (size_t i = 0; i < MaxIndexCount; i += 6)
        {
            indices[i + 0] = 0 + offset;
            indices[i + 1] = 1 + offset;
            indices[i + 2] = 2 + offset;

            indices[i + 3] = 2 + offset;
            indices[i + 4] = 3 + offset;
            indices[i + 5] = 0 + offset;

            offset += 4;
        }

        m_IB = std::make_unique<IndexBuffer>(indices, MaxIndexCount);

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

        uint32_t indexCount = 0;

        std::array<Vertex, 1000> vertices{};
        Vertex* buffer = vertices.data();
        
        // draw grid with alternating textures
        for (int y = 0; y < 500; y += 101)
        {
            for (int x = 0; x < 500; x += 101)
            {
                buffer = CreateQuad(buffer, x, y, 1.0f, 1.0f, 1.0f, 1.0f, (x + y) % 2);
                indexCount += 6;
            }
        }

        // blue tint penguin
        buffer = CreateQuad(buffer, m_Quad0Position[0], m_Quad0Position[1], 0.18f, 0.60f, 0.96f, 1.0f, 0.0f);
        indexCount += 6;
        // red tint penguin
        buffer = CreateQuad(buffer, m_Quad1Position[0], m_Quad1Position[1], 0.91f, 0.26f, 0.21f, 1.0f, 0.0f); 
        indexCount += 6;
        // yellow tint icon
        buffer = CreateQuad(buffer, m_Quad2Position[0], m_Quad2Position[1], 1.00f, 0.93f, 0.24f, 1.0f, 1.0f);
        indexCount += 6;

        m_VB->Bind();
        // load data into vertex buffer
        /*glMapBuffer();
        glUnmapBuffer();*/
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data()));

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