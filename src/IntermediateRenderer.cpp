#include "IntermediateRenderer.h"

#include <glad/glad.h>
#include <string.h>

#include "ShaderProgram.h"
#include "Shaders/SolidPixelColor.h"
#include "Shaders/ViewVertex.h"

IntermediateRenderer::IntermediateRenderer()
{
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    const size_t vertexSize = sizeof(Vertex);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 4, GL_FLOAT, false, vertexSize, (void*)offsetof(Vertex, Position));
    glVertexAttribPointer(1, 4, GL_FLOAT, false, vertexSize, (void*)offsetof(Vertex, Color));

    int vertexS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexS, 1, &VIEWVERTEX, nullptr);
    glCompileShader(vertexS);

    int pixelS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(pixelS, 1, &SOLIDPIXELCOLOR, nullptr);
    glCompileShader(pixelS);

    m_program = new ShaderProgram(pixelS, vertexS);

    glDeleteShader(vertexS);
    glDeleteShader(pixelS);
}
IntermediateRenderer::~IntermediateRenderer()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
    glDeleteVertexArrays(1, &m_vao);

    delete m_program;
}

void IntermediateRenderer::Reset()
{
    m_vertices.clear();
    m_indices.clear();
}

void IntermediateRenderer::DrawLine(const glm::vec3& a_start, const glm::vec3& a_end, float a_width, const glm::vec4& a_color)
{
    glm::vec3 up = glm::vec3(0, 1, 0);

    const glm::vec3 dir = glm::normalize(a_end - a_start);

    if (glm::dot(dir, up) >= 0.9f)
    {
        up = glm::vec3(0, 0, 1);
    }

    const glm::vec3 right = glm::cross(up, dir);

    const float halfWidth = a_width * 0.5f;

    const Vertex a = { glm::vec4(a_start + right * halfWidth, 1), a_color };
    const Vertex b = { glm::vec4(a_start - right * halfWidth, 1), a_color };
    const Vertex c = { glm::vec4(a_end + right * halfWidth, 1), a_color };
    const Vertex d = { glm::vec4(a_end - right * halfWidth, 1), a_color };

    const size_t startInd = m_vertices.size();

    m_vertices.emplace_back(a);
    m_vertices.emplace_back(b);
    m_vertices.emplace_back(c);
    m_vertices.emplace_back(d);

    m_indices.emplace_back(startInd + 0);
    m_indices.emplace_back(startInd + 1);
    m_indices.emplace_back(startInd + 2);

    m_indices.emplace_back(startInd + 1);
    m_indices.emplace_back(startInd + 3);
    m_indices.emplace_back(startInd + 2);
}

void IntermediateRenderer::Draw()
{
    const size_t indexCount = m_indices.size();
    if (indexCount > 0)
    {
        glUseProgram(m_program->GetHandle());

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices.front(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &m_indices.front(), GL_STATIC_DRAW);

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
}