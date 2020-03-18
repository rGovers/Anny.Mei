#include "IntermediateRenderer.h"

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <math.h>
#include <string.h>

#include "ShaderProgram.h"
#include "Shaders/SolidPixelColor.h"
#include "Shaders/ViewVertex.h"

const static float PI2 = (M_PI * 2);

IntermediateRenderer::IntermediateRenderer()
{
	const char* str;

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

    const int vertexS = glCreateShader(GL_VERTEX_SHADER);
	str = VIEWVERTEX;
    glShaderSource(vertexS, 1, &str, nullptr);
    glCompileShader(vertexS);

    const int pixelS = glCreateShader(GL_FRAGMENT_SHADER);
	str = SOLIDPIXELCOLOR;
    glShaderSource(pixelS, 1, &str, nullptr);
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
    if (a_start == a_end)
    {
        return;
    }

    glm::vec3 up = glm::vec3(0, 0, 1);

    const glm::vec3 dir = glm::normalize(a_end - a_start);

    if (abs(glm::dot(dir, up)) >= 0.9f)
    {
        up = glm::vec3(0, 1, 0);
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

void IntermediateRenderer::DrawCircle(const glm::vec3& a_pos, int a_iteration, float a_radius, float a_width, const glm::vec4& a_color, float a_widthScale, float a_heightScale)
{
    for (int i = 0; i < a_iteration; ++i)
    {
        const float angleA = (i / (float)a_iteration) * PI2;
        const float angleB = ((i + 1) / (float)a_iteration) * PI2;

        const glm::vec2 dirA = { sin(angleA), cos(angleA) };
        const glm::vec2 dirB = { sin(angleB), cos(angleB) };

        const glm::vec3 posA = a_pos + glm::vec3(dirA.x * a_radius * a_widthScale, dirA.y * a_radius * a_heightScale, 0);
        const glm::vec3 posB = a_pos + glm::vec3(dirB.x * a_radius * a_widthScale, dirB.y * a_radius * a_heightScale, 0) * a_radius;

        DrawLine(posA, posB, a_width, a_color);
    }
}
void IntermediateRenderer::DrawSolidCircle(const glm::vec3& a_pos, int a_iteration, float a_radius, const glm::vec4& a_color, float a_widthScale, float a_heightScale)
{
    const size_t startInd = m_vertices.size();

    int prevIndex = startInd + 1;
    glm::vec2 dir = { sin(0), cos(0) };
    const glm::vec3 dirScaled = { dir.x * a_radius * a_widthScale, dir.y * a_radius * a_heightScale, 0 };

    m_vertices.emplace_back(Vertex{ glm::vec4(a_pos, 1), a_color });
    m_vertices.emplace_back(Vertex{ glm::vec4(a_pos + dirScaled, 1), a_color });

    for (int i = 1; i <= a_iteration; ++i)
    {
        const float angle = (i / (float)a_iteration) * PI2;

        const glm::vec2 dir = { sin(angle), cos(angle) };

        const glm::vec3 dirScaled = { dir.x * a_radius * a_widthScale, dir.y * a_radius * a_heightScale, 0 };

        const Vertex vert = { glm::vec4(a_pos + dirScaled, 1), a_color };

        m_vertices.emplace_back(vert);

        m_indices.emplace_back(startInd);
        m_indices.emplace_back(prevIndex++);
        m_indices.emplace_back(prevIndex);
    }
}
void IntermediateRenderer::DrawBox(const glm::vec3& a_start, const glm::vec3& a_end, float a_width, const glm::vec4& a_color)
{
    const float midZ = (a_start.z + a_end.z) * 0.5f;

    DrawLine({ a_start.x, a_start.y, a_start.z }, { a_start.x, a_end.y, midZ }, a_width, a_color);
    DrawLine({ a_start.x, a_start.y, a_start.z }, { a_end.x, a_start.y, midZ }, a_width, a_color);
    DrawLine({ a_start.x, a_end.y, midZ}, { a_end.x, a_end.y, a_end.z }, a_width, a_color);
    DrawLine({ a_end.x, a_start.y, midZ}, { a_end.x, a_end.y, a_end.z }, a_width, a_color);
}
void IntermediateRenderer::DrawArrow(const glm::vec3& a_pos, const glm::vec3& a_dir, float a_size, float a_width, const glm::vec4& a_color)
{
    glm::vec3 up = glm::vec3(0, 0, 1);

    if (abs(glm::dot(a_dir, up)) >= 0.9f)
    {
        up = glm::vec3(0, 1, 0);
    }

    const glm::vec3 right = glm::cross(up, a_dir);

    const float halfSize = a_size * 0.5f;

    const glm::vec3 pointA = a_pos + a_dir * a_size;
    const glm::vec3 pointB = a_pos + right * halfSize;
    const glm::vec3 pointC = a_pos - right * halfSize;

    DrawLine(pointA, pointB, a_width, a_color);
    DrawLine(pointA, pointC, a_width, a_color);
    DrawLine(pointB, pointC, a_width, a_color);
}

void IntermediateRenderer::Draw()
{
    const size_t indexCount = m_indices.size();
    if (indexCount > 0)
    {
        const size_t vertexCount = m_vertices.size();

        glUseProgram(m_program->GetHandle());

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
}