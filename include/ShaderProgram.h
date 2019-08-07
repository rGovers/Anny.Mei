#pragma once

class ShaderProgram
{
private:
    unsigned int m_program;
protected:

public:
    ShaderProgram() = delete;
    ShaderProgram(int a_pixelShader, int a_vertexShader);
    ~ShaderProgram();

    unsigned int GetHandle() const;
};