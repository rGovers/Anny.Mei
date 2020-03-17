#include "ShaderProgram.h"

#include <glad/glad.h>
#include <stdio.h>

ShaderProgram::ShaderProgram(int a_pixelShader, int a_vertexShader)
{
    m_program = glCreateProgram();

    glAttachShader(m_program, a_pixelShader);
    glAttachShader(m_program, a_vertexShader);

    glLinkProgram(m_program);

    char* buff = new char[1024];
    glGetProgramInfoLog(m_program, 1024, 0, buff);
    if (buff[0] != 0)
    {
        printf("Failed to compile shader: %s \n", buff);
    }
    delete[] buff;
}
ShaderProgram::ShaderProgram(int a_pixelShader, int a_geometryShader, int a_vertexShader)
{
    m_program = glCreateProgram();

    glAttachShader(m_program, a_pixelShader);
    glAttachShader(m_program, a_geometryShader);
    glAttachShader(m_program, a_vertexShader);

    glLinkProgram(m_program);

    char* buff = new char[1024];
    glGetProgramInfoLog(m_program, 1024, 0, buff);
    if (buff[0] != 0)
    {
        printf("Failed to compile shader: %s \n", buff);
    }
    delete[] buff;
}
ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_program);
}

unsigned int ShaderProgram::GetHandle() const
{
    return m_program;
}