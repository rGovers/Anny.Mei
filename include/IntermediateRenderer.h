#pragma once

#include <glm/glm.hpp>
#include <vector>

class ShaderProgram;

class IntermediateRenderer
{
private:
    struct Vertex
    {
        glm::vec4 Position;
        glm::vec4 Color;
    };

    ShaderProgram*            m_program;

    unsigned int              m_vao;
    unsigned int              m_vbo;
    unsigned int              m_ibo;

    std::vector<unsigned int> m_indices;
    std::vector<Vertex>       m_vertices;
protected:

public:
    IntermediateRenderer();
    ~IntermediateRenderer();

    void Reset();

    void DrawLine(const glm::vec3& a_start, const glm::vec3& a_end, float a_width, const glm::vec4& a_color = glm::vec4(0, 1, 0, 1));
    void DrawCircle(const glm::vec3& a_pos, int a_iteration, float a_radius, float a_width, const glm::vec4& a_color = glm::vec4(1, 0, 0, 1), float a_widthScale = 1.0f, float a_heightScale = 1.0f);
    void DrawSolidCircle(const glm::vec3& a_pos, int a_iteration, float a_radius, const glm::vec4& a_color = glm::vec4(0, 1, 0, 1), float a_widthScale = 1.0f, float a_heightScale = 1.0f);
    void DrawBox(const glm::vec3& a_start, const glm::vec3& a_end, float a_width, const glm::vec4& a_color = glm::vec4(1, 0, 0, 1));
    void DrawArrow(const glm::vec3& a_pos, const glm::vec3& a_dir, float a_size, float a_width, const glm::vec4& a_color = glm::vec4(0, 0, 1, 1));

    void Draw();
};