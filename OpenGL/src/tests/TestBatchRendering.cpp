#include "TestBatchRendering.h"

#include "stb_image/stb_image.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

namespace test {

    static unsigned int LoadTexture(const std::string& path)
    {
        int Width, Height, BPP;

        stbi_set_flip_vertically_on_load(1);
        unsigned char* pixels = stbi_load(path.c_str(), &Width, &Height, &BPP, 4);

        unsigned int textureID;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        if (pixels)
            stbi_image_free(pixels);

        return textureID;
    }

    TestBatchRendering::TestBatchRendering()
        : m_Proj(glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f)),
        m_Model(glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f))),
        m_Translation(0, 0, 0),
        m_IndexCount(0), m_QuadBuffer(nullptr), m_QuadBufferPtr(nullptr), 
        m_TextureSlotIndex(1)
    {
        // statistics
        Stats m_RenderStats;

        m_Shader = std::make_unique<Shader>("res/shaders/BatchRender.shader");
        m_Shader->Bind();

        int samplers[32]{};
        for (int i = 0; i < 32; i++)
            samplers[i] = i;
        m_Shader->SetUniform1iv("u_Textures", 32, samplers);

        m_QuadBuffer = new Vertex[MaxVertexCount];

        m_VAO = std::make_unique<VertexArray>();

        m_VB = std::make_unique<VertexBuffer>(nullptr, MaxVertexCount * sizeof(Vertex), "dynamic");

        VertexBufferLayout layout;
        layout.Push<float>(2); // window coord
        layout.Push<float>(2); // texture coord
        layout.Push<float>(4); // color
        layout.Push<float>(1); // texture ID
        // links the vertex array with the vertex buffer
        m_VAO->AddBuffer(*m_VB, layout);

        uint32_t indices[MaxIndexCount]{};
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


        // 1x1 white texture
        glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureWhite);
        glBindTexture(GL_TEXTURE_2D, m_TextureWhite);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        uint32_t color = 0xffffffff;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

        m_TextureSlots[0] = m_TextureWhite;
        for (size_t i = 1; i < MaxTextures; i++)
            m_TextureSlots[i] = 0;

        // load texture
        m_Texture1 = LoadTexture("res/textures/Penguin.png");
        m_Texture2 = LoadTexture("res/textures/icon.png");
    }

    TestBatchRendering::~TestBatchRendering()
    {
    }

    void TestBatchRendering::OnUpdate(float deltaTime)
    {
    }

    void TestBatchRendering::OnRender()
    {
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        m_Shader->Bind();

        glm::mat4 view = glm::translate(glm::mat4(1.0f), m_Translation);
        glm::mat4 mvp = m_Proj * view * m_Model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);

        TestBatchRendering::ResetStats();
        TestBatchRendering::BeginBatch();

        // draw background
        for (float y = 0.0f; y < 1080.0f; y += 10.0f)
        {
            for (float x = 0.0f; x < 1080.0f; x += 10.0f)
            {
                glm::vec4 color = { (x / 108.0f), 0.2f, (y / 108.0f), 1.0f };
                TestBatchRendering::DrawQuad({ x, y }, { 9.0f, 9.0f }, color);
            }
        }

        // draw grid with alternating textures
        for (int y = 0; y < 500; y += 101)
        {
            for (int x = 0; x < 500; x += 101)
            {
                unsigned int tex = (x + y) % 2 == 0 ? m_Texture1 : m_Texture2;
                TestBatchRendering::DrawQuad({ x, y }, { 100.0f, 100.0f }, tex);
            }
        }

        // penguin
        TestBatchRendering::DrawQuad(m_Quad1Position, { 200.0f, 200.0f }, m_Texture1);
        // icon
        TestBatchRendering::DrawQuad(m_Quad2Position, { 450.0f, 450.0f }, m_Texture2);

        TestBatchRendering::EndBatch();

        TestBatchRendering::Flush();
    }

    void TestBatchRendering::OnImGuiRender()
    {
        ImGui::SliderFloat3("Translation", &m_Translation.x, 0.0f, 1080.0f);
        ImGui::DragFloat2("Quad 1 Position", &m_Quad1Position[0], 1.0f);
        ImGui::DragFloat2("Quad 2 Position", &m_Quad2Position[0], 1.0f);
        ImGui::Text("Quads: %d", TestBatchRendering::GetStats().QuadCount);
        ImGui::Text("Draws: %d", TestBatchRendering::GetStats().DrawCount);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    void TestBatchRendering::BeginBatch()
    {
        m_QuadBufferPtr = m_QuadBuffer;
    }

    void TestBatchRendering::EndBatch()
    {
        GLsizeiptr size = (uint8_t*)m_QuadBufferPtr - (uint8_t*)m_QuadBuffer;
        m_VB->Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, m_QuadBuffer);
    }

    void TestBatchRendering::Flush()
    {
        for (uint32_t i = 0; i < m_TextureSlotIndex; i++)
            glBindTextureUnit(i, m_TextureSlots[i]);

        m_VAO->Bind();
        glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
        m_RenderStats.DrawCount++;

        m_IndexCount = 0;
        m_TextureSlotIndex = 1;
    }

    void TestBatchRendering::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        if (m_IndexCount >= MaxIndexCount)
        {
            EndBatch();
            Flush();
            BeginBatch();
        }

        // default no texture for pure color rendering
        float textureIndex = 0.0f;

        m_QuadBufferPtr->Position = { position.x, position.y };
        m_QuadBufferPtr->TexCoord = { 0.0f, 0.0f };
        m_QuadBufferPtr->Color = color;
        m_QuadBufferPtr->TextureID = textureIndex;
        m_QuadBufferPtr++;

        m_QuadBufferPtr->Position = { position.x + size.x, position.y };
        m_QuadBufferPtr->TexCoord = { 1.0f, 0.0f };
        m_QuadBufferPtr->Color = color;
        m_QuadBufferPtr->TextureID = textureIndex;
        m_QuadBufferPtr++;

        m_QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
        m_QuadBufferPtr->TexCoord = { 1.0f, 1.0f };
        m_QuadBufferPtr->Color = color;
        m_QuadBufferPtr->TextureID = textureIndex;
        m_QuadBufferPtr++;

        m_QuadBufferPtr->Position = { position.x, position.y + size.y };
        m_QuadBufferPtr->TexCoord = { 0.0f, 1.0f };
        m_QuadBufferPtr->Color = color;
        m_QuadBufferPtr->TextureID = textureIndex;
        m_QuadBufferPtr++;

        m_IndexCount += 6;
        m_RenderStats.QuadCount++;
    }

    void TestBatchRendering::DrawQuad(const glm::vec2& position, const glm::vec2& size, const uint32_t textureID)
    {
        if (m_IndexCount >= MaxIndexCount || m_TextureSlotIndex > 31)
        {
            EndBatch();
            Flush();
            BeginBatch();
        }

        // default no tint for texture
        constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

        // check if hte texture is used
        float textureIndex = 0.0f;
        for (uint32_t i = 1; i < m_TextureSlotIndex; i++)
        {
            if (m_TextureSlots[i] == textureID)
            {
                textureIndex = (float)i;
                break;
            }
        }

        // if texture has not been used, save it
        if (textureIndex == 0.0f)
        {
            textureIndex = (float)m_TextureSlotIndex;
            m_TextureSlots[m_TextureSlotIndex] = textureID;
            m_TextureSlotIndex++;
        }

        m_QuadBufferPtr->Position = { position.x, position.y };
        m_QuadBufferPtr->TexCoord = { 0.0f, 0.0f };
        m_QuadBufferPtr->Color = color;
        m_QuadBufferPtr->TextureID = textureIndex;
        m_QuadBufferPtr++;

        m_QuadBufferPtr->Position = { position.x + size.x, position.y };
        m_QuadBufferPtr->TexCoord = { 1.0f, 0.0f };
        m_QuadBufferPtr->Color = color;
        m_QuadBufferPtr->TextureID = textureIndex;
        m_QuadBufferPtr++;

        m_QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
        m_QuadBufferPtr->TexCoord = { 1.0f, 1.0f };
        m_QuadBufferPtr->Color = color;
        m_QuadBufferPtr->TextureID = textureIndex;
        m_QuadBufferPtr++;

        m_QuadBufferPtr->Position = { position.x, position.y + size.y };
        m_QuadBufferPtr->TexCoord = { 0.0f, 1.0f };
        m_QuadBufferPtr->Color = color;
        m_QuadBufferPtr->TextureID = textureIndex;
        m_QuadBufferPtr++;

        m_IndexCount += 6;
        m_RenderStats.QuadCount++;
    }

    const test::TestBatchRendering::Stats test::TestBatchRendering::GetStats()
    {
        return m_RenderStats;
    }

    void TestBatchRendering::ResetStats()
    {
        memset(&m_RenderStats, 0, sizeof(Stats));
    }

}