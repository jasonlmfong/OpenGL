#pragma once

#include<glm/glm.hpp>

class BatchRenderer
{
public:
    static void Init();
    static void Shutdown();

    static void BeginBatch();
    static void EndBatch();
    static void Flush();

    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const uint32_t textureID);

    // statistics
    struct Stats
    {
        uint32_t DrawCount = 0;
        uint32_t QuadCount = 0;
    };

    static const Stats& GetStats();
    static void ResetStats();
};